/*
* simple process listing module
*/

#include <linux/module.h>
#include <linux/printk.h>
#include <linux/sched.h>

static int __init ex_init(void)
{
    struct task_struct *task;

    for_each_process(task)
      if (task -> state != 0){
        printk("Name:%s\tPID:%d\tParent PID:%d\tState:%li\n",task->comm,
         task->pid, task -> parent -> pid, task -> state);
      }

    return 0;
}

static void __exit ex_fini(void)
{
}

module_init(ex_init);
module_exit(ex_fini);
