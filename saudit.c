#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/kernel.h>  
#include <linux/unistd.h>
#include <asm/cacheflush.h>  
#include <asm/page.h>  
#include <asm/current.h>
#include <linux/kallsyms.h>

#include "saudit.h"

unsigned long *syscall_table = (unsigned long *)0xc1513160; 

void (*pages_rw)(struct page *page, int numpages) =  (void *) 0xc10308d0;
void (*pages_ro)(struct page *page, int numpages) =  (void *) 0xc10308b0;

static saudit_device_major = SAUDIT_DEVICE_MAJOR;

module_param(saudit_device_major, int, S_IRUGO);

struct saudit_device *saudit_devicep; /*设备结构体指针*/

struct cdev cdev; 

/*系统调用替换*/
asmlinkage int (*original_write)(unsigned int, const char __user *, size_t);
asmlinkage int new_write(unsigned int fd, const char __user *buf, size_t count) {

    // hijacked write

    printk(KERN_ALERT "WRITE HIJACKED");

    return (*original_write)(fd, buf, count);
}

/*文件打开函数*/
int saudit_device_open(struct inode *inode, struct file *filp)
{
    struct saudit_device *dev;
    
    /*获取次设备号*/
    int num = MINOR(inode->i_rdev);

    if (num >=  SAUDIT_DEVICE_NR_DEVS) 
            return -ENODEV;
    dev = &saudit_devicep[num];
    
    /*将设备描述结构指针赋值给文件私有数据指针*/
    filp->private_data = dev;
    
    return 0; 
}

/*文件释放函数*/
int saudit_device_release(struct inode *inode, struct file *filp)
{
    return 0;
}

/*读函数*/
static ssize_t saudit_device_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
    unsigned long p = *ppos;
    unsigned int count = size;
    int ret = 0;
    struct saudit_device *dev = filp->private_data; /*获得设备结构体指针*/

    /*判断读位置是否有效*/
    if (p >= SAUDIT_DEVICE_SIZE)
        return 0;
    if (count >  SAUDIT_DEVICE_SIZE - p)
        count =  SAUDIT_DEVICE_SIZE - p;

    /*读数据到用户空间*/
    if (copy_to_user(buf, (void*)(dev->data + p), count))
        ret = - EFAULT;
    else
    {
        *ppos += count;
        ret = count;    
        printk(KERN_INFO "read %d bytes(s) from %d\n", count, p);
    }

    return ret;
}

/*写函数*/
static ssize_t saudit_device_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
    unsigned long p = *ppos;
    unsigned int count = size;
    int ret = 0;
    struct saudit_device *dev = filp->private_data; /*获得设备结构体指针*/
  
    /*分析和获取有效的写长度*/
    if (p >=  SAUDIT_DEVICE_SIZE)
        return 0;
    if (count >  SAUDIT_DEVICE_SIZE - p)
        count =  SAUDIT_DEVICE_SIZE - p;
    
    /*从用户空间写入数据*/
    if (copy_from_user(dev->data + p, buf, count))
        ret = - EFAULT;
    else
    {
        *ppos += count;
        ret = count;
        printk(KERN_INFO "written %d bytes(s) from %d\n", count, p);
    }

    return ret;
}

/* seek文件定位函数 */
static loff_t saudit_device_llseek(struct file *filp, loff_t offset, int whence)
{ 
    loff_t newpos;

    switch(whence) {
      case 0: /* SEEK_SET */
        newpos = offset;
        break;

      case 1: /* SEEK_CUR */
        newpos = filp->f_pos + offset;
        break;

      case 2: /* SEEK_END */
        newpos =  SAUDIT_DEVICE_SIZE -1 + offset;
        break;

      default: /* can't happen */
        return -EINVAL;
    }
    if ((newpos<0) || (newpos> SAUDIT_DEVICE_SIZE))
     return -EINVAL;
     
    filp->f_pos = newpos;
    return newpos;

}

/*文件操作结构体*/
static const struct file_operations saudit_device_fops =
{
  .owner = THIS_MODULE,
  .llseek = saudit_device_llseek,
  .read = saudit_device_read,
  .write = saudit_device_write,
  .open = saudit_device_open,
  .release = saudit_device_release,
};

/*设备驱动模块加载函数*/
static int device_init(void)
{
    /*
    struct page *sys_call_page_temp;

    sys_call_page_temp = virt_to_page(&syscall_table);
    pages_rw(sys_call_page_temp, 1);

    original_write = (void *)syscall_table[__NR_write];
    syscall_table[__NR_write] = new_write; 
    */

    int result;
    int i;

    dev_t devno = MKDEV(saudit_device_major, 0);

    /* 静态申请设备号*/
    if (saudit_device_major)
        result = register_chrdev_region(devno, 2, "sauditdevice");
    else /* 动态分配设备号 */
    {
        result = alloc_chrdev_region(&devno, 0, 2, "sauditdevice");
        saudit_device_major = MAJOR(devno);
    } 
  
    if (result < 0)
        return result;

    /*初始化cdev结构*/
    cdev_init(&cdev, &saudit_device_fops);
    cdev.owner = THIS_MODULE;
    cdev.ops = &saudit_device_fops;
  
    /* 注册字符设备 */
    cdev_add(&cdev, MKDEV(saudit_device_major, 0),  SAUDIT_DEVICE_NR_DEVS);
   
    /* 为设备描述结构分配内存*/
    saudit_devicep = kmalloc( SAUDIT_DEVICE_NR_DEVS * sizeof(struct saudit_device), GFP_KERNEL);
    if (!saudit_devicep) /*申请失败*/
    {
        result = - ENOMEM;
        unregister_chrdev_region(devno, 1);
  	return result;
    }
    memset(saudit_devicep, 0, sizeof(struct saudit_device));
  
    /*为设备分配内存*/
    for (i=0; i <  SAUDIT_DEVICE_NR_DEVS; i++) 
    {
        saudit_devicep[i].size =  SAUDIT_DEVICE_SIZE;
        saudit_devicep[i].data = kmalloc( SAUDIT_DEVICE_SIZE, GFP_KERNEL);
        memset(saudit_devicep[i].data, 0,  SAUDIT_DEVICE_SIZE);
    }
    
    return 0;
}

/*模块卸载函数*/
static void device_exit(void)
{
    /*
    struct page *sys_call_page_temp;
   
    sys_call_page_temp = virt_to_page(syscall_table);
    syscall_table[__NR_write] = original_write;  
    pages_ro(sys_call_page_temp, 1);
    */

    cdev_del(&cdev); /*注销设备*/
    kfree(saudit_devicep); /*释放设备结构体内存*/
    unregister_chrdev_region(MKDEV(saudit_device_major, 0), 2); /*释放设备号*/
}

MODULE_AUTHOR("Keywind");
MODULE_LICENSE("GPL");

module_init(device_init);
module_exit(device_exit);
