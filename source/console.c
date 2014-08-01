#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
#include<errno.h>
#include"parsecmd.h"
#include"console.h"
#include"file.h"
#include"play.h"
#include"utils.h"
#include"config.h"

#include"linenoise.h"


CMD_HEAD_S CMDs;

CMD_INFO_S*cur_prefix=NULL;
CMD_INFO_S*last_cmd=NULL;

static volatile int running;

void print_cur_prefix()
{
    if(cur_prefix)
        printf("CMD Preffix[%s]\n",cur_prefix->cmd_name);
    else
        printf("CMD Preffix[None]\n");
}



void print_argtype(ARG_T type)
{

    char p[100];
    switch(type){
        case ARG_0:
            strcpy(p,"Without arguments");
            break;

        case ARG_1:
            strcpy(p,"Only 1 argument");
            break;
    
        case ARG_2:
            strcpy(p,"Supply 2 arguments exactly");
            break;
    
        case ARG_N:
            strcpy(p,"At least 1 argument,Not limited");
            break;
        default:
            strcpy(p,"UNKNOW");
            break;
    }

    fprintf(stdout,"%s\n",p);
}



void print_indent(int n)
{
    while(n>0){
        fprintf(stdout," ");
        n--;
    }

}
/*
char* split_prefix( char*args,char splitchr)
{
    int len=strlen(args);
    char* pos=strchr(args,splitchr);
    if(!pos)
        return NULL;

    *pos='\0';
    if(pos==args+len-1)
        return NULL;
    else
        return pos+1;

}
*/


void print_cmds(CMD_HEAD_S*heads)
{
    static int i=1;

    if(heads->num_of_cmds==0)
        return;
//    print_indent(i);
//    printf("NumofCmds(%d)\n",heads->num_of_cmds);
    CMD_INFO_S*cur_cmd=heads->head;

    while(cur_cmd){
//        print_indent(i);
//        fprintf(stdout,"Name::%s::\n",cur_cmd->cmd_name);

        if(i!=1)
            print_indent(5);
        fprintf(stdout," %-30s::",cur_cmd->cmd_desc);
        if(i!=1)
            print_argtype(cur_cmd->argtype);
        else
            fprintf(stdout,"\n");
//        print_indent(i);
//        fprintf(stdout,"Arg Number ::%d::\n",cur_cmd->argtype);
//        print_indent(i);
//        fprintf(stdout,"CMD level   ::%d::\n\n",cur_cmd->level);
//        print_indent(i);
//        printf(">name:|%s|+[%s]\n",cur_cmd->cmd_name,cur_cmd->cmd_desc);
        i++;
        print_cmds(cur_cmd->sub_cmd);
        i--;
        if(1==i)
            fprintf(stdout,"\n");
        cur_cmd=cur_cmd->next;
    }

}



CMD_INFO_S*console_get_current_prefix(void)
{
    return cur_prefix;

}


void console_set_current_prefix(CMD_INFO_S*prefix)
{
    cur_prefix=prefix;

}

CMD_INFO_S*console_get_last_cmd(void)
{
    return last_cmd;

}


void console_set_last_cmd(CMD_INFO_S*cmd)
{
    last_cmd=cmd;

}




CMD_INFO_S* cmd_new(int level,ARG_T arg_type,const char* cmd_name,const char*cmd_desc,void(*callback)(void*))
{
    CMD_INFO_S*pnew=(CMD_INFO_S*)malloc(sizeof(CMD_INFO_S));

    strcpy(pnew->cmd_name,cmd_name);
    strcpy(pnew->cmd_desc,cmd_desc);
    pnew->argtype=arg_type;
    pnew->level=level;
    pnew->pFun=callback;
    
    pnew->next=NULL;
    pnew->sub_cmd=(CMD_HEAD_S*)malloc(sizeof(CMD_HEAD_S));
    pnew->sub_cmd->num_of_cmds=0;
    pnew->sub_cmd->head=NULL;
    pnew->sub_cmd->tail=NULL;

    return pnew;
}


CMD_INFO_S* find_cmd_by_name(CMD_HEAD_S*heads,const char*cmd_name)
{

    CMD_INFO_S*cur_cmd=heads->head;
    if(cmd_name==NULL)
        return NULL;
    while(cur_cmd){
        if(!strcmp(cur_cmd->cmd_name,cmd_name))
            return cur_cmd;
        cur_cmd=cur_cmd->next;
    }

    return NULL;

}

void insert_cmd_at(CMD_HEAD_S*heads,CMD_INFO_S*newcmd)
{
    newcmd->next=NULL;
    if(heads->head==NULL){
    
        heads->head=newcmd;
        heads->tail=newcmd;
        heads->num_of_cmds=1;
    }else{
        heads->tail->next=newcmd;
        heads->tail=newcmd;
        heads->num_of_cmds++;
    
    }

}

typedef void (*cmd_callback)(void*);

static int get_valid_argv(char** argvs)
{

    int start_id=0;
    while(*argvs ){
        if(!strcmp(*argvs,"--")){
            start_id++;
        }else{
            break;
        }
        argvs++;
    }

    return start_id;

}


static int get_remainder_argv_n(char**argvs)
{

    int i=0;
    while(*argvs){
        i++;
        argvs++;
    }
    return i;

}
/*
void invoke_cmd(CMD_HEAD_S*heads,char** argvs)
{
//    printf("invoke command.{%s}.\n",argvs[0]);
    static cmd_callback pcallback;

    CMD_INFO_S*cur_cmdinfo;
    cur_cmdinfo=find_cmd_by_name(heads,*argvs);
    if(cur_cmdinfo==NULL){

        if(pcallback){

            int sid=get_valid_argv(argvs);

            pcallback(argvs+sid);
            pcallback=NULL;

        }else{
            fprintf(stderr,"WARNING:: Unknow commands..\n");
        }
        return;
    }else{
        if(pcallback==NULL){//first invoke set ccmd prefix
            console_set_current_prefix(cur_cmdinfo);
        }
        pcallback=cur_cmdinfo->pFun;
        argvs++;
        invoke_cmd(cur_cmdinfo->sub_cmd,argvs);
    }

}

*/


void invoke_cmd(CMD_HEAD_S*heads,char** argvs)
{
//    printf("invoke command.{%s}.\n",argvs[0]);
    static cmd_callback pcallback;


    CMD_INFO_S*cur_cmdinfo;
    cur_cmdinfo=find_cmd_by_name(heads,*argvs);
    if(cur_cmdinfo==NULL){

        if(pcallback){
//    print_argv(argvs);

            int sid=get_valid_argv(argvs);

            pcallback(argvs+sid);
            pcallback=NULL;
            int rargs=get_remainder_argv_n(argvs+sid);
            CMD_INFO_S*last_cmd=console_get_last_cmd();
            if(last_cmd->argtype==ARG_0){
                if(rargs>=1)
                    invoke_cmd(console_get_current_prefix()->sub_cmd,argvs+sid+0);
            
            }else if(last_cmd->argtype==ARG_1){
                if(rargs>=2)
                    invoke_cmd(console_get_current_prefix()->sub_cmd,argvs+sid+1);
            
            }else if(last_cmd->argtype==ARG_2){
                if(rargs>=3)
                    invoke_cmd(console_get_current_prefix()->sub_cmd,argvs+sid+2);
            
            }else{
            
                //Do nothing;
            }

        }else{
            fprintf(stderr,"WARNING:: Unknow commands [%s]..\n",*argvs);
        }
        return;
    }else{
        if(pcallback==NULL){//first invoke set ccmd prefix
            console_set_current_prefix(cur_cmdinfo);
        }

        pcallback=cur_cmdinfo->pFun;
        console_set_last_cmd(cur_cmdinfo);

        argvs++;
        invoke_cmd(cur_cmdinfo->sub_cmd,argvs);
    }

}



void register_cmd(CMD_HEAD_S*heads,const char**argvs,int cmdlevel,char*cmd_desc,ARG_T argtype,void(*callback)(void*))
{
    const char*cur_cmdname=argvs[0];

    CMD_INFO_S*cur_cmdinfo;

    cur_cmdinfo=find_cmd_by_name(heads,cur_cmdname);

    if(cur_cmdinfo==NULL){
        CMD_INFO_S*newcmd;
        if(cmdlevel==1)
            newcmd=cmd_new(0,argtype,cur_cmdname,cmd_desc,callback);
        else
            newcmd=cmd_new(0,argtype,cur_cmdname,NULL,NULL);

        insert_cmd_at(heads,newcmd);
        cur_cmdinfo=newcmd;
    }
    cmdlevel--;
    if(cmdlevel>0)
        register_cmd(cur_cmdinfo->sub_cmd,&argvs[1],cmdlevel,cmd_desc,argtype,callback);

}

void print_cmds0(CMD_HEAD_S*cmds_head)
{

    CMD_INFO_S*info=cmds_head->head;
    
    while(info){
        fprintf(stderr,"CMD Name    ::%s::\n",info->cmd_name);
        fprintf(stderr,"CMD DESC    ::%s::\n",info->cmd_desc);
        fprintf(stderr,"CMD ArgType ::%d::\n",info->argtype);
        fprintf(stderr,"CMD level   ::%d::\n",info->level);
        if(info->sub_cmd){
        
            print_cmds(info->sub_cmd);
        }

        info=info->next;
    }


}



static void cmds_init()
{
    CMDs.head=NULL;
    CMDs.tail=NULL;
    CMDs.num_of_cmds=0;

    cur_prefix=NULL;
    last_cmd=NULL;
}


static void sig_int_hdr(int signo)
{

    console_quit();

}

static void sig_tstp_hdr(int signo)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigprocmask(SIG_UNBLOCK,&mask,NULL);

    //default behavior
    signal(SIGTSTP,SIG_DFL);

    kill(getpid(),SIGTSTP);

    //reestablish
    signal(SIGTSTP,sig_tstp_hdr);

//    kill(getpid(),SIGCONT);

}



static void signal_init()
{

    signal(SIGINT,sig_int_hdr);

    if(signal(SIGTSTP,SIG_IGN)==SIG_DFL)
        signal(SIGTSTP,sig_tstp_hdr);


}



/*** CallBack functions for CMDS; */
void list_cb(void*args)
{
//    print_cur_prefix();
    char**argvs=(char**)args;
    printf("list [show|new|from|delete|add|remove] :\n");
    while(*argvs){
//        printf(" <%s>",*argvs);
		argvs++;
    }
}


void file_cb(void*args)
{
//    print_cur_prefix();
    char**argvs=(char**)args;
    printf("file [scan|update|show] [<path>] :\n");
    while(*argvs){
//        printf(" <%s>",*argvs);
		argvs++;
    }
}



void play_cb(void*args)
{
//    print_cur_prefix();
    char**argvs=(char**)args;
    printf("play [list|start|pause|resume|stop|mode] [<list..>|<id..>] :\n");
    while(*argvs){
        printf(" <%s>",*argvs);
		argvs++;
    }
}



void help_cb(void*args)
{
    fprintf(stdout," HELP INFOMATION :\n\n");
    print_cmds(&CMDs);
    fprintf(stdout," <filelist> can be substituted by <idx> or <name>, which showed by `file show all'\n");
    fprintf(stdout," <playlist> can be substituted by <idx> or <name>, which showed by `list show all'\n");
    fprintf(stdout," <music> can be substituted by <idx> or <name>, which showed by `file show <filelist>'\n");
    fprintf(stdout," <entry> can be substituted by <idx> or <name>, which showed by `list show <playlist>'\n\n");

}




void quit_cb(void*args)
{
    fprintf(stderr,"QUIT..\n");
    console_quit();

}

void bg_cb(void*args)
{
    fprintf(stderr,"BG..\n");

    kill(getpid(),SIGTSTP);

}




void update_flst(MP3_HEAD_S*flst,void*d)
{
    char name[LIST_NAME_LEN+1];
    strcpy(name,flst->list_name);
    flst_pool_remove_list(flst);
    flst_del(flst);

    flst=flst_new_for_path(name,name);
    flst_pool_add_list(flst);
}


void file_update_cb(void*args)
{
//    print_cur_prefix();
    char**argvs=(char**)args;
    printf("file update :\n");

    if(*argvs){
    
        while(*argvs){
             MP3_HEAD_S*fl=flst_pool_get_list_by(*argvs);
            if(fl){
                update_flst(fl,NULL);
            }
//            printf(" <%s>",*argvs);
            argvs++;
        }
    }else{
        flst_pool_travser(update_flst,NULL);
    }
}



void list_file_list(MP3_FILE_S* file,void*d)
{
    int* nb=(int*)d;

    printf("[%d] %-15s ::%15s - [%s]\n",*nb,file->filename,file->artist,file->name);
    (*nb)++;

}

void show_flists(MP3_HEAD_S*fl,void *d)
{
//    int id=0;
//    printf("\n==========\nfile list::[%s]\n-----------\n",fl->list_name);
//    flst_travser(fl,list_file_list,d);
//    printf("-----------\n");
    int*id=(int*)d;

    MP3_HEAD_S*cur_fl=flst_pool_get_current_list();
    if(cur_fl==fl)
        printf("\033[1m*[%d] %s\033[0m\n",(*id)++, fl->list_name);
    else
        printf(" [%d] %s\n",(*id)++, fl->list_name);

}



void file_show_all_cb(void*args)
{

//    print_cur_prefix();
    printf("file show all:\n");
    int id=1;
    flst_pool_travser(show_flists,&id);

}


void file_show_cb(void*args)
{
//    print_cur_prefix();
    char**argvs=(char**)args;
    printf("file show :\n");

    int id=1;
    if(*argvs){
        while(*argvs){
            MP3_HEAD_S*fl=flst_pool_get_list_by(*argvs);
            if(fl){
//                printf("file list::%s======\n",fl->list_name);
                flst_travser(fl,list_file_list,&id);
//                printf("=============\n");
            }
//            printf(" <%s>",*argvs);
            argvs++;
        }
    }else{
    
        flst_pool_travser(show_flists,&id);
    
    }


}


void file_select_cb(void*args)
{
//    print_cur_prefix();
    char**argvs=(char**)args;
    printf("file select :");
    MP3_HEAD_S*fl;

    while(*argvs){
        fl=flst_pool_get_list_by(*argvs);
        if(!fl){
            fprintf(stderr,"WARNING::Filelist{%s} not exists.\n",*argvs);
        }else{
            flst_pool_set_current_list(fl);
        }
        
//        printf(" <%s>",*argvs);

        argvs++;
    }

}



void file_scan_cb(void*args)
{
//    print_cur_prefix();
    char**argvs=(char**)args;
    printf("file scan :\n");
    MP3_HEAD_S*fl;
    while(*argvs){
        fl=flst_pool_get_list_by_name(*argvs);
        if(fl){
            fprintf(stderr,"[%s] Already Added!\nMaybe you want to file update %s ?\n",*argvs,*argvs);
            argvs++;
            continue;
        }

        fl=flst_new_for_path(*argvs,*argvs);
        if(fl)
            flst_pool_add_list(fl);
        else{
            fprintf(stderr,"WARNING:: Can not get file list@ %s\n",*argvs);
        }
//        printf(" <%s>",*argvs);
        argvs++;
    }

    file_show_all_cb(NULL);

}


void file_clean_cb(void*args)
{
//    print_cur_prefix();
    char**argvs=(char**)args;
    printf("file clean :\n");
    MP3_HEAD_S*fl;

    argvs_sort(argvs);

    while(*argvs){
        fl=flst_pool_get_list_by(*argvs);
        if(fl){
            flst_pool_remove_list(fl);
        }else{
            fprintf(stderr,"WARNING:: Can not locate Filelist [%s]\n",*argvs);
        }
        argvs++;
    }

}


static void list_pl(PL_MUSIC_S* m,void*d)
{
    int *id=(int*)d;
    PL_MUSIC_S*curmusic=play_get_current_music();
    if(curmusic==m)
        printf("\033[0;33m*");
    else
        printf(" ");

    printf(" [%d] %-20s >> %s\n",(*id)++,m->filename, m->artist);

    if(curmusic==m)
        printf("\033[0m");


}



static void show_plists(PL_HEAD_S*pl,void*d)
{
    int *id=(int*)d;
    PL_HEAD_S*curpl=pl_pool_get_current_list();
    PL_HEAD_S*ppl=play_get_current_pl();

    if(ppl==pl)
        printf("\033[0;32m");

    if(curpl==pl)
        printf("\033[1m*[%d] %s\n\033[0m",(*id)++,pl->plname);
    else
        printf(" [%d] %s\n",(*id)++,pl->plname);
        
    if(ppl==pl)
        printf("\033[0m");
    
}

void list_show_all_cb(void*args)
{
//    print_cur_prefix();
    printf("list show all:\n");
    int id=1;
    pl_pool_travser(show_plists,&id); 

}

void list_show_cb(void*args)
{
//    print_cur_prefix();
    char**argvs=(char**)args;
    printf("list show :\n");
    PL_HEAD_S*pl;
    int id=1;
    if(*argvs){
    while(*argvs){
//        fprintf(stderr,"]]%s[[\n",*argvs);
        pl=pl_pool_get_list_by(*argvs);
        if(pl){
            printf("\nPlay list::%s\n",*argvs);
            pl_travser(pl,list_pl,&id);
        }
//        printf(" <%s>",*argvs);
        argvs++;
    }
    }else{
        pl_pool_travser(show_plists,&id); 
    
    }
}

void list_new_cb(void*args)
{
//    print_cur_prefix();
    char**argvs=(char**)args;
    printf("list new :\n");
    PL_HEAD_S*pl;

//    while(*argvs){
        pl=pl_new(0,*argvs);
        pl_pool_add_list(pl);
//        printf(" <%s>",*argvs);
//        argvs++;
//    }
}



void list_add_cb(void*args)
{
//    print_cur_prefix();
    char**argvs=(char**)args;
    printf("list add :\n");
    MP3_HEAD_S*fl;
    MP3_FILE_S*f;


    while(*argvs){
//        strchr(*argvs,"@")
        f=flst_get_file_by(flst_pool_get_current_list(),*argvs);
        if(f){
            pl_add_file(pl_pool_get_current_list(),f);
        }else{
            char*fname=split_prefix(*argvs,">");
            fl=flst_pool_get_list_by(*argvs);
            if(fl){
                flst_pool_set_current_list(fl);
                f=flst_get_file_by(fl,fname);
                if(!f){
                    fprintf(stderr,"WARNING:: File entry [%s] not exist within {%s}\n",fname,*argvs);
                }else{
                    pl_add_file(pl_pool_get_current_list(),f);
                }
            }else{
                fprintf(stderr,"WARNING:: Filelist {%s} not exists..\n",*argvs);
            }   
        }
//        printf(" <%s>",*argvs);
        argvs++;
    }
}



static void add_to_pl(MP3_FILE_S*file,void*d)
{
    PL_HEAD_S*pl=(PL_HEAD_S*)d;
    pl_add_file(pl,file);
}


void list_from_cb(void*args)
{
//    print_cur_prefix();
    char**argvs=(char**)args;
    printf("list from :\n");

    MP3_HEAD_S*fl;
    PL_HEAD_S*curpl=pl_pool_get_current_list();

    while(*argvs){
        fl=flst_pool_get_list_by(*argvs);
        if(fl){
            flst_travser(fl,add_to_pl,curpl);
        }else{
            fprintf(stderr,"WARNING::Filelist{%s} not exists.\n",*argvs);
        }

//        printf(" <%s>",*argvs);
        argvs++;
    }
}

static void add_to_pl_l(PL_MUSIC_S*music,void*d)
{
//    fprintf(stderr,"add_to_pl_l::%p\n",music);
    PL_HEAD_S*pl=(PL_HEAD_S*)d;
    PL_MUSIC_S*new_music=pl_music_copy(music);
    pl_add_music(pl,new_music);
}



void list_copy_cb(void*args)
{
//    print_cur_prefix();
    char**argvs=(char**)args;
    printf("list from :\n");

    PL_HEAD_S*pl;
    PL_HEAD_S*curpl=pl_pool_get_current_list();

    while(*argvs){
        pl=pl_pool_get_list_by(*argvs);
        if(pl){
            pl_travser(pl,add_to_pl_l,curpl);
        }else{
            fprintf(stderr,"WARNING::Playliat{%s} not exists.\n",*argvs);
        }

//        printf(" <%s>",*argvs);
        argvs++;
    }
}



void list_remove_all_cb(void*args)
{
//    printf("list remove all music:\n");

    PL_HEAD_S*curpl=pl_pool_get_current_list();
    PL_MUSIC_S*cur;

//    int siz=pl->plsize;
    int siz=pl_get_size(curpl);
    int i=0;
    for(i=0;i<siz;i++){
//        printf("deallocated.%d.\n",i);
        cur=pl_remove_music_by_idx(curpl,1);
        pl_music_del(cur);
    }



}



void list_remove_cb(void*args)
{
//    print_cur_prefix();
    char**argvs=(char**)args;
    printf("list remove :\n");

    PL_MUSIC_S*m;
    PL_HEAD_S*curpl=pl_pool_get_current_list();
//    print_argv(--argvs);
//    print_argv(++argvs);
    argvs_sort(argvs);
//    fprintf(stderr,"sorted..\n");
//    print_argv(argvs);
    while(*argvs){      
        m=pl_remove_music_by(curpl,*argvs);
        if(!m){
            fprintf(stderr,"WARNING:: Music[%s] not in current playlist[%s]\n",*argvs,curpl->plname);
        }else{
            pl_music_del(m);
        }
//        printf(" <%s>",*argvs);
        argvs++;
    }

}




void list_rename_cb(void*args)
{
//    print_cur_prefix();
    char**argvs=(char**)args;
    printf("list rename :\n");

//    PL_MUSIC_S*m;
//    PL_HEAD_S*curpl=pl_pool_get_current_list();
    PL_HEAD_S*pl=pl_pool_get_list_by(argvs[0]);
    if(!pl){
        fprintf(stderr,"WARNING:: Playlist [%s] not exists!\n",argvs[0]);
        return;
    }
    if(argvs[1]){
        strcpy(pl->plname,argvs[1]);
    }else{
        fprintf(stderr,"WARNING:: Please specify new Playlist's name!\n");
        fprintf(stderr," list rename <oldlist> <newlist>\n");
    }

}





void list_select_cb(void*args)
{
//    print_cur_prefix();
    char**argvs=(char**)args;
    printf("list select :\n");
    PL_HEAD_S*pl;

//    while(*argvs){
        pl=pl_pool_get_list_by(*argvs);
        if(!pl){
            fprintf(stderr,"WARNING::Playlist{%s} not exists.\n",*argvs);
        }else{
            pl_pool_set_current_list(pl);
        
        }
        
//        printf(" <%s>",*argvs);
//        argvs++;
//    }
}


static void pl_mark_to_save(PL_HEAD_S*pl,void *d)
{
    pl->to_save=1;

}

void list_save_all_cb(void*args)
{

//    print_cur_prefix();
    printf("list save all:\n");

    pl_pool_travser(pl_mark_to_save,NULL);
    pl_pool_save_to_file(SAVLISTFILE);


}

void list_save_cb(void*args)
{
//    print_cur_prefix();
    char**argvs=(char**)args;
    printf("list save :\n");
    PL_HEAD_S*pl;

//    if(*argvs){
    while(*argvs){
        pl=pl_pool_get_list_by(*argvs);
        if(!pl){
            fprintf(stderr,"WARNING::Playlist{%s} not exists.\n",*argvs);
        }else{
//            fprintf(stderr,"WARNING:: UnImplement Use `list save' instead..\n");
            pl_mark_to_save(pl,NULL);
        }
        
//        printf(" <%s>",*argvs);
        argvs++;
    }
 /*   }else{
        pl_pool_save_to_file(SAVLISTFILE);
    
    }
*/
    pl_pool_save_to_file(SAVLISTFILE);
}




void delete_pl(PL_HEAD_S*pl,void *d)
{
    if(strcmp(pl->plname,DEFAULT_LIST))
        pl_pool_remove_list(pl);

}


void list_del_all_cb(void*args)
{

//    print_cur_prefix();
//    char**argvs=(char**)args;
    printf("list delete all:\n");

    pl_pool_travser(delete_pl,NULL);

}

void list_del_cb(void*args)
{
//    print_cur_prefix();
    char**argvs=(char**)args;
    printf("list delete :\n");
    PL_HEAD_S*todel;

    argvs_sort(argvs);

    while(*argvs){

        todel=pl_pool_remove_list_by(*argvs);
        if(!todel){
            fprintf(stderr,"WARNING::Cannot delete Playlist [%s]\n",*argvs);

        }else{
            pl_del(todel);
        }

//        printf(" <%s>",*argvs);

        argvs++;
    }
}

void play_list_cb(void*args)
{
//    print_cur_prefix();
    char**argvs=(char**)args;
    printf("play list :\n");
	play_stop();
    PL_HEAD_S*pl=pl_pool_get_list_by(*argvs);

    if(pl)
        play_list(pl);
    else{
        pl=play_get_current_pl();
        if(pl)
            play_list(pl);
        else
            fprintf(stderr,"WARNING: No list selected\n");
    }

//    while(*argvs){
//        printf(" <%s>",*argvs++);
//    }
}



void play_only_cb(void*args)
{

//    print_cur_prefix();
    char**argvs=(char**)args;
    printf("play only :\n");


    PL_HEAD_S*curpl=pl_pool_get_current_list();
    if(!curpl){
        fprintf(stderr,"WARNING:: No list selected\n");
        fprintf(stderr,"--> list select <playlist>\n");
        return;
    }
	fprintf(stdout,"CURRENT Playlist:%s:",curpl->plname);
    PL_HEAD_S*pl=pl_pool_get_default_list();
//    PL_HEAD_S*spl;
    PL_MUSIC_S*m,*nm;
    pl_pool_reset_default();

    while(*argvs){
        m=pl_get_music_by(curpl,*argvs);
        if(m){
			nm=pl_music_copy(m);
			pl_add_music(pl,nm);
        }else{
            fprintf(stderr,"WARNING:: Can not locate %s!\n",*argvs);
        
        }
        argvs++;
    }
//    play_set_current_pl(pl);
    play_list(pl);

}


/*
void play_only_cb(void*args)
{

//    print_cur_prefix();
    char**argvs=(char**)args;
    printf("play only :\n");

    pl_pool_reset_default();

    PL_HEAD_S*curpl=pl_pool_get_current_list();
    if(!curpl){
        fprintf(stderr,"WARNING:: No list selected\n");
        fprintf(stderr,"--> list select <playlist>\n");
        return;
    }
    PL_HEAD_S*pl=pl_pool_get_default_list();
    PL_HEAD_S*spl;
    PL_MUSIC_S*m,*nm;

    while(*argvs){
		fprintf(stderr,"--->ONLY ::%s::\n",*argvs);
        m=pl_get_music_by(curpl,*argvs);
		fprintf(stderr,"--->ONLY MMMMM%p::\n",m);
        if(m){
			fprintf(stderr,"get music<%s> ok..\n",m->musicname);
        nm=pl_music_copy(m);
        pl_add_music(pl,nm);
		fprintf(stderr,"ONLY FULL OK\n");;
        }else{
		fprintf(stderr,"ONLY SPLIT--->\n");;
        
            char*mname=split_prefix(*argvs,">");
            spl=pl_pool_get_list_by(*argvs);
//		fprintf(stderr,"ONLY SPLIT:pl:%s::name%s\n",spl->plname,mname);;
            if(spl){
                pl_pool_set_current_list(spl);
                m=pl_get_music_by(spl,mname);
				fprintf(stderr,"ONLY GOT PL:%s\n",spl->plname);
                if(!m){
                    fprintf(stderr,"WARNING:: Can not find (%s) at [%s]\n",mname,*argvs);
                }else{
					fprintf(stderr,"got mussic %s\n",m->musicname);
                    nm=pl_music_copy(m);
                    pl_add_music(pl,nm);
                
                }
            }else{
                fprintf(stderr,"WARNING:: Playlist[%s] not exists!\n",*argvs);
            }
        
        
        }
        argvs++;
    }
//    play_set_current_pl(pl);
    play_list(pl);

}


*/


void play_next_cb(void*args)
{
//    print_cur_prefix();
//    char**argvs=(char**)args;
    printf("play next :\n");
    
//    PL_MUSIC_S*pl=play_get_current_pl()
    play_stop();
    PL_MUSIC_S*music=play_get_next_music();
    play_music(music);


//    while(*argvs){
//        printf(" <%s>",*argvs++);
//    }
}


void play_start_cb(void*args)
{
//    print_cur_prefix();
//    char**argvs=(char**)args;
    printf("play start :\n");

//    if(play_get_status()==PLAY_STATUS_STOP){
//        PL_MUSIC_S*music=play_get_next_music();
//        play_music(music);
//    }else if(play_get_status()==PLAY_STATUS_PAUSE){
        play_start();
//    }

//    while(*argvs){
//        printf(" <%s>",*argvs++);
//    }
}


void play_pause_cb(void*args)
{
//    print_cur_prefix();
//    char**argvs=(char**)args;
    printf("play pause :\n");

    play_pause();

//    while(*argvs){
//        printf(" <%s>",*argvs++);
//    }
}


void play_stop_cb(void*args)
{
//    print_cur_prefix();
//    char**argvs=(char**)args;
    printf("play stop :\n");

    play_stop();

}



void play_from_cb(void*args)
{
//    print_cur_prefix();
    char**argvs=(char**)args;
    printf("play from :\n");

    play_stop();
    PL_HEAD_S*pl=play_get_current_pl();
    
    PL_MUSIC_S*music=pl_get_music_by(pl,*argvs);
    
    play_music(music);

}






void play_mode_cb(void*args)
{
//    print_cur_prefix();
    char**argvs=(char**)args;
    printf("play mode:\n");

    if(!*argvs || (*argvs&&!strcmp("default",*argvs))){
    
        play_set_mode(PLAY_MODE_DEFAULT);
    }else if(!strcmp("random",*argvs)){
        play_set_mode(PLAY_MODE_RANDOM);
    
    }else if(!strcmp("loop",*argvs)){
        play_set_mode(PLAY_MODE_LOOP);
    
    }else if(!strcmp("one",*argvs)){
        play_set_mode(PLAY_MODE_LOOPONE);
    
    }else{
        fprintf(stderr,"WARNING::Unknow Mode[%s]\n",*argvs);
        fprintf(stderr,"4 Modes Available:[default|random|loop|one]\n");
    
    }

//    while(*argvs){
//        printf(" <%s>",*argvs++);
//    }
}




void ln_completion(const char *buf, linenoiseCompletions *lc) {
    if (buf[0] == 'l') {
        linenoiseAddCompletion(lc,"list");
        linenoiseAddCompletion(lc,"list show");
        linenoiseAddCompletion(lc,"list new");
        linenoiseAddCompletion(lc,"list delete");
        linenoiseAddCompletion(lc,"list add");
        linenoiseAddCompletion(lc,"list remove");
        linenoiseAddCompletion(lc,"list from");
        linenoiseAddCompletion(lc,"list copy");
        linenoiseAddCompletion(lc,"list rename");
        linenoiseAddCompletion(lc,"list select");
    }


    if (buf[0] == 'f') {
        linenoiseAddCompletion(lc,"file");
        linenoiseAddCompletion(lc,"file show");
        linenoiseAddCompletion(lc,"file scan");
        linenoiseAddCompletion(lc,"file update");
        linenoiseAddCompletion(lc,"file select");
    }

    if (buf[0] == 'p') {

        linenoiseAddCompletion(lc,"play");
        linenoiseAddCompletion(lc,"play next");
        linenoiseAddCompletion(lc,"play list");
        linenoiseAddCompletion(lc,"play only");
        linenoiseAddCompletion(lc,"play from");
        linenoiseAddCompletion(lc,"play start");
        linenoiseAddCompletion(lc,"play pause");
        linenoiseAddCompletion(lc,"play stop");

    }


}





void console_init()
{

/*
    char*homedir=getenv("HOME");
    fprintf(stderr,"HOME::%s:\n",homedir);
    //enter home dir
    int rv=chdir(homedir);
    if(rv<0){
        fprintf(stderr,"WARNING:: Can not enter your home directory\n[%s]!!\n",strerror(errno));
        
    }
*/




    cmds_init();

    const char*cmd_list[]={"list"};
    const char*cmd_file[]={"file"};
    const char*cmd_play[]={"play"};

    const char*cmd_help[]={"help"};
    const char*cmd_quit[]={"quit"};
//    const char*cmd_bg[]={"bg"};

    const char*cmd_file_scan[]={"file","scan"};
    const char*cmd_file_clean[]={"file","clean"};
    const char*cmd_file_update[]={"file","update"};
    const char*cmd_file_show[]={"file","show"};
    const char*cmd_file_show_all[]={"file","show","all"};
    const char*cmd_file_select[]={"file","select"};


    const char*cmd_list_show[]={"list","show"};
    const char*cmd_list_show_all[]={"list","show","all"};

    const char*cmd_list_new[]={"list","new"};
    const char*cmd_list_add[]={"list","add"};
    const char*cmd_list_from[]={"list","from"};
    const char*cmd_list_copy[]={"list","copy"};

    const char*cmd_list_remove[]={"list","remove"};
    const char*cmd_list_remove_all[]={"list","remove","all"};

    const char*cmd_list_rename[]={"list","rename"};
    const char*cmd_list_del[]={"list","delete"};
    const char*cmd_list_del_all[]={"list","delete","all"};
    const char*cmd_list_select[]={"list","select"};
    const char*cmd_list_save[]={"list","save"};
    const char*cmd_list_save_all[]={"list","save","all"};


    const char*cmd_play_next[]={"play","next"};
    const char*cmd_play_list[]={"play","list"};
//    const char*cmd_play_allist[]={"play","list","all"};
    const char*cmd_play_only[]={"play","only"};
    const char*cmd_play_start[]={"play","start"};
    const char*cmd_play_pause[]={"play","pause"};
    const char*cmd_play_stop[]={"play","stop"};
    const char*cmd_play_mode[]={"play","mode"};
    const char*cmd_play_from[]={"play","from"};

    register_cmd(&CMDs,cmd_list,1,"\033[0;32mlist [new|delete|add|remove|from|copy|select|save|rename]\033[0m",ARG_0,list_cb);
    register_cmd(&CMDs,cmd_file,1,"\033[0;32mfile [scan|update|clean|show|select]\033[0m",ARG_0,file_cb);
    register_cmd(&CMDs,cmd_play,1,"\033[0;32mplay [list|only|from|mode|start|stop|pause|next]\033[0m",ARG_0,play_cb);
    register_cmd(&CMDs,cmd_quit,1,"\033[0;32mquit\033[0m",ARG_0,quit_cb);
    register_cmd(&CMDs,cmd_help,1,"\033[0;32mhelp\033[0m",ARG_0,help_cb);
//    register_cmd(&CMDs,cmd_bg,1,"background",ARG_0,bg_cb);


    register_cmd(&CMDs,cmd_file_scan,2,"file scan <path>",ARG_N,file_scan_cb);
    register_cmd(&CMDs,cmd_file_clean,2,"file clean <filelist>",ARG_N,file_clean_cb);
    register_cmd(&CMDs,cmd_file_update,2,"file update <filelist>",ARG_N,file_update_cb);
    register_cmd(&CMDs,cmd_file_show,2,"file show <filelist>",ARG_N,file_show_cb);
    register_cmd(&CMDs,cmd_file_show_all,3,"file show all",ARG_0,file_show_all_cb);
    register_cmd(&CMDs,cmd_file_select,2,"file select <filelist>",ARG_1,file_select_cb);

    register_cmd(&CMDs,cmd_list_show,2,"list show <playlist>",ARG_N,list_show_cb);
    register_cmd(&CMDs,cmd_list_show_all,3,"list show all",ARG_0,list_show_all_cb);
    register_cmd(&CMDs,cmd_list_new,2,"list new <name>",ARG_1,list_new_cb);
    register_cmd(&CMDs,cmd_list_add,2,"list add <filelist>><entry>",ARG_N,list_add_cb);
    register_cmd(&CMDs,cmd_list_from,2,"list from <filelist>",ARG_N,list_from_cb);
    register_cmd(&CMDs,cmd_list_copy,2,"list copy <playlist>",ARG_N,list_copy_cb);
    register_cmd(&CMDs,cmd_list_remove,2,"list remove <music>",ARG_N,list_remove_cb);
    register_cmd(&CMDs,cmd_list_remove_all,3,"list remove all",ARG_0,list_remove_all_cb);
    register_cmd(&CMDs,cmd_list_rename,2,"list rename  <playlist> <newname>",ARG_2,list_rename_cb);
    register_cmd(&CMDs,cmd_list_del,2,"list delete <playlist>",ARG_N,list_del_cb);
    register_cmd(&CMDs,cmd_list_del_all,3,"list delete all",ARG_0,list_del_all_cb);
    register_cmd(&CMDs,cmd_list_select,2,"list select <playlist>",ARG_1,list_select_cb);
    register_cmd(&CMDs,cmd_list_save,2,"list save <playlist>",ARG_N,list_save_cb);
    register_cmd(&CMDs,cmd_list_save_all,3,"list save all",ARG_0,list_save_all_cb);


    register_cmd(&CMDs,cmd_play_next,2,"play next",ARG_0,play_next_cb);
    register_cmd(&CMDs,cmd_play_list,2,"play list <playlist>",ARG_1,play_list_cb);
//    register_cmd(&CMDs,cmd_play_allist,3,"play list all",ARG_0,play_list_all_cb);
    register_cmd(&CMDs,cmd_play_only,2,"play only <filelist>><entry>",ARG_N,play_only_cb);
    register_cmd(&CMDs,cmd_play_start,2,"play start",ARG_0,play_start_cb);
    register_cmd(&CMDs,cmd_play_pause,2,"play pause",ARG_0,play_pause_cb);
    register_cmd(&CMDs,cmd_play_stop,2,"play stop",ARG_0,play_stop_cb);
    register_cmd(&CMDs,cmd_play_mode,2,"play mode <default|loop|one|random>",ARG_1,play_mode_cb);
    register_cmd(&CMDs,cmd_play_from,2,"play from <music>",ARG_1,play_from_cb);
    


//    print_cmds(&CMDs);
    signal_init();

    linenoiseHistoryLoad(CMDHIST_FILE);

    linenoiseSetCompletionCallback(ln_completion);


}


int console_start()
{


//    char buffer[1024];
    printf("MPP Music Player ==>\n");
    
    char**argvs;
    char**nargvs;

    char*buffer;

    running=1;

    while(running){
/*    
        print_prompt();
        if(NULL==fgets(buffer,1024,stdin))
            continue;
*/
        
        buffer=linenoise(PROMPT);
        if(buffer==NULL)
            continue;

        linenoiseHistoryAdd(buffer);
        linenoiseHistorySave(CMDHIST_FILE);
        nargvs=parse_cmds_n(buffer);
        
//        print_argv(nargvs);
//        int i=0;
        char**iargvs=nargvs;
        while(*iargvs){
            argvs=parse_cmd(*iargvs);
//            print_argv(argvs);
            if(!(*argvs)){
                free_argv(argvs);
                break;
            }else{
            invoke_cmd(&CMDs,(char**)argvs);
            free_argv(argvs);
            }
            iargvs++;
        }
        free_argv(nargvs);
        free(buffer);
    }
    return 0;

}




void console_quit()
{
    

    pl_pool_save_to_file(SAVLISTFILE);
    play_stop();
    play_kill_player();

    running=0;
}

/*
int main(int argc,char** argv)
{
    flst_pool_init();
    pl_pool_init();
    play_init();
    console_init();
//    print_cmds(&CMDs);
//    const char*mcmd[]={"list", "show","123","234",NULL};
//    invoke_cmd(&CMDs,mcmd);
    console_start();
    return 0;

}
*/
