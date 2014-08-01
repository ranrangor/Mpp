
#include"utils.h"
#include<string.h>
#include<stdio.h>
#include<stdlib.h>


int _atoi(const char*desc)
{
    if(!desc)
        return 0;
//    int len=strlen(desc);
    int val=0;
    while(*desc){
//        printf(".%c.",*desc);
        if(*desc<='9' && *desc>='0')
            val=val*10+*desc-'0';
        else{
            val=0;
            break;
        }
        desc++;
    }
    return val;

}



char* split_prefix( char*args,char *spliter)
{

    char*savpt=NULL;
//first part::flst_ident
    strtok_r(args,spliter,&savpt);
//second part::file_ident
    char*fn=strtok_r(NULL,spliter,&savpt);

    return fn;
}



static int cmp(char**s1, char**s2){
    return -strcmp(*s1,*s2);
}


void argvs_sort(char**argvs)
{
    int len=0;
    char** iargv=argvs;
    while(*iargv){
//        fprintf(stderr,"+-");
        len++;
        iargv++;
    }
//    fprintf(stderr,"len=%d::argv:%p:\n",len,argvs) ;   
    qsort(argvs,len,sizeof(char*),(int (*)(const void*,const void*))cmp);

}




