#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/list.h>
#include <linux/slab.h>


static uint count = 1;
module_param(count, uint, 0444);
MODULE_PARM_DESC(count, "How many times to print 'Hello, world!'");

static LIST_HEAD(hello_list);

struct hello_item {
    struct list_head list;
    ktime_t time;
};


static int __init hello_init(void)
{
    int i;

    if (count > 10) {
        pr_err("Invalid count value: %u (must be <= 10)\n", count);
        return -EINVAL;
    }

    if (count == 0 || (count >= 5 && count <= 10)) {
        pr_warn("Suspicious count value: %u\n", count);
    }

    for (i = 0; i < count; i++) {
        struct hello_item *item;

        item = kmalloc(sizeof(*item), GFP_KERNEL);
        if (!item) {
            pr_err("Memory allocation failed\n");
            return -ENOMEM;
        }

        item->time = ktime_get();
        list_add_tail(&item->list, &hello_list);

        pr_info("Hello, world!\n");
    }

    return 0;
}

static void __exit hello_exit(void)
{
    struct hello_item *item, *tmp;

    list_for_each_entry_safe(item, tmp, &hello_list, list) {
        pr_info("Event time (ns): %lld\n", ktime_to_ns(item->time));
        list_del(&item->list);
        kfree(item);
    }
    pr_info("Goodbye, world!\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Illia Zernyshkin");
MODULE_DESCRIPTION("Simple Hello World module");
