#include"file.h"
#include"playlist.h"
#include"play.h"
#include"console.h"

int main(int argc,char** argv)
{
    flst_pool_init();
    pl_pool_init();
    play_init();
    console_init();
//    print_cmds(&CMDs);
//    const char*mcmd[]={"list", "show","123","234",NULL};
//    invoke_cmd(&CMDs,mcmd);
    console_start();
    return 0;

}

