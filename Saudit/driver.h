#ifndef _DRIVER_H_
#define _DRIVER_H_

#ifndef SAUDIT_DEVICE_MAJOR
#define SAUDIT_DEVICE_MAJOR 88 /*预设的device的主设备号*/
#endif

#ifndef SCULL_P_NR_DEVS
#define SCULL_P_NR_DEVS 1 /*设备数*/
#endif

#ifndef SCULL_P_BUFFER
#define SCULL_P_BUFFER 4096
#endif

void driver_init(void);
void driver_exit(void);

#endif 
