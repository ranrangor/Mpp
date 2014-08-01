#ifndef _H_PLAYLIST_
#define _H_PLAYLIST_

#include"file.h"


#define PL_NAME_LEN         30

#define PLMUSIC_PATH_LEN      256
#define PLMUSIC_TITLE_LEN     64
#define PLMUSIC_FILE_LEN      32
#define PLMUSIC_ARTIST_LEN    64

/*歌曲节点的信息*/
typedef struct pl_music
{
//    int idx;
    char musicname[PLMUSIC_TITLE_LEN+1];
    char filename[PLMUSIC_FILE_LEN+1];
    char artist[PLMUSIC_ARTIST_LEN+1];
    char musicpath[PLMUSIC_PATH_LEN+1];
    struct pl_music *prev;
    struct pl_music *next;
}PL_MUSIC_S;

/*播放列表头结点信息*/
typedef struct pl_head
{
    int   plsize;
    int   plidx;
    char plname[PL_NAME_LEN+1];
    int to_save;    
    struct pl_music *head;
//    struct pl_music *tail;
    struct pl_head*next;
}PL_HEAD_S;


typedef struct pl_pool{
    int num_of_list;
    struct pl_head*cur_pl;
    char*cur_plname;
    struct pl_head *head;
    struct pl_head *tail;

}PL_POOL_S;



void pl_pool_init();

PL_HEAD_S*pl_new(int plnum,const char*plname);

PL_HEAD_S*pl_pool_reset_default();


void pl_del(PL_HEAD_S*pl);
void pl_del_by_name(const char*plname);

PL_MUSIC_S*pl_music_new(MP3_FILE_S*file);
PL_MUSIC_S*pl_music_copy(PL_MUSIC_S*om);
PL_MUSIC_S*pl_music_new_for_path(const char*path);
void pl_music_del(PL_MUSIC_S*music);

void pl_pool_add_list(PL_HEAD_S*pl);
void pl_pool_insert_list(PL_HEAD_S*pl,int pos);

int pl_pool_remove_list(PL_HEAD_S*pl);
PL_HEAD_S* pl_pool_remove_list_by_name(const char*plname);
PL_HEAD_S* pl_pool_remove_list_by_idx(int idx);
PL_HEAD_S*pl_pool_remove_list_by(const char*desc);


PL_HEAD_S* pl_pool_get_default_list();

void pl_pool_set_current_list(PL_HEAD_S*pl);
PL_HEAD_S* pl_pool_get_current_list();
int pl_pool_get_current_idx();
const char* pl_pool_get_current_name();
int pl_pool_get_size();

PL_HEAD_S*pl_pool_get_list_by_idx(int num);
PL_HEAD_S*pl_pool_get_list_by_name(const char*name);
PL_HEAD_S*pl_pool_get_list_by(const char*str);

void pl_pool_travser(void(*func)(PL_HEAD_S*pl,void*),void*);

int pl_pool_is_contain_list(PL_HEAD_S*pl);

void pl_add_file(PL_HEAD_S*list,MP3_FILE_S*file);
void pl_add_music(PL_HEAD_S*list,PL_MUSIC_S*m);
void pl_add_music_by_name(PL_HEAD_S*pl,MP3_HEAD_S*flst,const char*filename);
PL_HEAD_S*pl_new_from_flst(MP3_HEAD_S*flst,const char*plname);

PL_MUSIC_S* pl_remove_music_by_idx(PL_HEAD_S*list,int idx);
PL_MUSIC_S* pl_remove_music_by_name(PL_HEAD_S*list,const char*filename);
PL_MUSIC_S* pl_remove_music_by(PL_HEAD_S*list,const char*desc);
PL_MUSIC_S* pl_remove_music(PL_HEAD_S*pl,PL_MUSIC_S*music);


PL_MUSIC_S*pl_get_music_by_name(PL_HEAD_S*pl,const char*name);
PL_MUSIC_S*pl_get_music_by_idx(PL_HEAD_S*pl,int idx);
PL_MUSIC_S*pl_get_music_by(PL_HEAD_S*pl,const char*desc);

int pl_get_size(PL_HEAD_S*pl);


/* 将链表同步到文件当中 
成功返回0，失败返回-1 */
//int pl_syncfile(const char *plname, char flag);
int pl_pool_save_to_file(const char*fname);
int pl_pool_load_from_file(const char*fname);

/* 根据播放列表名，遍历 */
void pl_travser_by_name(const char *plname ,void (*pFunc)(PL_MUSIC_S*,void*),void*);
void pl_travser(PL_HEAD_S*pl, void(*pFunc)(PL_MUSIC_S*,void*),void*);

/* 遍历播放列表 */
//int pl_travser();
/*
 * des:得到当前歌曲下一首信息,填充到music artist path 指向的字符数组中
 * param:music(in),artist(in),path(in)
 * return 0成功，-1失败
 * */
extern int pl_getnextInfo(char *music,char *artist,char *path);

//得到当前歌曲上一首歌曲的信息
extern int pl_getlastInfo(char *music,char *artist,char *path);
  






#endif
