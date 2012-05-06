#include <linux/sched.h>
#include <linux/init.h>
#include <linux/module.h>

#include "saudit.h"
#include "hijack.h"
#include "driver.h"
#include "proc_file.h"

/*设备驱动模块加载函数*/
static int saudit_init(void)
{
    hijack_init();
    driver_init();
    proc_init();

    printk(KERN_ALERT "Saudit -- Saudit init\n");
    return 0;
}

/*模块卸载函数*/
static void saudit_exit(void)
{
    hijack_exit();
    driver_exit();
    proc_exit();

    printk(KERN_ALERT "Saudit -- Saudit init\n");
}

MODULE_AUTHOR("Keywind");
MODULE_LICENSE("GPL");

module_init(saudit_init);
module_exit(saudit_exit);
