#ifndef _SAUDIT_H_
#define _SAUDIT_H_

#ifndef SAUDIT_DEVICE_MAJOR
#define SAUDIT_DEVICE_MAJOR 88 /*预设的device的主设备号*/
#endif

#ifndef SAUDIT_DEVICE_NR_DEVS
#define SAUDIT_DEVICE_NR_DEVS 2 /*设备数*/
#endif

#ifndef SAUDIT_DEVICE_SIZE
#define SAUDIT_DEVICE_SIZE 4096
#endif

/*device设备描述结构体*/
struct saudit_device 
{ 
  char *data; 
  unsigned long size; 
};

#endif 
