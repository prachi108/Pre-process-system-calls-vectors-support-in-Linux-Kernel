#include <linux/module.h>
#include <asm/syscall_vector.h>

/* syscalls overridden by this module */
enum overridden_syscalls {
	OPEN,
	READ,
	WRITE,
	CLOSE,
	CLONE,
	MAX_OVERRIDDEN_SYSCALLS
};

/* syscall table used by this vector */
static sys_call_ptr_t syscall_tbl[__NR_syscall_max + 1];
/* table containing original (unmodified) syscalls */
static sys_call_ptr_t org_syscall_tbl[MAX_OVERRIDDEN_SYSCALLS];

static long sys_open1(unsigned long arg1, unsigned long arg2,
		      unsigned long arg3, unsigned long arg4,
		      unsigned long arg5, unsigned long arg6)
{
	pr_info("sys_open issued by user (%u)\n", current->real_cred->uid.val);
	/* Disallow all users except root */
	if (current->real_cred->uid.val)
		return -EPERM;
	return org_syscall_tbl[OPEN] (arg1, arg2, arg3, arg4, arg5, arg6);
}

static long sys_read1(unsigned long arg1, unsigned long arg2,
		      unsigned long arg3, unsigned long arg4,
		      unsigned long arg5, unsigned long arg6)
{
	pr_info("sys_read issued by user (%u)\n", current->real_cred->uid.val);
	/* Disallow all users except root */
	if (current->real_cred->uid.val)
		return -EPERM;
	return org_syscall_tbl[READ] (arg1, arg2, arg3, arg4, arg5, arg6);
}

static long sys_write1(unsigned long arg1, unsigned long arg2,
		       unsigned long arg3, unsigned long arg4,
		       unsigned long arg5, unsigned long arg6)
{
	pr_info("sys_write issued by user (%u)\n", current->real_cred->uid.val);
	return org_syscall_tbl[WRITE] (arg1, arg2, arg3, arg4, arg5, arg6);
}

static long sys_close1(unsigned long arg1, unsigned long arg2,
		       unsigned long arg3, unsigned long arg4,
		       unsigned long arg5, unsigned long arg6)
{
	pr_info("sys_close issued by user (%u)\n", current->real_cred->uid.val);
	/* Disallow all users except root */
	if (current->real_cred->uid.val)
		return -EPERM;
	return org_syscall_tbl[CLOSE] (arg1, arg2, arg3, arg4, arg5, arg6);
}

static long sys_clone1(unsigned long arg1, unsigned long arg2,
		       unsigned long arg3, unsigned long arg4,
		       unsigned long arg5, unsigned long arg6)
{
	pr_info("sys_clone issued by user (%u)\n", current->real_cred->uid.val);
	/* Disallow all users except root */
	if (current->real_cred->uid.val)
		return -EPERM;
	return org_syscall_tbl[CLONE] (arg1, arg2, arg3, arg4, arg5, arg6);
}

static struct syscall_vector_info syscall_vector1 = {
	.name = "syscall vector 1 (open, read, write, close, clone)",
	.id = 1,
	.owner = THIS_MODULE,
	.nr_process = 0,
	.nr_process_lock = __SPIN_LOCK_UNLOCKED(sys_vec1.nr_process_lock),
	.syscalls = syscall_tbl,
	.next = NULL
};

static int __init syscall_vector_init(void)
{
	int err = 0;

	pr_info("Registering syscall vector 1\n");
	get_default_syscall_table(syscall_tbl);
	org_syscall_tbl[OPEN] = syscall_tbl[__NR_open];
	syscall_tbl[__NR_open] = sys_open1;
	org_syscall_tbl[CLOSE] = syscall_tbl[__NR_close];
	syscall_tbl[__NR_close] = sys_close1;
	org_syscall_tbl[READ] = syscall_tbl[__NR_read];
	syscall_tbl[__NR_read] = sys_read1;
	org_syscall_tbl[WRITE] = syscall_tbl[__NR_write];
	syscall_tbl[__NR_write] = sys_write1;
	org_syscall_tbl[CLONE] = syscall_tbl[__NR_clone];
	syscall_tbl[__NR_clone] = sys_clone1;
	err = register_syscall_vector(&syscall_vector1);
	return err;
}

static void __exit syscall_vector_exit(void)
{
	unregister_syscall_vector(&syscall_vector1);
}

module_init(syscall_vector_init);
module_exit(syscall_vector_exit);
MODULE_LICENSE("GPL");
