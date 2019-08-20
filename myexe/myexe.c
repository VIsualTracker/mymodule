#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <media/mymodule_common.h>
#include <asm-generic/ioctl.h>

#define MYDEVICE_PATH "/dev/mydevice"

int reset(int fd){

    if(ioctl(fd, MYMODULE_CMD_RESET) < 0) {
        printf("reset fail!\n");
        return -1;
    }
    else {
        printf("reset success!\n");
        return 1;
    }
}

int check_state(char *cstate){

    if (0 == strncmp("on", cstate, 2))
        return MYMODULE_STATE_ON;
    else if (0 == strncmp("off", cstate, 3))
        return MYMODULE_STATE_OFF;
    else
        return -1;
}

int set_module_state(int fd, char *cstate){
    int istate;

    printf("set state: %s\n", cstate);

    istate = check_state(cstate);
    if(istate < 0){
        printf("Unknow state!\n");
        return -1;
    }

    if (ioctl(fd, MYMODULE_CMD_SET_STATE, &istate) < 0){
        printf("set state fail!\n");
        return -1;
    } else {
        printf("set state success!\n");
        return 1;
    }
}

int check_module(char *module){

    if ( (0 != strncmp("sensor", module, 6)) &&
         (0 != strncmp("eeprom", module, 6)) &&
         (0 != strncmp("actuator", module, 8)) &&
         (0 != strncmp("flash", module, 5)) )
        return -1;
    else
        return 1;
}
int set_module(int fd, char *module){
    int len;

    if (check_module(module) < 0){
        printf("Unknow Module!\n");
        return -1;
    }

    printf("set module: %s(%zu)\n", module, strlen(module));

    len = write(fd, module, strlen(module)+1);
    if (len < 0)
        printf("write fail!\n");
    else
        printf("write success!\n");

    return len;
}


int get_dev_info(int fd){

    int len;
    struct my_data udata;

    strlcpy(udata.module, "none", sizeof(udata.module));
    udata.state = -1;

    len = read(fd, &udata, sizeof(udata));
    if (len < 0)
        printf("read fail\n");
    else
        printf("read success: current module: [%s], current state: [%d]\n",
                udata.module, udata.state);

    return len;
}

int main(int argc, char *argv[])
{

    int fd, parm_num;
    parm_num = argc;

    if (parm_num < 2){
        printf("need 2 params!\n");
        return -1;
    }

    fd = open(MYDEVICE_PATH, O_RDWR);
    if (fd < 0){
        printf("open dev fail!\n");
        return -1;
    }

    if (0 == strncmp("read", argv[1], 4)) {

        if(get_dev_info(fd) < 0)
            return -1;

    } else if (0 == strncmp("write", argv[1], 5)) {

        if(set_module(fd, argv[2]) < 0)
            return -1;

    } else if (0 == strncmp("state", argv[1], 5)) {

        if(set_module_state(fd, argv[2]) < 0)
            return -1;

    } else if (0 == strncmp("reset", argv[1], 5)){
        if(reset(fd) < 0)
            return -1;
    }

    close(fd);
    return 0;
}