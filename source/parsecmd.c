#include"parsecmd.h"
#include"utils.h"
#include<string.h>
#include<stdio.h>
#include<stdlib.h>


#define MAX_ARGS 10

/*
static char*strlower(char*str)
{
    char*rstr=str;

    while(*str!='\0'){
        if(*str<=90 && *str>=65)
            *str=*str+0x20;
        str++;
    }
//    printf("strlower::|%s|\n",rstr);
    return rstr;
}
*/
void free_argv(char**argv)
{
    if(!argv)
        return;

    int i;
    for(i=0;i<MAX_ARGS;i++){
        if(argv[i])
            free(argv[i]);
    }
    free(argv);

}

void print_argv(char**argv)
{
    if(!argv){
        fprintf(stderr,"--> NONE <--\n");
        return ;
    }
        fprintf(stderr,"--> argv:%p <--\n",argv);
    int i;
    for(i=0;argv[i];i++){
//        if(argv[i])
            fprintf(stderr,"%d:(%d)|%s|\n",i,strlen(argv[i]),argv[i]);
    }
        fprintf(stderr,"--> end argv:%p <--\n",argv);
 
}



static char*strtrim(char*str)
{
    int ind_start;
    int ind_end;;
    ind_start=0;

    while(str[ind_start]=='\n'||str[ind_start]=='\t'||str[ind_start]==' '||str[ind_start]=='"'||str[ind_start]=='\''){
        ind_start++;
    }
    int len=strlen(str);
    ind_end=len-1;

    while(str[ind_end]=='\n'||str[ind_end]=='\t'||str[ind_end]==' '||str[ind_end]=='"'||str[ind_end]=='\''){
        ind_end--;
    }

    int i;
    for(i=0;ind_start<=ind_end;i++,ind_start++){
        str[i]=str[ind_start];
    }
    str[i]='\0';
//    printf("under strtrim:|%s|\n",str);
    return str;
}

char** parse_cmds_n(const char *total_cmd_line)
{
    if(!total_cmd_line)
        return NULL;

    const char*delimit=",&";
    char** argv=(char**)malloc(MAX_ARGS*sizeof(char*));
//    fprintf(stderr,"Parse cmds_n:%p:\n",argv);
    memset(argv,0,sizeof(char*)*MAX_ARGS);

    int len=strlen(total_cmd_line);
    char* cmd=(char*)malloc((1+len)*sizeof(char));
    strcpy(cmd,total_cmd_line);
//    strtrim(cmd);

    char*savpt;
    char*token;
    int n_arg=0;

    for(n_arg=0;n_arg<MAX_ARGS;cmd=NULL){

        token=strtok_r(cmd,delimit,&savpt);

        if(token==NULL){
            argv[n_arg]=NULL;
            break;
        }
//        fprintf(stderr,"token-->(%s)\n",token);
//        fprintf(stderr,"token(%d):[%s]\n",strlen(token),token);
        argv[n_arg]=strdup(token);

        n_arg++;
    }
    
    free(cmd);
    return argv;




}


char** parse_cmd(const char*totalcmd)
{
    if(!totalcmd)
        return NULL;


    const char*delimit=" ";
    char** argv=(char**)malloc(MAX_ARGS*sizeof(char*));
//    fprintf(stderr,"Parse cmd:%p:\n",argv);
    memset(argv,0,sizeof(char*)*MAX_ARGS);

    int len=strlen(totalcmd);
    char* cmd=(char*)malloc((1+len)*sizeof(char));
    strcpy(cmd,totalcmd);
    strtrim(cmd);

    char*savpt;
    char*token;
    int n_arg=0;

    for(n_arg=0;n_arg<MAX_ARGS;cmd=NULL){

        token=strtok_r(cmd,delimit,&savpt);

        if(token==NULL){
            argv[n_arg]=NULL;
            break;
        }
//        fprintf(stderr,"token-->(%s)\n",token);
//        fprintf(stderr,"     token(%d):[%s]\n",strlen(token),token);
        argv[n_arg]=strdup(token);

        n_arg++;
    }
    
    free(cmd);
    return argv;

}

/* For Test

int main(int argc,char**argv)
{

    char buf[1024]="";
    
    strcpy(buf,argv[1]);

    printf(":|%s|..\n",buf);

    char**argvs=parse_cmd(buf);

    print_argv(argvs);

//    printf("After Trim:|%s|..\n",buf);

    free_argv(argvs);
    return 0;
}


*/
