#ifndef _H_FILE_
#define _H_FILE_


#define MUSIC_NAME_LEN      30
#define LIST_NAME_LEN       30
#define MUSIC_ARTIST_LEN    30
#define MUSIC_PATH_LEN      90
#define FILE_NAME_LEN       50
typedef struct _mp3_file{
    char filename[FILE_NAME_LEN+1];
    char name[MUSIC_NAME_LEN+1];
    char artist[MUSIC_ARTIST_LEN+1];
    char path[MUSIC_PATH_LEN+1];
    struct _mp3_file *next;
}MP3_FILE_S;
//MP3信息头
typedef struct _mp3_head{
    char list_name[LIST_NAME_LEN+1];//pathname
    int maxmusic;
    struct _mp3_file *head;
    struct _mp3_file *tail;
    struct _mp3_head*next;
}MP3_HEAD_S;
//

typedef struct _mps_head_pool{
    int num_of_list;
    struct _mp3_head* head;
    struct _mp3_head* tail;
    MP3_HEAD_S* cur_flst;
}MP3_HEAD_POOL_S;



void print_mp3file(MP3_FILE_S*f);



void flst_pool_init();
void flst_pool_add_list(MP3_HEAD_S*newlst);
int flst_pool_remove_list(MP3_HEAD_S*pst);
int flst_pool_remove_list_by_name(const char*flstname);

MP3_HEAD_S*flst_pool_get_list_by_name(const char* flstname);
MP3_HEAD_S*flst_pool_get_list_by_idx(int idx);
MP3_HEAD_S*flst_pool_get_list_by(const char*desc);

MP3_HEAD_S*flst_pool_get_current_list();
void flst_pool_set_current_list(MP3_HEAD_S*fl);

int flst_pool_get_size();

void flst_pool_travser(void(*func)(MP3_HEAD_S*,void*),void* d);


MP3_HEAD_S* flst_new(const char*flstname);
void flst_del(MP3_HEAD_S*flst);
void flst_add_file(MP3_HEAD_S*lst,MP3_FILE_S *pst);

MP3_FILE_S*file_init(const char *path,const char *name);
MP3_FILE_S*file_init_for_path(const char *pathname);
void file_del(MP3_FILE_S*file);


MP3_HEAD_S*flst_new_for_path(const char*flstname,const char *dirpath);


int flst_remove_file(MP3_HEAD_S*flst,MP3_FILE_S *pst);
MP3_FILE_S* flst_remove_file_by_name(MP3_HEAD_S*flst,const char*filename);
MP3_FILE_S*flst_remove_file_by_idx(MP3_HEAD_S*flst,int idx);


void flst_travser(MP3_HEAD_S* flst,void(*func)(MP3_FILE_S*,void*),void*);

int flst_get_size(MP3_HEAD_S* flst);


MP3_FILE_S *flst_get_file_by_idx(MP3_HEAD_S*flst,int musicidx);
MP3_FILE_S* flst_get_file_by_name(MP3_HEAD_S*flst,const char* filename);
MP3_FILE_S* flst_get_file_by(MP3_HEAD_S*flst,const char* desc);




#endif
