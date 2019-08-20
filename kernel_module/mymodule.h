#include <linux/cdev.h>
#include <media/mymodule_common.h>

#define CLASS_NAME "myclass"

#define MAX_GPIO_NUM 5
#define GPIO_MAX_LABEL_LEN 20

struct mymodule_gpio {
	int gpio;
	char label[GPIO_MAX_LABEL_LEN];
};

struct mymodule_dev {
    struct cdev my_cdev;
    char cur_module[MODULE_SIZE];
	int num_gpio;
	int cur_state;
	struct mymodule_gpio my_gpio[MAX_GPIO_NUM];
};

int get_mymodule_gpios(struct mymodule_dev *mydev);
int request_mymodule_gpios(struct mymodule_dev *mydev);
void set_mymodule_gpios(struct mymodule_dev *mydev, int value);
void free_mymodule_gpios(struct mymodule_dev *mydev);
void mymodule_gpio_init(struct mymodule_dev *mydev);

