#include <media/mymodule_common.h>

#define CLASS_NAME "myclass"

struct mymodule_dev {
    struct cdev my_cdev;
    char cur_module[MODULE_SIZE];
	int cur_state;
};