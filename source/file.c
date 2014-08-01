#include"file.h"
#include"utils.h"
#include<dirent.h>
#include<string.h>
#include<stdlib.h>
#include<limits.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdio.h>

MP3_HEAD_POOL_S flst_pool;
//MP3_HEAD_S cur_flst;




void print_mp3file(MP3_FILE_S*f)
{

    fprintf(stderr,"===MP3_FILE %p\n",f);
    if(!f)
        return;

    fprintf(stderr,"->filename::%s::\n",f->filename);
    fprintf(stderr,"->name::%s::\n",f->name);
    fprintf(stderr,"->artist::%s::\n",f->artist);
    fprintf(stderr,"->path::%s::\n",f->path);


}

  struct id3 {
    char tag[3];
    char title[30];
    char artist[30];
    char album[30];
    char year[4];
    /* With ID3 v1.0, the comment is 30 chars long */
    /* With ID3 v1.1, if comment[28] == 0 then comment[29] == tracknum */
    char comment[30];
    unsigned char genre;
  };




const char*path_get_name(const char*fullpath)
{
    int flag_has_sep=0;
    const char* p=NULL;;
    const char*newpath=fullpath;

    while(newpath){
        p=newpath;
        newpath=strstr(newpath+1,"/");
        if(newpath)
            flag_has_sep=1;
    }

    if(!flag_has_sep)
        return p;
    else
        return p+1;

}


int read_mp3_tag_into(const char*filepath,MP3_FILE_S*mf)
//int PrintID3v1Tag(char *filepath,MP3_HEAD_S*mf) 
// code from id3 
{

    struct id3 id3v1tag;
    memset(&id3v1tag,0,sizeof(id3v1tag));
  FILE *fp;
  
  fp = fopen(filepath, "r"); /* read only */

  if (fp == NULL) { /* file didn't open */
    fprintf(stderr, "fopen: %s: ", filepath);
    perror("id3v2");
    return 0;
  }
  if (fseek(fp, -128, SEEK_END) < 0) {
    /* problem rewinding */
  } else { /* we rewound successfully */ 
    if (fread(&id3v1tag, 128, 1, fp) != 1) {
      /* read error */
      fprintf(stderr, "fread: %s: ", filepath);
      perror("");
    }
  }


    
//  fclose(fp);

    /* This simple detection code has a 1 in 16777216
     * chance of misrecognizing or deleting the last 128
     * bytes of your mp3 if it isn't tagged. ID3 ain't
     * world peace, live with it.
     */
 
  if (!strncmp(id3v1tag.tag, "TAG", 3))
  {
/*
    printf("id3v1 tag info for %s :\n",filepath);
    printf("Title  : %-30.30s  Artist: %-30.30s\n",
            id3v1tag.title, id3v1tag.artist);
*/
    strcpy(mf->name,id3v1tag.title);
    strcpy(mf->artist,id3v1tag.artist);

/*    printf("Album  : %-30.30s  Year: %-4.4s, Genre: %s (%d)\n",
            id3v1tag.album, id3v1tag.year, 
            (id3v1tag.genre < GetGenreCount())
            ? GetGenreFromNum(id3v1tag.genre) : 
            "Unknown", id3v1tag.genre);
    if (!id3v1tag.comment[28])
      printf("Comment: %-28.28s    Track: %d\n", 
             id3v1tag.comment, id3v1tag.comment[29]);
    else
      printf("Comment: %-30.30s\n", id3v1tag.comment);

*/
    return 1;
  } 
  else 
  {
        const char *name=path_get_name(filepath);
        char namebuf[FILE_NAME_LEN+2];
        strcpy(namebuf,"(");
        strcat(namebuf,name);
        strcat(namebuf,")");
        strcpy(mf->name,namebuf);
        strcpy(mf->artist,"(Anonymous)");
    return 1;     
  }
  return 0;
}




void flst_pool_init(){
    flst_pool.num_of_list=0;
    flst_pool.head=NULL;
    flst_pool.tail=NULL;

}


void flst_pool_add_list(MP3_HEAD_S*newlst)
{

    newlst->next=NULL;
    if(flst_pool.head==NULL){
    //first add
        flst_pool.head=newlst;
        flst_pool.tail=newlst;
//        flst_pool.num_of_list=1;
    }else{
        flst_pool.tail->next=newlst;
        flst_pool.tail=newlst;
    }
    flst_pool.cur_flst=newlst;
    flst_pool.num_of_list++;

}

MP3_HEAD_S*flst_pool_get_current_list()
{
    return flst_pool.cur_flst;

}


int flst_pool_is_contain(MP3_HEAD_S*fl){

    MP3_HEAD_POOL_S*pool=&flst_pool;
    MP3_HEAD_S*cur=pool->head;

    while(cur){
        if(fl==cur){
            return 1;
        }
        cur=cur->next;
    }
    return 0;

}

void flst_pool_set_current_list(MP3_HEAD_S*fl)
{
    MP3_HEAD_POOL_S*pool=&flst_pool;
    if(flst_pool_is_contain(fl))
        pool->cur_flst=fl;

}

int flst_pool_remove_list(MP3_HEAD_S*pst)
{
    MP3_HEAD_S*cur=flst_pool.head;
    if(!cur)
        return 0;

    if(cur==pst){
        flst_pool.head=cur->next;
        cur->next=NULL;
        flst_pool.num_of_list--;
        return 1;
    }
    while(cur->next){
        if(cur->next==pst){
            if(cur->next->next==NULL){
            //need modify tail;
                flst_pool.tail=cur;
            }
            cur->next=pst->next;
            pst->next=NULL;
            flst_pool.num_of_list--;
            return 2;
        }
        cur=cur->next;
    }
    return 0;
}


int flst_pool_remove_list_by_name(const char*lstname)
{
    if(!lstname)
        return 0;

    MP3_HEAD_S*cur=flst_pool.head;
    if(!cur)
        return 0;

    if(!strcmp(cur->list_name,lstname)){
        flst_pool.head=cur->next;
        cur->next=NULL;
        flst_pool.num_of_list--;
        return 1;
    }
    while(cur->next){
        if(!strcmp(cur->next->list_name,lstname)){
            MP3_HEAD_S*tmp=cur->next;
            if(cur->next->next==NULL){
            //need modify tail;
                flst_pool.tail=cur;
            }
            cur->next=tmp->next;
            tmp->next=NULL;
            flst_pool.num_of_list--;
            return 2;
        }
        cur=cur->next;
    }
    return 0;
}

MP3_HEAD_S*flst_pool_get_list_by_name(const char* flstname)
{
    if(!flstname)
        return NULL;

    MP3_HEAD_S*cur=flst_pool.head;
    if(!cur)
        return NULL;
    
    while(cur){
        if(!strcmp(cur->list_name,flstname)){
            return cur;
        }
        cur=cur->next;
    }
    return NULL;
}



MP3_HEAD_S*flst_pool_get_list_by_idx(int idx)
{

    int id=1;
//    int maxflst=flst_pool_get_size();
//    idx=idx>maxflst?maxflst:idx<1?1:idx;

    MP3_HEAD_S*cur=flst_pool.head;
    if(!cur)
        return NULL;
    
    while(cur){
        if(id==idx){
            return cur;
        }
        cur=cur->next;
        id++;
    }
    return NULL;
}


MP3_HEAD_S*flst_pool_get_list_by(const char*desc)
{
    
    MP3_HEAD_S*fl;
    int val;

    val=_atoi(desc);
    if(val==0){
//        fprintf(stderr,"FLST GET BY NAME:%s:\n",desc);
        fl=flst_pool_get_list_by_name(desc);
    }else{
//        fprintf(stderr,"FLST GET BY IDX:%d:\n",val);
        fl=flst_pool_get_list_by_idx(val);
    
    }

    return fl;

}




void flst_pool_travser(void(*func)(MP3_HEAD_S*,void*),void* d)
{
    MP3_HEAD_S*cur=flst_pool.head;
    
    while(cur){
        func(cur,d);
        cur=cur->next;
    }


}



int flst_pool_get_size()
{
    return flst_pool.num_of_list;
}




MP3_HEAD_S* flst_new(const char*flstname)
{
    MP3_HEAD_S*new_flst=(MP3_HEAD_S*)malloc(sizeof(MP3_HEAD_S));
    strcpy(new_flst->list_name,flstname);
    new_flst->head=NULL;
    new_flst->tail=NULL;
    new_flst->maxmusic=0;
    
    return new_flst;
}



void flst_del(MP3_HEAD_S*flst)
{

    MP3_FILE_S*cur;
    int siz=flst_get_size(flst);
    int i=0;
    for(i=0;i<siz;i++){
        cur=flst_remove_file_by_idx(flst,1);
        file_del(cur);

    }
    free(flst);

}


MP3_HEAD_S* flst_new_for_path(const char*flstname,const char *dirpath)
{

    DIR*dirp=opendir(dirpath);
    if(!dirp){
        fprintf(stderr,"ERROR:: Can not open directory[%s]\n",dirpath);
        return NULL;

    }
    struct dirent*dirents;
    const char*name;
    MP3_FILE_S*pnew;

    MP3_HEAD_S*new_flst=flst_new(flstname);

    while((dirents=readdir(dirp))){
        name=dirents->d_name;
        char*suffix=strrchr(name,'.');
        if(!suffix)
            continue;
        if(!strcmp(suffix,".mp3")){
            pnew=file_init(dirpath,name);
            if(!pnew)
                fprintf(stderr,"WARNING::%s/%s is not a mp3 file!.\n",dirpath,name);
            else
                flst_add_file(new_flst,pnew);
        }
    }

    closedir(dirp);

    return new_flst;

}

MP3_FILE_S*file_init(const char *path,const char *name)
{
    MP3_FILE_S*pnew;
    pnew=(MP3_FILE_S*)malloc(sizeof(MP3_FILE_S));
    memset(pnew,0,sizeof(MP3_FILE_S));

    pnew->next=NULL;

//    int ll=strlen(path)+strlen(name);
//    char*fullpath=(char*)malloc(ll*sizeof(char));
    char fullpath[MUSIC_PATH_LEN+1];

    strcpy(fullpath,path);
    strcat(fullpath,"/");
    strcat(fullpath,name);

    char maxpath[PATH_MAX];

    if(!realpath(fullpath,maxpath)){
        fprintf(stderr,"ERROR::Can not resolve file path[%s]\n",fullpath);
        free(pnew);
        pnew=NULL;
    }else{
    int rt=read_mp3_tag_into(maxpath,pnew);
    
    if(rt){
    strcpy(pnew->path,maxpath);
    strcpy(pnew->filename,name);
//    strcpy(pnew->name,name);
    }else{
        free(pnew);
        pnew=NULL;
    }

    }

    return pnew;
}

MP3_FILE_S*file_init_for_path(const char *pathname)
{
//    fprintf(stderr,"PATH.::%s::\n",pathname);

    MP3_FILE_S*pnew;
    pnew=(MP3_FILE_S*)malloc(sizeof(MP3_FILE_S));
    pnew->next=NULL;

    const char*fullpath=pathname;;

    char*name=strrchr(fullpath,'/');
    if(name)
        name++;


    char maxpath[PATH_MAX];
    if(!realpath(fullpath,maxpath)){
        fprintf(stderr,"ERROR::Can not resolve file path[%s]\n",fullpath);
        free(pnew);
        pnew=NULL;
    }else{

    int rt=read_mp3_tag_into(maxpath,pnew);
    
    if(rt){
    strcpy(pnew->path,maxpath);
    strcpy(pnew->filename,name);
//    strcpy(pnew->name,name);
    }else{
        free(pnew);
        pnew=NULL;
    }

    }

    return pnew;
}






void file_del(MP3_FILE_S*file)
{
    free(file);
}

void flst_add_file(MP3_HEAD_S*flst,MP3_FILE_S *pst)
{
    if(!pst)
        return;

    pst->next=NULL;
    if(flst->head==NULL){
        flst->head=pst;
        flst->tail=pst;

    }else{
        flst->tail->next=pst;
        flst->tail=pst;

    }
    flst->maxmusic++;
    
}


int flst_remove_file(MP3_HEAD_S*flst,MP3_FILE_S *pst)
{
    MP3_FILE_S*cur=flst->head;

    if(cur==pst){
        flst->head=cur->next;
        cur->next=NULL;
        flst->maxmusic--;
        return 1;
    }
    while(cur->next){
        if(cur->next==pst){
            if(cur->next->next==NULL){
            //need modify tail;
                flst->tail=cur;
            }
            cur->next=pst->next;
            pst->next=NULL;
            flst->maxmusic--;
            return 2;
        }
    }
    return 0;

}


MP3_FILE_S*flst_remove_file_by_name(MP3_HEAD_S*flst,const char*filename)
{
    if(!filename||!flst)
        return NULL;


    MP3_FILE_S*cur=flst->head;

    if(!strcmp(cur->name,filename)){
        flst->head=cur->next;
        cur->next=NULL;
        flst->maxmusic--;
        return cur;
    }
    while(cur->next){
        if(!strcmp(cur->next->name,filename)){
            MP3_FILE_S*tmp=cur->next;
            if(cur->next->next==NULL){
            //need modify tail;
                flst->tail=cur;
            }
            cur->next=tmp->next;
            tmp->next=NULL;
            flst->maxmusic--;
            return cur->next;
        }
        cur=cur->next;
    }
    return NULL;

}


MP3_FILE_S*flst_remove_file_by_idx(MP3_HEAD_S*flst,int idx)
{
    int id=1;
//    int maxidx=flst_get_size(flst);
//    idx=idx>maxidx?maxidx:(idx<1?1:idx);
    MP3_FILE_S*cur=flst->head;
    if(!cur)
        return NULL;
    
    if(id==idx&&cur){
        flst->head=cur->next;
        cur->next=NULL;
        flst->maxmusic--;
        return cur;
    }
    id++;
    while(cur->next){
        if(id==idx){
            MP3_FILE_S*tmp=cur->next;
            if(cur->next->next==NULL){
            //need modify tail;
                flst->tail=cur;
            }
            cur->next=tmp->next;
            tmp->next=NULL;
            flst->maxmusic--;
            return cur->next;
        }
        cur=cur->next;
        id++;
    }
    return NULL;

}




void flst_travser(MP3_HEAD_S* flst,void(*func)(MP3_FILE_S*,void*),void*d)
{
//    int num_of_music=0;
    MP3_FILE_S*cur=flst->head;
    while(cur){
        func(cur,d);
        cur=cur->next;
    }
}


int flst_get_size(MP3_HEAD_S* flst)
{
    int num_of_music=0;
    MP3_FILE_S*cur=flst->head;
    while(cur){
        cur=cur->next;
        num_of_music++;
    }
    return num_of_music;
}

MP3_FILE_S *flst_get_file_by_idx(MP3_HEAD_S*flst,int idx)
{
//    int maxidx=flst_get_size(flst);
//    idx=idx>maxidx?maxidx:(idx<1?1:idx);
    MP3_FILE_S*curmusic=flst->head;
    int id=1;
    while(curmusic){
        if(id==idx)
            return curmusic;
     
        curmusic=curmusic->next;
        id++;
    }

    return NULL;
    
}

MP3_FILE_S* flst_get_file_by_name(MP3_HEAD_S*flst,const char* filename)
{

    MP3_FILE_S*curmusic=flst->head;
    while(curmusic){

        if(!strcmp(curmusic->name,filename))
            return curmusic;
        curmusic=curmusic->next;
    }

    return NULL;
}


MP3_FILE_S* flst_get_file_by(MP3_HEAD_S*flst,const char* desc)
{
    MP3_FILE_S*f;
    int val;
    val=_atoi(desc);
    if(val==0){
//        fprintf(stderr,"FILE GET BY NAME:%s:\n",desc);
        f=flst_get_file_by_name(flst,desc);
    }else{
//        fprintf(stderr,"FILE GET BY IDX:%d:\n",val);
        f=flst_get_file_by_idx(flst,val);
    }

    return f;
}


