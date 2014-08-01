
#include"utils.h"
#include"play.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<signal.h>
#include<time.h>
#include<errno.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<unistd.h>



PLAY_INFO_S play_info;


PL_HEAD_S*play_get_current_pl()
{

    if(!play_info.curplaylist)
        fprintf(stderr,"WARNING:PLAY has not be assigned a playlist !!\n");
    return play_info.curplaylist;


}

PL_MUSIC_S*play_get_current_music()
{

    return play_info.curmusic;

}

const char*play_get_current_music_name()
{
    PL_MUSIC_S*m=play_get_current_music();
    if(m)
        return m->musicname;
    else
        return "NOMUSIC";

}


void play_list(PL_HEAD_S*pl)
{
	if(!pl)
		return;
    play_info.curplaylist=pl;
    
    PL_MUSIC_S*music=play_get_next_music();
    play_music(music);
    

}



void play_set_current_pl(PL_HEAD_S*pl)
{
    play_info.curplaylist=pl;

}

void play_init()
{
    play_info.playmode=PLAY_MODE_DEFAULT;
    play_info.curplaylist=pl_pool_get_default_list();
	play_info.curmusic=NULL;
	play_info.playstatus=PLAY_STATUS_STOP;
	play_info.player_id=0;
    play_init_signal();

}


void play_kill_player()
{
    fprintf(stderr,"Pretend to kill Player(%d)\n",play_info.player_id);
    kill(play_info.player_id,SIGKILL);

}



PLAY_STATUS_S play_get_status()
{

    return play_info.playstatus;
}

void play_music(PL_MUSIC_S*music)
{
    if(!music)
        return ;

    play_info.playstatus=PLAY_STATUS_PLAY;
    pid_t pid;
    if((pid=fork())==-1){
        fprintf(stderr,"ERROR::Fork Player\n");
        return;
    }
    if(pid==0){
        // Child process[Player]
        char*mp3path=music->musicpath;
//        char pbuf[100];
//        fprintf(stderr,"To Open [%s]... CWD:<%s>\n",mp3path,getcwd(pbuf,100));
        execlp("player","player",mp3path,NULL);
//        execl("./player","player",mp3path,NULL);
    
    }else{
        //here is parent process;
        play_info.player_id=pid;
        play_info.playstatus=PLAY_STATUS_PLAY;
        play_info.curmusic=music;
        printf("Current Playing:: \033[0;32m\033[1m%s\033[0m::\n",music->filename);
        
    }

}



void player_over(int signo)
{

//    fprintf(stderr,"PLAY Over:%s:no:%d \n",strsignal(signo),signo);
    int stat;
	int rval;
    while((rval=waitpid(-1,&stat,WUNTRACED|WCONTINUED|WNOHANG))>0);
	if(rval<0 && errno!=ECHILD)
	{
        fprintf(stderr,"ERROR::waitpid(%d)\n",play_info.player_id);
        return;
    }

    if(WIFEXITED(stat)){
    
//        fprintf(stderr,"WIFEXITED: child process exited:(%d)\n",WEXITSTATUS(stat));
 
        if(play_info.playstatus==PLAY_STATUS_PLAY){
    
            PL_MUSIC_S*nm=play_get_next_music();
            play_music(nm);
        }
   
    }
    if(WIFSTOPPED(stat)){
    
//        fprintf(stderr,"WIFSTOPPED: child process is stopped:(%d)\n",WSTOPSIG(stat));
    }

    if(WIFSIGNALED(stat)){
    
//        fprintf(stderr,"WIFSIGNALED: child process killed:(%d)\n",WTERMSIG(stat));
    }

    if(WIFCONTINUED(stat)){
//        fprintf(stderr,"WIFCONTINUED: child process continued:(-)\n");
        return;
    }

}

void play_init_signal()
{
    signal(SIGCHLD,player_over);

}



void play_start()
{

    if(play_info.playstatus==PLAY_STATUS_PAUSE){
    
        play_info.playstatus=PLAY_STATUS_PLAY;
        //fprintf(stderr,"RESUME.....\n");
        printf("Resume Playing:: \033[0;32m\033[1m%s\033[0m::\n", play_get_current_music_name());
        kill(play_info.player_id,SIGUSR2);
//--        kill(play_info.player_id,SIGCONT);
    
    }else if(play_info.playstatus==PLAY_STATUS_STOP){
    
        play_info.playstatus=PLAY_STATUS_PLAY;
        
//        fprintf(stderr,"START.....\n");
        printf("Start Playing:: \033[0;32m\033[1m%s\033[0m::\n", play_get_current_music_name());
//        PL_MUSIC_S*nm=play_get_next_music();
        play_music(play_info.curmusic);
    
    }    

}


/* 暂停歌曲 */
void  play_pause()
{

    if(play_info.playstatus==PLAY_STATUS_PLAY){ 
        play_info.playstatus=PLAY_STATUS_PAUSE;
        kill(play_info.player_id,SIGUSR1);
//--        kill(play_info.player_id,SIGSTOP);
//        fprintf(stderr,"PAUSE.....\n");
        printf("Pause Playing:: \033[0;33m\033[1m%s\033[0m::\n", play_get_current_music_name());
    }

}

/* 停止播放歌曲  */
void play_stop()
{

	if(play_info.playstatus==PLAY_STATUS_STOP)
		return;

//    fprintf(stderr,"PLAYER PID:[%d]\n",play_info.player_id);
    printf("Stop Playing:: \033[0;31m\033[1m%s\033[0m::\n", play_get_current_music_name());
    play_info.playstatus=PLAY_STATUS_STOP;
//    kill(play_info.player_id,SIGUSR1);
    kill(play_info.player_id,SIGKILL);

}

/* 设置播放的模式 */
void play_set_mode(PLAY_MODE_S emode)
{
    play_info.playmode=emode;
}

PL_MUSIC_S* play_get_next_music()
{

//    PL_HEAD_S*pl=play_info.curplaylist;

    PL_MUSIC_S*nextmusic;

    switch(play_info.playmode){
    
        case PLAY_MODE_LOOP:
            if(play_info.curmusic==NULL)
                nextmusic=play_info.curplaylist->head;
            else
                nextmusic=play_info.curmusic->next;
            break;

        case PLAY_MODE_LOOPONE:
            if(play_info.curmusic==NULL)
                nextmusic=play_info.curplaylist->head;
            else
                nextmusic=play_info.curmusic;
            break;

        case PLAY_MODE_RANDOM:
            srand(time(NULL));
            int id=rand()%(play_info.curplaylist->plsize+1)+1;
            nextmusic=play_info.curplaylist->head;

            while(id>0){
                nextmusic=nextmusic->next;
                id--;
            }
            break;

        case PLAY_MODE_DEFAULT:
        
        default:

            if(play_info.curmusic==NULL&& play_info.curplaylist!=NULL)
                nextmusic=play_info.curplaylist->head;

			else if(play_info.curplaylist==NULL)
				nextmusic=NULL;
			else if(play_info.curmusic!=NULL&& play_info.curplaylist!=NULL &&
            play_info.curmusic->next==play_info.curplaylist->head)
                nextmusic=NULL;
            else
                nextmusic=play_info.curmusic->next;


         break;   
    
    }
    return nextmusic;

}




