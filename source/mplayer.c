#include<stdio.h>
#include<time.h>
#include<unistd.h>
#include<signal.h>





static void pause_hdr(int signo)
{
    fprintf(stderr," ++ PAUSED(%d)\n",getpid());

    kill(getpid(),SIGSTOP);

}


static void resume_hdr(int signo){

    fprintf(stderr," ++ RESUMED(%d)\n",getpid());

    kill(getpid(),SIGCONT);


}




static void signal_init()
{

    signal(SIGUSR1,pause_hdr);
    signal(SIGUSR2,resume_hdr);


}


int main(int argc,char** argv)
{

    if(argc<2)
        return -1;

//    signal_init();
    char info[100];

    int times=10;

    printf(">>>>>>>>>>>>>>PLAY START[%s]<<<<<<<<<<<<<<<<\n",argv[1]);
    while(times>0){
        sprintf(info,"------PID:(%d)-------->>PLAYING:: [%s]\n",getpid(),argv[1]);

        printf("%50s",info);
        times--;
        sleep(1);
    }

    printf(">>>>>>>>>>>>>>PLAY END  [%s]<<<<<<<<<<<<<<<<\n",argv[1]);
    
    return 0;

}
