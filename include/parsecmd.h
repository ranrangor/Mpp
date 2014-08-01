#ifndef _H_PARSE_CMD_
#define _H_PARSE_CMD_





char** parse_cmd(const char*totalcmd);

char** parse_cmds_n(const char *total_cmd_line);

void free_argv(char**argv);

void print_argv(char**argv);
























#endif
