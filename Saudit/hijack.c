#include <linux/sched.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/string.h>

#include "hijack.h"

struct
{
    unsigned short limit;
    unsigned int base;
}__attribute__((packed))idtr;

struct
{
    unsigned short off1;
    unsigned short sel;
    unsigned char none, flags;
    unsigned short off2;
}__attribute__((packed))idt;

unsigned int* syscall_table;

unsigned int get_sys_call_table(void)
{
    unsigned int sys_call_off;
    unsigned int sys_call_table;
    char *p;
    int i;
    //获得中断描述符表寄存器地址

    asm("sidt %0":"=m"(idtr)); 
    printk("add of idtr %x\n", idtr.base);
    //获取0x80中断处理程序的地址

    memcpy((char* )&idt,(char*)(idtr.base+8*0x80),sizeof(idt));
    sys_call_off = ((idt.off2 << 16) | idt.off1 );
    printk("addr of idt 0x80 %x\n", sys_call_off);
    //从0x80中断处理程序的二进制代码中搜索sys_call_table地址    

    p = (char *)sys_call_off;
    for(i = 0; i < 100; i++)
    {
        //0xff 0x14 0x85暂时不明白是什么特殊的汇编代码

        if(p[i] == '\xff' && p[i+1] == '\x14' && p[i+2] == '\x85')
        {
            sys_call_table = *(unsigned int*)(p + i + 3);
            printk("addr of sys_call_table %x\n", sys_call_table);
            return sys_call_table;
        }
    }
    return 0;
}

asmlinkage int (*original_write)(unsigned int, const char __user *, size_t);

asmlinkage int new_write(unsigned int fd, const char __user *buf, size_t count) {

    // hijacked write
    char *name = "cat";
    if (strcmp(current->comm, name) == 0)
    {
        printk(KERN_ALERT "HIAJCK -- write hiajcked and process is %s\n", current->comm);
    }

    return (*original_write)(fd, buf, count);
}

unsigned int clear_and_return_cr0(void)
{
    unsigned int cr0 = 0;
    unsigned int ret;
    asm("movl %%cr0 ,%%eax":"=a"(cr0));
    ret = cr0;
    //清除cr0的WP(Write Protect)标志位

    cr0 &= 0xfffeffff;//第16位为WP

    asm("movl %%eax, %%cr0"::"a"(cr0));
    return ret;
}

void setback_cr0(unsigned int val)
{
    asm("movl %%eax, %%cr0"::"a"(val));
}

void hijack_init(void)
{
    unsigned int orig_cr0;
    printk(KERN_ALERT "HIJACK -- hijack init\n");
    
    syscall_table = (unsigned int*) get_sys_call_table();
    if(syscall_table == 0)
    {
        printk("HIJACK -- can't find syscall_table addr\n");
        return;
    } 
    original_write = syscall_table[__NR_write];
    printk("HIJACK -- addr of original_write %x\n", original_write);
    orig_cr0 = clear_and_return_cr0();
    syscall_table[__NR_write] = (unsigned long) new_write;
    setback_cr0(orig_cr0);
}

void hijack_exit(void)
{
    unsigned int orig_cr0 = clear_and_return_cr0();
    syscall_table[__NR_write] = (unsigned long) original_write;
    setback_cr0(orig_cr0);
    printk(KERN_ALERT "HIJACK -- hijack exit\n");
}