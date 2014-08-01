#ifndef _H_PLAY_
#define _H_PLAY_

#include"playlist.h"
#include<unistd.h>


typedef enum{
    PLAY_MODE_DEFAULT = 0x1,
    PLAY_MODE_LOOP,
    PLAY_MODE_LOOPONE,
    PLAY_MODE_RANDOM,

}PLAY_MODE_S;

typedef enum{
    PLAY_STATUS_PLAY = 0x1,
    PLAY_STATUS_PAUSE,
    PLAY_STATUS_STOP,
    PLAY_STATUS_RESERV,
}PLAY_STATUS_S;

/* 播放相关信息 */
typedef struct play_info
{
    PLAY_MODE_S playmode;
    PLAY_STATUS_S playstatus;
    PL_HEAD_S*curplaylist;
    PL_MUSIC_S*curmusic;

    pid_t player_id;

/*
 *
    char *curplaymusic;
    char *curartish;
    char *curmusicpath;
*/
}PLAY_INFO_S;


PLAY_STATUS_S play_get_status();
void play_init();
void play_init_signal();

void play_list(PL_HEAD_S*pl);

PL_HEAD_S*play_get_current_pl();
PL_MUSIC_S*play_get_current_music();

void play_set_current_pl(PL_HEAD_S*pl);
void play_music(PL_MUSIC_S*music);


void play_start();
/* 暂停歌曲 */
void play_pause();

/* 停止播放歌曲  */
void play_stop();

/* 设置播放的模式 */
void play_set_mode(PLAY_MODE_S emode);


void play_kill_player();


PL_MUSIC_S* play_get_next_music();












#endif
