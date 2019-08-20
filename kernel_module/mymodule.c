#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>

#include "mymodule.h"

dev_t mymodule_devno;
struct mymodule_dev *mymodule_dev_ptr;

static struct class *my_class;
static struct device *my_device;

int mymodule_open(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "[mymodule] mymodule_open\n");
    return 0;
}

ssize_t mymodule_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{

	struct my_data kdata;
	
    if (size > sizeof(kdata))
        size = sizeof(kdata);
    
    printk("[mymodule] mymodule_read(%zu)\n", size);

	strlcpy(kdata.module, mymodule_dev_ptr->cur_module, sizeof(kdata.module));

	kdata.state = get_mymodule_gpios(mymodule_dev_ptr);

	printk("[mymodule] module: %s, state: %d\n", kdata.module, kdata.state);

    if(copy_to_user(buf, &kdata, sizeof(kdata))){
		printk("[mymodule] copy_to_user fail!\n");
	} else {
		printk("[mymodule] copy_to_user success!\n");
	}
	
    return 0;   // 返回值不为0, cat节点时会死循环
}

ssize_t mymodule_write(struct file *file, const char __user *buf, size_t size, loff_t *ppos)
{

    int ret;
    char module[MODULE_SIZE];
	
    printk(KERN_INFO "[mymodule] mymodule_write(%zu)\n", size);

    if (copy_from_user(module, buf, size)){
        printk("[mymodule] copy_from_user fail!\n");
        ret = -EFAULT;
    } else {
        ret = size;
		module[size-1] = '\0';
		
		strlcpy(mymodule_dev_ptr->cur_module, module, sizeof(mymodule_dev_ptr->cur_module));

		printk("[mymodule] update module success: %s\n", mymodule_dev_ptr->cur_module);
    }
    
    return ret;
}

static long mymodule_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int state;
	
	switch (cmd)
	{
	case MYMODULE_CMD_RESET:
		printk("[mymodule] do MYMODULE_CMD_RESET");
		
		mymodule_dev_ptr->cur_state = MYMODULE_STATE_OFF;
		strlcpy(mymodule_dev_ptr->cur_module, "none", sizeof(mymodule_dev_ptr->cur_module));
		break;
		
	case MYMODULE_CMD_SET_STATE:
		printk("[mymodule] do MYMODULE_CMD_SET_STATE");

		if(copy_from_user(&state, (int*)arg, sizeof(state))){
			printk("[mymodule] copy_from_user fail!\n");
		} else {
			printk("[mymodule] set module state: %d\n", state);

			if (state == MYMODULE_STATE_ON) {

				request_mymodule_gpios(mymodule_dev_ptr);
				set_mymodule_gpios(mymodule_dev_ptr, state);

			} else if (state == MYMODULE_STATE_OFF){

				set_mymodule_gpios(mymodule_dev_ptr, state);
				free_mymodule_gpios(mymodule_dev_ptr);
			}
		}
		break;
	default:
		break;
	}	
	return 0;
}


static const struct file_operations mymodule_fops = {
    .owner = THIS_MODULE,
    .open = mymodule_open,
    .read = mymodule_read,
    .write = mymodule_write,
    .unlocked_ioctl = mymodule_ioctl,
};

static void mymodule_cdev_setup(void)
{
    // 字符设备初始化 && 注册
    int ret;
    
    cdev_init(&mymodule_dev_ptr->my_cdev, &mymodule_fops);
    mymodule_dev_ptr->my_cdev.owner = THIS_MODULE;
    ret = cdev_add(&mymodule_dev_ptr->my_cdev, mymodule_devno, 1);
    if (ret){
        printk(KERN_INFO "[mymodule] add cdev fail!\n");
    }
}

static int mymodule_init(void)
{
    int ret;
    printk(KERN_INFO "[mymodule] E\n");
    
    // 系统动态分配设备号
    ret = alloc_chrdev_region(&mymodule_devno, 0, 1, "mymodule");
    if(ret < 0){
        printk(KERN_INFO "[mymodule] alloc_chrdev_region fail!\n");
        return ret;
    }
    
    // 为驱动结构体分配内存
    mymodule_dev_ptr = kzalloc(sizeof(struct mymodule_dev), GFP_KERNEL);
    if(!mymodule_dev_ptr){
        printk(KERN_INFO "[mymodule] malloc memory for mymodule_dev fail!\n");
        ret = -ENOMEM;
        goto free_devno;
    }

    mymodule_cdev_setup();

    my_class = class_create(THIS_MODULE, CLASS_NAME);
    if(IS_ERR(my_class)){
        printk(KERN_INFO "[mymodule] class_create fail!\n");
        return -1;
    }

    my_device = device_create(my_class, NULL, mymodule_devno, NULL, "mydevice");

	strlcpy(mymodule_dev_ptr->cur_module, "none", sizeof(mymodule_dev_ptr->cur_module));
	mymodule_dev_ptr->cur_state = MYMODULE_STATE_OFF;

	mymodule_gpio_init(mymodule_dev_ptr);
	
    return 0;

free_devno:
    unregister_chrdev_region(mymodule_devno, 1);
    return ret;
}
static void mymodule_exit(void)
{
    printk(KERN_INFO "[mymodule] X\n");

    device_destroy(my_class, mymodule_devno);
    class_destroy(my_class); 
    cdev_del(&mymodule_dev_ptr->my_cdev);
    kfree(mymodule_dev_ptr);
    unregister_chrdev_region(mymodule_devno, 1);
}

module_init(mymodule_init);
module_exit(mymodule_exit);
MODULE_LICENSE("GPL v2");
