#include"playlist.h"
#include"file.h"
#include"config.h"
#include"utils.h"
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<fcntl.h>

#define MAX_FILE_COL 1024

PL_POOL_S pl_pool;


void pl_pool_init(){
    pl_pool.num_of_list=0;
    pl_pool.cur_pl=NULL;
    pl_pool.cur_plname=NULL;
    pl_pool.head=NULL;
    pl_pool.tail=NULL;

    char rpath[PATH_MAX];

    if(realpath(SAVLISTFILE,rpath)){
        printf("\n++++LOAD Playlists @ %s ++++\n\n",rpath);
        pl_pool_load_from_file(rpath);
    }else{
        printf("\n----LOAD Playlists @ %s ----\n\n",SAVLISTFILE);
        pl_pool_load_from_file(SAVLISTFILE);
    
    }

    pl_pool_reset_default();
    /*
    if(!pl_pool_get_list_by_name(DEFAULT_LIST)){
        PL_HEAD_S*defpl=pl_new(0,DEFAULT_LIST);
        pl_pool_add_list(defpl);
    }
    */

}


PL_HEAD_S*pl_pool_reset_default()
{
    PL_HEAD_S*pl;

    pl=pl_pool_remove_list_by_name(DEFAULT_LIST);
    pl_del(pl);

    pl=pl_new(0,DEFAULT_LIST);
    pl->to_save=1;
    pl_pool_insert_list(pl,1);
    return pl;
}


PL_HEAD_S*pl_new(int plidx,const char*plname)
{
    PL_HEAD_S*pnew=(PL_HEAD_S*)malloc(sizeof(PL_HEAD_S));

//    pnew->plidx=plidx;
    pnew->plsize=0;
    pnew->to_save=0;
    if(plname)
        strncpy(pnew->plname,plname,PL_NAME_LEN);
    else
        strcpy(pnew->plname,"UNNAMED");
    pnew->head=NULL;
//    pnew->tail=NULL;
    pnew->next=NULL;

    return pnew;

}



void pl_del(PL_HEAD_S*pl)
{
    PL_MUSIC_S*cur;
    if(!pl)
        return;

    int siz=pl->plsize;
    int i=0;
    for(i=0;i<siz;i++){
//        printf("deallocated.%d.\n",i);
        cur=pl_remove_music_by_idx(pl,1);
        pl_music_del(cur);
    }

    free(pl);

}


void pl_del_by_name(const char*plname)
{
    PL_HEAD_S*pl=pl_pool_get_list_by_name(plname);
    if(pl)
        pl_del(pl);


}




//void pl_pool_add_list(PL_POOL_S*pool,PL_HEAD_S*pl)
void pl_pool_add_list(PL_HEAD_S*pl)
{
//    static int cnt;
//    fprintf(stderr,"<%d> PL_POOL_add_list %p \n",++cnt,pl);
    if(!pl)
        return;

    PL_POOL_S*pool=&pl_pool;
    

    if(pool->head==NULL){
        pool->head=pl;
        pool->tail=pl;
        pool->num_of_list=1;
    }else{
        pool->tail->next=pl;
        pool->tail=pl;
        pool->num_of_list++;
    }

    pool->cur_plname=pl->plname;
    pool->cur_pl=pl;

}




void pl_pool_insert_list(PL_HEAD_S*pl,int pos)
{

    PL_POOL_S*pool=&pl_pool;
    PL_HEAD_S*cur_pl=pool->head;

    pos=pos<1?1:pos;


    if(pos==1){
        pool->num_of_list++;
        pl->next=pool->head;
        pool->head=pl;
        if(pool->num_of_list==1){
            pool->tail=pl;
        }
        pool->cur_pl=pl;
        return ;
    }

    while(cur_pl&& pos>2){
        cur_pl=cur_pl->next;
        pos--;
    }

    pl->next=cur_pl->next;
    cur_pl->next=pl;

    if(cur_pl==pool->tail)
        pool->tail=pl;
    
    pool->num_of_list++;
    pool->cur_pl=pl;

}



//int pl_pool_remove_list(PL_POOL_S*pool,PL_HEAD_S*pl)
int pl_pool_remove_list(PL_HEAD_S*pl)
{

    PL_POOL_S*pool=&pl_pool;
    PL_HEAD_S*cur_pl=pool->head;

    if(cur_pl==pl){
        pool->num_of_list--;
        pool->head=cur_pl->next;
        pl->next=NULL;
        if(pool->num_of_list==0){
            pool->tail=NULL;
        }
        return 1;
    }

    while(cur_pl->next){
        if(cur_pl->next==pl){
            if(cur_pl->next==pool->tail){
                pool->tail=cur_pl;
            }
            PL_HEAD_S*tmp=cur_pl->next;
            cur_pl->next=tmp->next;
            tmp->next=NULL;
            pool->num_of_list--;
            return 2;
        }
        cur_pl=cur_pl->next;
    }

    return 0;

}

/*
//int pl_pool_remove_list_by_name(PL_POOL_S*pool,const char*plname)
PL_HEAD_S* pl_pool_remove_list_by_name(const char*plname)
{

    PL_POOL_S*pool=&pl_pool;
    PL_HEAD_S*cur_pl=pool->head;

    if(!strcmp(cur_pl->plname,plname)){
        pool->head=cur_pl->next;
        cur_pl->next=NULL;
        pool->num_of_list--;
        return cur_pl;
    }
    while(cur_pl->next){
        if(!strcmp(cur_pl->next->plname,plname)){
            PL_HEAD_S*tmp=cur_pl->next;
            if(cur_pl->next==pool->tail){
            //need modify tail;
                pool->tail=cur_pl;
            }
            cur_pl->next=tmp->next;
            tmp->next=NULL;
            pool->num_of_list--;
            return tmp;
        }
        cur_pl=cur_pl->next;
    }
    return NULL;

}
*/

PL_HEAD_S* pl_pool_remove_list_by_name(const char*plname)
{

    PL_POOL_S*pool=&pl_pool;
    PL_HEAD_S*cur_pl=pool->head;

    

    while(cur_pl){
        if(!strcmp(plname,cur_pl->plname)){
            pl_pool_remove_list(cur_pl);
            return cur_pl;
        }
        cur_pl=cur_pl->next;
    }

    return NULL;

}



PL_HEAD_S* pl_pool_remove_list_by_idx(int idx)
{

    PL_POOL_S*pool=&pl_pool;
    PL_HEAD_S*cur_pl=pool->head;

    int id=1;
    int maxplsize=pl_pool_get_size();
    
    idx=idx>maxplsize?maxplsize:idx<1?1:idx;

    while(cur_pl){
        if(id==idx){
            pl_pool_remove_list(cur_pl);
            return cur_pl;
        }
    
        cur_pl=cur_pl->next;
        id++;
    }

    return NULL;

}


PL_HEAD_S*pl_pool_remove_list_by(const char*desc)
{
    PL_HEAD_S*pl;
    int val;
    val=_atoi(desc);
    if(val==0){
        pl=pl_pool_remove_list_by_name(desc);
    }else{
        pl=pl_pool_remove_list_by_idx(val);
    }

    return pl;
}






//int pl_pool_get_size(PL_POOL_S*pool)
int pl_pool_get_size()
{
    PL_POOL_S*pool=&pl_pool;
    return pool->num_of_list;
}

//int pl_pool_get_current_idx(PL_POOL_S*pool)
PL_HEAD_S* pl_pool_get_current_list()
{
    PL_POOL_S*pool=&pl_pool;
    return pool->cur_pl;
}


PL_HEAD_S* pl_pool_get_default_list()
{

//    PL_POOL_S*pool=&pl_pool;
    PL_HEAD_S*defpl=pl_pool_get_list_by_name(DEFAULT_LIST);
    if(defpl)
        return defpl;
    else
        return NULL;
}


const char* pl_pool_get_current_name()
{
    
    PL_POOL_S*pool=&pl_pool;
    return pool->cur_pl->plname;

}

void pl_pool_set_current_list(PL_HEAD_S*pl)
{
    PL_POOL_S*pool=&pl_pool;
    if(pl_pool_is_contain_list(pl))
        pool->cur_pl=pl;
}



int pl_pool_is_contain_list(PL_HEAD_S*pl)
{
    PL_POOL_S*pool=&pl_pool;
    PL_HEAD_S*cur=pool->head;
    
    while(cur){
        if(pl == cur)
            return 1;
        cur=cur->next;
    }
    return 0;

}



PL_HEAD_S*pl_pool_get_list_by_idx(int idx)
{
    PL_POOL_S*pool=&pl_pool;
    PL_HEAD_S*cur_pl=pool->head;

//    int maxid=pl_pool_get_size();
    int id=1;
//    idx=idx>maxid?maxid:idx<1?1:idx;

    while(cur_pl){
        if(id==idx)
            return cur_pl;
        cur_pl=cur_pl->next;
        id++;
    }

    return NULL;
}


PL_HEAD_S*pl_pool_get_list_by_name(const char*name)
{

    if(name==NULL)
        return NULL;

    PL_POOL_S*pool=&pl_pool;

    PL_HEAD_S*cur_pl=pool->head;
    while(cur_pl){
        if(!strcmp(cur_pl->plname,name))
            return cur_pl;

        cur_pl=cur_pl->next;
    }
    return NULL;
}




PL_HEAD_S*pl_pool_get_list_by(const char*str)
{
    PL_HEAD_S*pl;
    int val;
    val=_atoi(str);
    if(val==0){
//        fprintf(stderr,"BY NAME:%s:\n",str);
        pl=pl_pool_get_list_by_name(str);
    }else{
//        fprintf(stderr,"BY ID:%d:\n",val);
        pl=pl_pool_get_list_by_idx(val);
    }

    return pl;
}



int pl_get_size(PL_HEAD_S*pl)
{
    return pl->plsize;

}


PL_MUSIC_S*pl_get_music_by_name(PL_HEAD_S*pl,const char*name)
{

    if(name==NULL)
        return NULL;

    PL_MUSIC_S*cur_m=pl->head;
	int siz=pl_get_size(pl);

    while(cur_m && siz--){
		fprintf(stderr,"while by name:%p:\n",cur_m);
        if(!strcmp(cur_m->filename,name))
            return cur_m;
    
        cur_m=cur_m->next;
    }
    return NULL;
}


PL_MUSIC_S*pl_get_music_by_idx(PL_HEAD_S*pl,int idx)
{

	if(!pl)
		return NULL;
//    fprintf(stderr,"THE PL is [%p]:\n",pl);
//    fprintf(stderr,"GET MUSIC BY IDX--ARG IDX:%d:\n",idx);
    PL_MUSIC_S*cur_m=pl->head;

    int id=1;
//    int maxid=pl_get_size(pl);
//    idx=idx>maxid?maxid:(idx<1?1:idx);

//    fprintf(stderr,"GET MUSIC BY IDX--REAL IDX:%d:\n",idx);
    while(cur_m){
        
        if(id==idx)
            return cur_m;
        cur_m=cur_m->next;
        id++;
    }
    return NULL;

}



PL_MUSIC_S*pl_get_music_by(PL_HEAD_S*pl,const char*desc)
{

	if(!pl)
		return NULL;

//	fprintf(stderr,"GET MUSIC BY @:%s:",pl->plname);
    PL_MUSIC_S*m;

    int val;
    val=_atoi(desc);
    if(val==0){
//        fprintf(stderr,"PL GET MUSIC BY NAME:%s:",desc);
        m=pl_get_music_by_name(pl,desc);
    
    }else{
//        fprintf(stderr,"PL GET MUSIC BY IDX:[%d]:",val);
        m=pl_get_music_by_idx(pl,val);
    
    }
    return m;

}




void pl_pool_travser(void(*func)(PL_HEAD_S*pl,void*),void*d)
{
    PL_POOL_S*pool=&pl_pool;

    PL_HEAD_S*cur_pl=pool->head;
    while(cur_pl){
        func(cur_pl,d);
        cur_pl=cur_pl->next;
    }

}




PL_MUSIC_S*pl_music_new(MP3_FILE_S*file){

    if(!file)
        return NULL;

//    print_mp3file(file);

    PL_MUSIC_S*pnew= (PL_MUSIC_S*)malloc(sizeof(PL_MUSIC_S));
    strcpy(pnew->musicname,file->name);
    strcpy(pnew->filename,file->name);
    strcpy(pnew->musicpath,file->path);
    strcpy(pnew->artist,file->artist);

    pnew->prev=NULL;
    pnew->next=NULL;

    return pnew;

}


PL_MUSIC_S*pl_music_copy(PL_MUSIC_S*om)
{

    if(!om)
        return NULL;

    PL_MUSIC_S*pnew= (PL_MUSIC_S*)malloc(sizeof(PL_MUSIC_S));

    strcpy(pnew->musicname,om->musicname);
    strcpy(pnew->filename,om->filename);
    strcpy(pnew->musicpath,om->musicpath);
    strcpy(pnew->artist,om->artist);

    pnew->prev=NULL;
    pnew->next=NULL;

    return pnew;
}






PL_MUSIC_S*pl_music_new_for_path(const char*path){

    MP3_FILE_S*f=file_init_for_path(path);
    PL_MUSIC_S*m=pl_music_new(f);
    file_del(f);
    return m;

}






void pl_music_del(PL_MUSIC_S*music)
{
    if(!music)
        return ;
/*
    if(music->musicname)
        free(music->musicname);

    if(music->filename)
        free(music->filename);

    if(music->musicpath)
        free(music->musicpath);

    if(music->artist)
        free(music->artist);
*/
    free(music);
}



void pl_add_music(PL_HEAD_S*pl,PL_MUSIC_S*music)
{
    if(!music|| !pl)
        return;

    if(pl->head==NULL){
        pl->head=music;
        music->next=music;
        music->prev=music;
    
    }else{
        music->prev=pl->head->prev;
        music->next=pl->head;
        pl->head->prev->next=music;
        pl->head->prev=music;    
    }
    pl->plsize++;


}

void pl_add_file(PL_HEAD_S*pl,MP3_FILE_S*file)
{
    PL_MUSIC_S* music;
    music=pl_music_new(file);

    pl_add_music(pl,music);


}


void pl_add_music_by_name(PL_HEAD_S*pl,MP3_HEAD_S*flst,const char*filename)
{

    MP3_FILE_S*file=flst_get_file_by_name(flst,filename);
    pl_add_file(pl,file);

}

void creat_pl(MP3_FILE_S*file,void*pl)
{
    pl_add_file((PL_HEAD_S*)pl,file);

}


PL_HEAD_S*pl_new_from_flst(MP3_HEAD_S*flst,const char*plname)
{


    PL_HEAD_S*pl=pl_new(0,plname);

    flst_travser(flst,creat_pl,pl);
    return pl;

}




PL_MUSIC_S* pl_remove_music_by_idx(PL_HEAD_S*pl,int idx)
{
    int id=1;
    PL_MUSIC_S* cur_music=pl->head;

    while(cur_music&& id<=pl->plsize){
        if(id==idx){

            if(cur_music==pl->head){
                pl->head=cur_music->next;
                if(pl->plsize==1)
                    pl->head=NULL;
            }

            cur_music->prev->next=cur_music->next;
            cur_music->next->prev=cur_music->prev;
            cur_music->next=cur_music->prev=NULL;
            pl->plsize--;
            return cur_music;
        }
    
        cur_music=cur_music->next;
        id++;
    }
    return NULL;

}





PL_MUSIC_S* pl_remove_music_by_name(PL_HEAD_S*pl,const char*name)
{
    int id=0;
    PL_MUSIC_S* cur_music=pl->head;

    while(cur_music&& id<pl->plsize){
        if(!strcmp(name,cur_music->musicname)){

            if(cur_music==pl->head){
                pl->head=cur_music->next;
                if(pl->plsize==1)
                    pl->head=NULL;
            }

            cur_music->prev->next=cur_music->next;
            cur_music->next->prev=cur_music->prev;
            cur_music->next=cur_music->prev=NULL;
            pl->plsize--;
            return cur_music;
        }
    
        cur_music=cur_music->next;
        id++;
    }
    return NULL;

}


PL_MUSIC_S* pl_remove_music_by(PL_HEAD_S*pl,const char*desc)
{
	if(!pl)
		return NULL;

    PL_MUSIC_S*m;
	fprintf(stderr,"REMOVE MUSIC BY @:%s:",pl->plname);
    int val;
    val=_atoi(desc);
    if(val==0){
        m=pl_remove_music_by_name(pl,desc);
    
    }else{
        m=pl_remove_music_by_idx(pl,val);
    
    }
    return m;



}





PL_MUSIC_S* pl_remove_music(PL_HEAD_S*pl,PL_MUSIC_S*music)
{
    int id=0;
    PL_MUSIC_S* cur_music=pl->head;

    while(cur_music&& id<pl->plsize){
        if(music==cur_music){

            if(cur_music==pl->head){
                pl->head=cur_music->next;
                if(pl->plsize==1)
                    pl->head=NULL;
            }

            cur_music->prev->next=cur_music->next;
            cur_music->next->prev=cur_music->prev;
            cur_music->next=cur_music->prev=NULL;
            pl->plsize--;
            return cur_music;
        }
    
        cur_music=cur_music->next;
        id++;
    }
    return NULL;

}





 /* 根据播放列表名，遍历 */
void pl_travser_by_name(const char *plname ,void (*pFunc)(PL_MUSIC_S*,void*),void*d)
{
    PL_HEAD_S*pl=pl_pool_get_list_by_name(plname);

//    printf("pl is %p\n plsize :%d\n",pl,pl->plsize);
    PL_MUSIC_S*cur_music=pl->head;

    int id=0;

    while(cur_music && id<pl->plsize){
    
        pFunc(cur_music,d);
    
        cur_music=cur_music->next;
        id++;
    }

}




void pl_travser(PL_HEAD_S*pl, void(*pFunc)(PL_MUSIC_S*,void*),void*d)
{

    PL_MUSIC_S*cur_music=pl->head;

    int id=0;

    while(cur_music && id<pl->plsize){
    
        pFunc(cur_music,d);
    
        cur_music=cur_music->next;
        id++;
    }

}



void save_music(PL_MUSIC_S*m,void*d)
{
    FILE*fp=(FILE*)d;
//    fprintf(fp,"  ITEM:%s:%s:%s:%s:\n",m->filename,m->musicname,m->artist,m->musicpath);
    fprintf(fp,"  ITEM::%s::\n",m->musicpath);

}


void save_pl(PL_HEAD_S*pl,void*d)
{
    if(!pl->to_save)
        return;
    FILE*fp=(FILE*)d;
    fprintf(fp,"START_PLAYLIST::%s::\n",pl->plname);
    pl_travser(pl,save_music,fp);
    fprintf(fp,"END_PLAYLIST::%s::\n",pl->plname);

}



int pl_pool_save_to_file(const char*fname)
{

//    int rv;
    char rpath[PATH_MAX];
    const char*filename;

    if(realpath(fname,rpath)){
        filename=rpath;
    }else{
        filename=fname;
    }


    int fd=open(filename,O_CREAT|O_TRUNC|O_WRONLY,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
    if(fd<0){
//        fprintf(stderr,"Can not open file *%s\n",filename);
        return 0;
    }
    FILE*fp=fdopen(fd,"w");
    if(!fp){
//        fprintf(stderr,"ERROR::can not open file [%s] to save playlists\n",filename);
        return 0;
    }

    pl_pool_travser(save_pl,fp);
    
    fclose(fp);
    return 1;
}



char*line_get_pl_name(const char* line)
{
    char* pos,*rpos;
    if((pos=strstr(line,"::"))){
        rpos=strstr(pos+2,"::");
        if(rpos)
            *rpos='\0';
        return pos+2;
    
    }

    return NULL;
}


char*line_get_m_filename(const char*line){
    char* pos, *rpos;
    if((pos=strstr(line,"::"))){
        rpos=strstr(pos+2,"::");
        if(rpos)
            *rpos='\0';
        return pos+2;
    
    }
    return NULL;

}


//    fprintf(fp,"  ITEM:%s:%s:%s:%s:\n",m->filename,m->musicname,m->artist,m->musicpath);
/*
void line_read_music_into(char*line,PL_MUSIC_S*m)
{

    char*savpt;
    char*tok;

    char*ss=line;
    tok=strtok_r(ss,"::",&savpt);
    
    tok=strtok_r(NULL,"::",&savpt);
    //filename
    strncpy(m->filename,tok,)



}


*/

static int line_is_not_blank(const char*line)
{
    if(strlen(line)>0){
        return 1;
    }
    return 0;

}


static int line_is_pl_start(const char*line)
{
    if(strstr(line,"START_PLAYLIST")){
        return 1;
    }
    return 0;

}


static int line_is_pl_end(const char*line)
{
    if(strstr(line,"END_PLAYLIST")){
        return 1;
    }
    return 0;

}




static int parse_pl_file(FILE*fp)
{
    char*linebuf=(char*)malloc(sizeof(char)*MAX_FILE_COL);
    memset(linebuf,0,sizeof(char)*MAX_FILE_COL);

    PL_HEAD_S*cur_pl=NULL;

    while(fgets(linebuf,MAX_FILE_COL,fp)!=NULL){
    
        if(line_is_pl_start(linebuf)){
            //create_new_pl_list---------;
            cur_pl=pl_new(0,line_get_pl_name(linebuf));
            //marked as to_save;
            cur_pl->to_save=1;
            

        }else if(line_is_pl_end(linebuf)){
            if(cur_pl)
                pl_pool_add_list(cur_pl);

        }else if(line_is_not_blank(linebuf)){
            char*filename=line_get_m_filename(linebuf);
            if(filename){
            //create_music;
            //add_music_to_cur_pl;
                PL_MUSIC_S*m=pl_music_new_for_path(filename) ;
                if(m)
                    pl_add_music(cur_pl,m);
            }
        
        }else{
            fprintf(stderr,"BLANK LINE in Playlist File..\n");
        
        }
    
    }

    return 1;

}


int pl_pool_load_from_file(const char*fname)
{
    int fd=open(fname,O_RDONLY);
    if(fd<0){
        fprintf(stderr,"ERROR:: Can not open file [%s]\n",fname);
    }
    FILE*fp=fdopen(fd,"r");
    if(!fp){
        fprintf(stderr,"ERROR:: Can not open file [%s] to load playlists\n",fname);
        return 0;
    }

    parse_pl_file(fp);


    fclose(fp);

    return 1;
}


/*
PL_MUSIC_S* pl_get_next_idx(PL_HEAD_S*pl,PLAY_MODE_S mode)
{
    


}

*/
/*
MP3_HEAD_S*newlst;

void func(MP3_FILE_S*file,void*d)
{
    
    PL_HEAD_S*pl=(PL_HEAD_S*)d;
    printf("add file[%s] to MP3_HEAD..\n",file->name);
    pl_add_file(pl,file);   

}

void pfunc(PL_MUSIC_S*music,void*d)
{
    printf("===>\n");
    printf("%20s\t",music->musicname);
    printf("%20s\t",music->filename);
    printf("%20s\n\n",music->musicpath);
//    printf("--->\n");

}
*/
/*
int main(int argc,char**argv)
{

    flst_pool_init();
    newlst=flst_new_for_path(argv[2],argv[1]);
    flst_pool_add(newlst);

//    pl=pl_new(22,"first list");
//
//    printf("pl==%x\n",pl);
//
//    flst_travser(newlst,func,NULL);
    printf("---------\n");


    PL_HEAD_S*pl;
    pl_pool_init();

//    pl=pl_new(22,"flist");
//    flst_travser(newlst,func,pl);
    pl=pl_new_from_flst(newlst,"flist");

    pl_pool_add_list(pl);

    pl_travser_by_name("flist",pfunc,NULL);

//    PL_HEAD_S*npl;
//    npl=pl_pool_remove_list_by_name("flist");
//    printf("new pl is %x\n",npl);
    pl_del_by_name("flist");

    return 0;


}

*/
