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


int set_module_state(int fd, int state){
	int ret;

	printf("set state: %d\n", state);
	
	ret = ioctl(fd, MYMODULE_CMD_SET_STATE, &state);
	if (ret < 0)
		printf("ioctl fail!\n");
	else
		printf("ioctl success!\n");
	
	return ret;
}

int choose_module(int fd, char *module){
	int len;
	
	printf("set module: %s(%zu)\n", module, strlen(module));
	
    len = write(fd, module, strlen(module)+1);
	if (len < 0)
		printf("write fail!\n");
	else
		printf("write success!\n");

	return len;
}


int check_module(char *module){

	if ( (0 != strncmp("sensor", module, 6)) && 
		 (0 != strncmp("eeprom", module, 6)) &&
		 (0 != strncmp("actuator", module, 8)) &&
		 (0 != strncmp("flash", module, 5)) )
		return 1;
	else
		return 0;
}

int check_state(char *cstate){
	
	if (0 == strncmp("on", cstate, 2))
		return MYMODULE_STATE_ON;	
	else if (0 == strncmp("off", cstate, 3))
		return MYMODULE_STATE_OFF;
	else
		return MYMODULE_STATE_NONE;
}

int get_dev_info(int fd, struct my_data data){
	int len;
	
	len = read(fd, &data, sizeof(data));
	if (len < 0)
		printf("read fail\n");
	else
		printf("read success: current module: [%s], current state: [%d]\n",
				data.module, data.state);

	return len;
}

int main(int argc, char *argv[])
{

	int fd, parm_num;
	struct my_data udata;
	
	parm_num = argc;
	strlcpy(udata.module, "none", sizeof(udata.module));
	udata.state = -1;
	
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

		if(get_dev_info(fd, udata) < 0)
			return -1;
		
	} else if (0 == strncmp("write", argv[1], 5)) {

		if (check_module(argv[2])){
			printf("Unknow Module!\n");
			return -1;
		}
	
		strlcpy(udata.module, argv[2], sizeof(udata.module));
		if(choose_module(fd, udata.module) < 0)
			return -1;
		
	} else if (0 == strncmp("state", argv[1], 5)) {

		udata.state = check_state(argv[2]);
		
		if(udata.state >= MYMODULE_STATE_MAX){
			printf("Unknow state!\n");
			return -1;
		}
		
		if(set_module_state(fd, udata.state) < 0)
			return -1;
		
	} else if (0 == strncmp("reset", argv[1], 5)){
	
		if(!ioctl(fd, MYMODULE_CMD_RESET))
			printf("reset success!\n");
		else
			printf("reset fail!\n");
	}

	close(fd);
    return 0;
}