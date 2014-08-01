#ifndef _H_CONSOLE_
#define _H_CONSOLE_



#define CMD_LEN     1024
#define DES_LEN     1024
#define CMD_NUM     10

typedef enum{

    ARG_0=0,
    ARG_1,
    ARG_2,
    ARG_3=4,
    ARG_N=8

}ARG_T;


typedef struct cmd_info
{
    char cmd_name[CMD_LEN];
    char cmd_desc[DES_LEN];
    void (*pFun)(void *);
    ARG_T argtype;
    int level;
    struct cmd_info_head*sub_cmd;
    struct cmd_info* next;
}CMD_INFO_S;


typedef struct cmd_info_head{

    int num_of_cmds;
    struct cmd_info*head;
    struct cmd_info*tail;
}CMD_HEAD_S;



void console_init();

int console_start();
void console_quit();



CMD_INFO_S*console_get_current_preffix(void);

void console_set_current_preffix(CMD_INFO_S*preffix);












#endif
