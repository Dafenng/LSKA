#include <linux/init.h>   
#include <linux/module.h>   
#include <linux/timer.h> 
#include <linux/time.h>  
#include <linux/types.h>
#include <net/sock.h>
#include <net/netlink.h> //it include linux/netlink.h

#define NETLINK_TEST 17
#define MAX_MSGSIZE 1024

struct sock *nl_sk = NULL;

// Get string's length
int stringlength(char *s)
{
	int slen = 0;

	for(; *s; s++){
        slen++;
    }

	return slen;
}

// Send message by netlink
void sendnlmsg(char *message)  
{
	struct sk_buff *skb;
	struct nlmsghdr *nlh;
	int len = NLMSG_SPACE(MAX_MSGSIZE);
	int slen = 0;
	
	if(!message || !nl_sk){
        return;
    }

	// Allocate a new sk_buffer
	skb = alloc_skb(len, GFP_KERNEL);
	if(!skb){
		printk(KERN_ERR "my_net_link: alloc_skb Error./n");
		return;
	}

	slen = stringlength(message);

    //Initialize the header of netlink message
	nlh = nlmsg_put(skb, 0, 0, 0, MAX_MSGSIZE, 0);

	NETLINK_CB(skb).pid = 0; // from kernel
    NETLINK_CB(skb).dst_group = 1; // multi cast

	message[slen] = '/0';
	memcpy(NLMSG_DATA(nlh), message, slen+1);
    printk("my_net_link: send message '%s'./n", (char *)NLMSG_DATA(nlh));

	//send message by multi cast
    netlink_broadcast(nl_sk, skb, 0, 1, GFP_KERNEL); 
    return;
}

// Initialize netlink
int netlink_init(void)
{
    int i = 10;
    struct completion cmpl;

    nl_sk = netlink_kernel_create(&init_net, NETLINK_TEST, 1, 
                                  NULL, NULL, THIS_MODULE);

	if(!nl_sk){
		printk(KERN_ERR "my_net_link: create netlink socket error./n");
		return 1;
	}

	printk("my_net_link: create netlink socket ok./n");
    while(i--){
        init_completion(&cmpl);
        wait_for_completion_timeout(&cmpl, 3 * HZ);
        sendnlmsg("I am from kernel!");
        
    }

	return 0;
}

static void netlink_exit(void)   
{   
	if(nl_sk != NULL){
    	sock_release(nl_sk->sk_socket);
  	}

	printk("my_net_link: self module exited/n");   
}

module_init(netlink_init);   
module_exit(netlink_exit);   

MODULE_AUTHOR("donglp");   
MODULE_LICENSE("GPL"); 