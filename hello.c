#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>
#include <linux/types.h>

#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/list.h>

struct list_element {
	struct list_head next;
	ktime_t start_time;
	ktime_t end_time;
};

MODULE_AUTHOR("Stanislav Rudenko <rudenkostas2@gmail.com>");
MODULE_DESCRIPTION("Module for AK-2 Lab6");
MODULE_LICENSE("Dual BSD/GPL");

static LIST_HEAD(head);

static uint counter = 1;
module_param(counter, uint, 0444);
MODULE_PARM_DESC(counter, "This parameter is for number of hello world");

static int __init hello_init(void)
{
	uint i = 0;

	struct list_element *last_var;

	pr_info("Input %d\n", counter);

	if (!counter || (counter >= 5 && counter <= 10))
		pr_warn("WARNING!\tYou should enter number less than 5 and more than 0");

	BUG_ON(counter > 10);

	for (i = 0; i < counter; i++) {
		last_var = kmalloc(sizeof(struct list_element), GFP_KERNEL);

		if (i == 3)
			last_var = 0;

		if (ZERO_OR_NULL_PTR(last_var))
			goto mem_error;

		last_var->start_time = ktime_get();
		pr_info("Hello, world!\n");
		last_var->end_time = ktime_get();

		list_add_tail(&(last_var->next), &head);
	}

	pr_info("");
	return 0;

mem_error:
	{
		struct list_element *md, *tmp;

		pr_err("kmalloc is running out of memory");
		list_for_each_entry_safe(md, tmp, &head, next) {
			list_del(&md->next);
			kfree(md);
		}
		BUG();
		return -ENOMEM;
	}
}

static void __exit hello_exit(void)
{
	struct list_element *md, *tmp;

	list_for_each_entry_safe(md, tmp, &head, next) {
		pr_info("Printing time: %lld", md->end_time - md->start_time);
		list_del(&md->next);
		kfree(md);
	}
	BUG_ON(!list_empty(&head));
	pr_info("");
}

module_init(hello_init);
module_exit(hello_exit);
