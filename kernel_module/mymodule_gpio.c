#include <linux/gpio.h>

#include "mymodule.h"

int get_mymodule_gpios(struct mymodule_dev *mydev)
{
	int i, ret = 1;
	for (i = 0; i < mydev->num_gpio; i++){
		if (gpio_is_valid(mydev->my_gpio[i].gpio)) {

			if (gpio_get_value(mydev->my_gpio[i].gpio) == 0)
				ret = 0;
		
		} else {
			printk("[mymodule] gpio(%d) is not valid!\n",
					mydev->my_gpio[i].gpio);
			ret = 0;
		}
	}
	return ret;
}

int request_mymodule_gpios(struct mymodule_dev *mydev)
{
	int i, ret = 0;

	for (i = 0; i < mydev->num_gpio; i++){
		
		if (gpio_is_valid(mydev->my_gpio[i].gpio)) {
			
			ret = gpio_request_one(mydev->my_gpio[i].gpio,
								0, mydev->my_gpio[i].label);
			if (ret) {
				printk("[mymodule] request gpio(%d) fail!\n",
					mydev->my_gpio[i].gpio);
			}
			gpio_export(mydev->my_gpio[i].gpio, 0);
		} else {
			printk("[mymodule] gpio(%d) is not valid!\n",
					mydev->my_gpio[i].gpio);
		}
	}
	return ret;
}

void set_mymodule_gpios(struct mymodule_dev *mydev, int value)
{
	int i;

	for (i = 0; i < mydev->num_gpio; i++){
			
		if (gpio_is_valid(mydev->my_gpio[i].gpio)) {

			gpio_direction_output(mydev->my_gpio[i].gpio, value);
		
		} else {
			printk("[mymodule] gpio(%d) is not valid!\n",
					mydev->my_gpio[i].gpio);
		}
	}
}

void free_mymodule_gpios(struct mymodule_dev *mydev)
{
	int i;

	for (i = 0; i < mydev->num_gpio; i++){
		
		if (gpio_is_valid(mydev->my_gpio[i].gpio)) {
			
			gpio_free(mydev->my_gpio[i].gpio);
		}  else {
			printk("[mymodule] gpio(%d) is not valid!\n",
					mydev->my_gpio[i].gpio);
		}
	}
}

void mymodule_gpio_init(struct mymodule_dev *mydev)
{
	mydev->num_gpio = 2;

	mydev->my_gpio[0].gpio = 131;
	mydev->my_gpio[0].label[0] = '\0';
	strlcpy(mydev->my_gpio[0].label, "GPIO1", GPIO_MAX_LABEL_LEN);

	mydev->my_gpio[1].gpio = 39;
	mydev->my_gpio[1].label[0] = '\0';
	strlcpy(mydev->my_gpio[1].label, "GPIO2", GPIO_MAX_LABEL_LEN);
}


