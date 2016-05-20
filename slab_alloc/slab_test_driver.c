#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/proc_fs.h>
#include <asm/io.h>
#include <asm/uaccess.h>

static struct kmem_cache *mycache;

struct my_cache_data {
	int i[4096];
};

struct my_cache_data *object1, *object2;
int n = 0 ;
void slab_ctor(void *mycache) {
	printk("Call slab_ctor :%d times.\n", n++);
}

static void init_mycache( void) {
	mycache = kmem_cache_create("mycache_hello", sizeof(struct my_cache_data), 0,
					SLAB_HWCACHE_ALIGN, slab_ctor);

	if (mycache)
		printk("create mycache.\n");

	return;
}

static int stab_test(void) {
	
	void *object;
	int i = 1024*1024 / 4;

	printk("cache object size:%d\n", 
			kmem_cache_size(mycache));

	object1 = (struct my_cache_data *)kmem_cache_alloc(mycache, GFP_KERNEL);
	if (!object1) {
		printk("kmem cache alloc object1, failed.\n");
		return -1;
	}
	printk("kmem cache alloc object1, ok.\n");
	object2 = (struct my_cache_data *)kmem_cache_alloc(mycache, GFP_KERNEL);
	if (!object2) {
		printk("kmem cache alloc object2, failed.\n");
		return -1;
	}

	while (i-- > 0) {
		object = (struct my_cache_data *)kmem_cache_alloc(mycache, GFP_KERNEL);
		if (!object) {
			printk("kmem cache alloc object, failed.\n");
			return -1;
		}
	}
	printk("kmem cache alloc object2, ok.\n");
	return 0;
}

static void remove_mycache(void) {

	if (object1)
		kmem_cache_free(mycache, object1);
	if (object2)
		kmem_cache_free(mycache, object2);
	if (mycache)
		kmem_cache_destroy(mycache);

	return;
}

int slab_test_driver_init(void) {
	printk("Ok to install slab test_driver\n");

	init_mycache();
	stab_test();

	return 0;
}
static void slab_test_driver_exit(void){
	printk("Ok to remove slab test_driver\n");
	remove_mycache();
	printk("remove mycache.\n");
}

module_init(slab_test_driver_init);
module_exit(slab_test_driver_exit);

MODULE_LICENSE("GPL");
