
#define MODULE_SIZE 10

struct my_data {
    char module[MODULE_SIZE];
    int state; // 1:on, 0:off
};


enum mymodule_states {
    MYMODULE_STATE_OFF,
    MYMODULE_STATE_ON,
    MYMODULE_STATE_MAX,
};

enum mymodule_modules {
	MYMODULE_MODULE_NONE,
    MYMODULE_MODULE_SENSOR,
    MYMODULE_MODULE_EEPROM,
    MYMODULE_MODULE_ACTUATOR,
    MYMODULE_MODULE_FLASH,
};


#define MY_CMD_MAGIC 0xba
#define MYMODULE_CMD_RESET _IO(MY_CMD_MAGIC, 0)
#define MYMODULE_CMD_SET_STATE _IOR(MY_CMD_MAGIC, 1, int)
