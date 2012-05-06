#include <linux/mount.h>
#include <linux/netdevice.h>
#include "util.h"

char* getfullpath2(struct file* file)
{
    char *path=NULL, *start=NULL;
    char *fullpath=NULL;

    fullpath = kmalloc(PATH_MAX,GFP_KERNEL);
    if(!fullpath) goto OUT; 
    memset(fullpath,0,PATH_MAX);

    path = kmalloc(PATH_MAX,GFP_KERNEL);
    if(!path) {
        kfree(fullpath);
        goto OUT; 
    }
    memset(path,0,PATH_MAX);

    //get the path
    start = d_path(&(file->f_path), path, PATH_MAX);
    strcpy(fullpath,start);

    kfree(path);

OUT:
    return fullpath;
}

void putfullpath(char* fullpath)
{
  if(fullpath)
    kfree(fullpath);
}
