#include <linux/init.h>
#include <linux/module.h>
#include <net/sock.h>
#include <net/inet_common.h>
#include <linux/kallsyms.h>
#include <asm/desc_defs.h>

MODULE_LICENSE("Dual BSD/GPL");

//static int filter = 0;
unsigned long sk_data_ready_addr = 0;

static int
inet_getname_toa(struct socket *sock, struct sockaddr *uaddr,
                int *uaddr_len, int peer)
{
        unsigned long *var;
        int j;
        struct desc_struct* dp;
        struct desc_struct* dp_h;
        printk(KERN_ALERT "before asm1\n");
        asm (
                        "sub $10,%%rsp;"
                        "sgdt (%%rsp);"
                        "mov %%rsp, %%rax;"
                        "add $2,%%rax;"
                        :"=a"(var)
            );
        printk(KERN_ALERT "before asm2\n");
        asm (
                        "add $10,%rsp;"
            );
        dp_h = (struct desc_struct*)*var;
        printk(KERN_ALERT "get gdt %p\n", dp_h);
        for (j=2;j<7;j++)
        {
                printk(KERN_ALERT "get kernel seg descriptor ptr[%d] %p\n",j, dp_h+j);
                dp = dp_h +j;
                printk(KERN_ALERT "get kernel seg descriptro limit0[%d] limit[%d] base0[%d] base1[%d] base2[%d] dpl[%d] \n",
                                dp->limit0, dp->limit, dp->base0, dp->base1, dp->base2, dp->dpl  );
        }

        /* call orginal one */
        return inet_getname(sock, uaddr, uaddr_len, peer);
}


static int hello_init(void)
{
        struct proto_ops *inet_stream_ops_p = NULL;
        printk(KERN_ALERT "before get symbol addr\n");
        sk_data_ready_addr = kallsyms_lookup_name("sock_def_readable");
        if ( 0 == sk_data_ready_addr) {
                return 1;
        }
        printk(KERN_ALERT "before get get hook function\n");
        printk(KERN_ALERT "&inet_stream_ops %p \n", &inet_stream_ops);
        inet_stream_ops_p = (struct proto_ops *)(kallsyms_lookup_name("inet_stream_ops"));
        printk(KERN_ALERT "symbol inet_stream_ops addr %p\n", inet_stream_ops_p);

        printk(KERN_ALERT "before update hook\n");
        inet_stream_ops_p->getname = inet_getname_toa;
        printk(KERN_ALERT "Hello, world\n");
        return 0;
}
static void hello_exit(void)
{
        printk(KERN_ALERT "Goodbye, cruel world\n");
}
module_init(hello_init);
module_exit(hello_exit);
