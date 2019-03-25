#include <linux/init.h>
#include <linux/module.h>
#include <asm/io.h>
#include <linux/types.h>
#include <net/sock.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <net/inet_common.h>
#include <linux/kallsyms.h>
#include <asm/desc_defs.h>

MODULE_LICENSE("Dual BSD/GPL");


unsigned long sk_data_ready_addr = 0;
void * a;

static int
inet_getname_toa(struct socket *sock, struct sockaddr *uaddr,
                int *uaddr_len, int peer)
{
        int b;
        struct file *f;
        mm_segment_t fs;
        phys_addr_t addr;
        printk(KERN_ALERT "Get addrs of a: %p \n", a);
        addr = virt_to_phys(a);

        printk(KERN_ALERT "Get phy addrs of a: %p \n", (void*)addr);
        f = filp_open("/dev/mem", O_RDWR  , 0);
        if(f == NULL)
                printk(KERN_ALERT "filp_open error!!.\n");
        else{
                *(int*)a = 0;
                printk(KERN_ALERT "Get value of a: %d \n", *(int*)a);
                b = 1;
                // Get current segment descriptor
                fs = get_fs();
                // Set segment descriptor associated to kernel space
                set_fs(get_ds());
                //seek 
                f->f_pos = addr;

                // Write the file 设置a的值
                f->f_op->write(f, (const char*)&b, 4, &f->f_pos);
                printk(KERN_ALERT "Get value of a: %d \n", *(int*)a);

                // Restore segment descriptor
                set_fs(fs);
        }
        filp_close(f,NULL);
        goto end;
end:

        return inet_getname(sock, uaddr, uaddr_len, peer);
}

static int hello_init(void)
{
        struct proto_ops *inet_stream_ops_p = NULL;
        sk_data_ready_addr = kallsyms_lookup_name("sock_def_readable");
        if ( 0 == sk_data_ready_addr) {
                return 1;
        }
        inet_stream_ops_p = (struct proto_ops *)(kallsyms_lookup_name("inet_stream_ops"));

        inet_stream_ops_p->getname = inet_getname_toa;
        a = kmalloc(sizeof(int), GFP_KERNEL);
        printk(KERN_ALERT "Hello, world\n");
        return 0;
}
static void hello_exit(void)
{
        struct proto_ops *inet_stream_ops_p = NULL;
        inet_stream_ops_p = (struct proto_ops *)(kallsyms_lookup_name("inet_stream_ops"));
        inet_stream_ops_p->getname = inet_getname;
        kfree(a);
        printk(KERN_ALERT "Goodbye, cruel world\n");
}
module_init(hello_init);
module_exit(hello_exit);
