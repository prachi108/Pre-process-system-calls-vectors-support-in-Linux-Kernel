#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/ioctl.h>
#include <linux/slab.h>
#include <asm/syscall_vector.h>
#include "syscalldev.h"

#define DEVICE_NUM	313
#define DEVICE_NAME	"syscalldev"

static long set_syscall_vector(unsigned long uarg)
{
	long ret;
	struct svctl_args svargs;

	if (copy_from_user
	    (&svargs, (struct svctl_args *)uarg, sizeof(struct svctl_args))) {
		ret = -EFAULT;
		goto out;
	}
	ret = update_syscall_vector_by_pid(svargs.pid, svargs.vid);
out:
	return ret;
}

static long get_vectors_list(unsigned long arg)
{
	struct syscall_vector *syscall_vectors;
	struct syscall_vector *uarg = (struct syscall_vector *)arg;
	long ret = 0;

	syscall_vectors =
	    kzalloc(sizeof(struct syscall_vector) * MAX_VECTOR_LEN, GFP_KERNEL);
	if (!syscall_vectors) {
		ret = -ENOMEM;
		goto end;
	}
	list_syscall_vectors(syscall_vectors);
	if (copy_to_user
	    (uarg, syscall_vectors,
	     sizeof(struct syscall_vector) * MAX_VECTOR_LEN)) {
		ret = -EFAULT;
	}
	kfree(syscall_vectors);
end:
	return ret;
}

static long get_syscall_vector_id(unsigned long arg)
{
	struct task_struct *tsk;
	long ret = 0;
	struct svctl_args svargs;
	struct svctl_args *uarg = (struct svctl_args *)arg;

	if (copy_from_user(&svargs, uarg, sizeof(struct svctl_args))) {
		ret = -EFAULT;
		goto out;
	}
	tsk = get_pid_task(find_get_pid((pid_t)svargs.pid), PIDTYPE_PID);

	if (!tsk) {
		ret = -EINVAL;
		pr_err("Invalid pid\n");
		goto out;
	}
	if (!tsk->syscall_vector) {
		pr_err("Syscall vector unassigned\n");
		ret = -EINVAL;
		goto out;
	}
	if (copy_to_user
	    (&uarg->vid, &tsk->syscall_vector->id,
	     sizeof(tsk->syscall_vector->id))) {
		ret = -EFAULT;
	}

out:
	return ret;
}

static long syscalldev_ioctl(struct file *file, unsigned int cmd,
			     unsigned long arg)
{
	long ret = 0;

	switch (cmd) {
	case SYSCALLDEV_GET_VECTOR_ID:
		ret = get_syscall_vector_id(arg);
		break;

	case SYSCALLDEV_SET_VECTOR_ID:
		ret = set_syscall_vector(arg);
		break;

	case SYSCALLDEV_GET_VECTOR_LIST:
		ret = get_vectors_list(arg);
		break;

	default:
		pr_err("Invalid cmd received. cmd: %u, args: %lu\n", cmd, arg);
		ret = -EINVAL;
		break;
	}
	return ret;
}

const struct file_operations syscalldev_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = syscalldev_ioctl,
};

static int __init init_syscalldev(void)
{
	int ret;

	pr_info("installing device: %s\n", DEVICE_NAME);
	ret = register_chrdev(DEVICE_NUM, DEVICE_NAME, &syscalldev_fops);
	if (ret < 0)
		pr_info("installing device failed with error: %d\n", ret);
	return ret;
}

static void __exit exit_syscalldev(void)
{
	unregister_chrdev(DEVICE_NUM, DEVICE_NAME);
	pr_info("removed device: %s\n", DEVICE_NAME);
}

module_init(init_syscalldev);
module_exit(exit_syscalldev);
MODULE_LICENSE("GPL");
