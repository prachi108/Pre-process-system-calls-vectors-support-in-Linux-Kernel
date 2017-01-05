#include <linux/slab.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <asm/syscall_vector.h>

/* syscalls overridden by this module */
enum overridden_syscalls {
	CHMOD,
	MKDIR,
	RMDIR,
	LINK,
	UNLINK,
	MAX_OVERRIDDEN_SYSCALLS
};

/* syscall table used by this vector */
static sys_call_ptr_t syscall_tbl[__NR_syscall_max + 1];
/* table containing original (unmodified) syscalls */
static sys_call_ptr_t org_syscall_tbl[MAX_OVERRIDDEN_SYSCALLS];

static long sys_chmod1(unsigned long arg1, unsigned long arg2,
		       unsigned long arg3, unsigned long arg4,
		       unsigned long arg5, unsigned long arg6)
{
	pr_info("sys_chmod issued by user (%u)\n", current->real_cred->uid.val);
	/*  Not allowing to change permissions to OTHERS to execute */
	if (arg2 & 1)
		return -EPERM;
	return org_syscall_tbl[CHMOD] (arg1, arg2, arg3, arg4, arg5, arg6);
}

static long sys_mkdir1(unsigned long arg1, unsigned long arg2,
		       unsigned long arg3, unsigned long arg4,
		       unsigned long arg5, unsigned long arg6)
{
	char *dir_name = kmalloc(PATH_MAX, GFP_KERNEL);

	pr_info("sys_mkdir issued by user (%u)\n", current->real_cred->uid.val);

	/* Not allowing to mkdir to create hidden directories */
	if (copy_from_user(dir_name, (void *)arg1, PATH_MAX) == 0) {
		if (strchr(dir_name, '.')) {
			kfree(dir_name);
			return -EPERM;
		}
	}
	kfree(dir_name);

	return org_syscall_tbl[MKDIR] (arg1, arg2, arg3, arg4, arg5, arg6);
}

static long sys_rmdir1(unsigned long arg1, unsigned long arg2,
		       unsigned long arg3, unsigned long arg4,
		       unsigned long arg5, unsigned long arg6)
{
	char *dir_name = kmalloc(PATH_MAX, GFP_KERNEL);

	pr_info("sys_rmdir issued by user (%u)\n", current->real_cred->uid.val);

	/* Not allowing rmdir to accidently remove important directories */
	if (copy_from_user(dir_name, (void *)arg1, PATH_MAX) == 0) {
		if (strstr(dir_name, "important")) {
			kfree(dir_name);
			return -EPERM;
		}
	}
	kfree(dir_name);
	return org_syscall_tbl[RMDIR] (arg1, arg2, arg3, arg4, arg5, arg6);
}

static long sys_link1(unsigned long arg1, unsigned long arg2,
		      unsigned long arg3, unsigned long arg4,
		      unsigned long arg5, unsigned long arg6)
{
	char *dir_name = kmalloc(PATH_MAX, GFP_KERNEL);

	pr_info("sys_link issued by user (%u)\n", current->real_cred->uid.val);

	/* Not allowing link to create hard links
	 * for tmp files (tmp are generally short living)
	 */
	if (copy_from_user(dir_name, (void *)arg1, PATH_MAX) == 0) {
		if (strstr(dir_name, "tmp")) {
			kfree(dir_name);
			return -EPERM;
		}
	}
	kfree(dir_name);
	return org_syscall_tbl[LINK] (arg1, arg2, arg3, arg4, arg5, arg6);
}

static long sys_unlink1(unsigned long arg1, unsigned long arg2,
			unsigned long arg3, unsigned long arg4,
			unsigned long arg5, unsigned long arg6)
{
	pr_info("sys_unlink issued by user (%u)\n",
		current->real_cred->uid.val);
	/* Not allowing process "virus" to unlink files */
	if (strstr(current->comm, "virus"))
		return -EPERM;
	return org_syscall_tbl[UNLINK] (arg1, arg2, arg3, arg4, arg5, arg6);
}

static struct syscall_vector_info syscall_vector2 = {
	.name = "syscall vector 2 (chmod, mkdir, rmdir, link, unlink)",
	.id = 2,
	.nr_process = 0,
	.nr_process_lock = __SPIN_LOCK_UNLOCKED(sys_vec1.nr_process_lock),
	.syscalls = syscall_tbl,
	.next = NULL
};

static int __init syscall_vector_init(void)
{
	int err = 0;

	pr_info("Registering syscall vector 2\n");
	get_default_syscall_table(syscall_tbl);
	org_syscall_tbl[CHMOD] = syscall_tbl[__NR_chmod];
	syscall_tbl[__NR_chmod] = sys_chmod1;
	org_syscall_tbl[MKDIR] = syscall_tbl[__NR_mkdir];
	syscall_tbl[__NR_mkdir] = sys_mkdir1;
	org_syscall_tbl[RMDIR] = syscall_tbl[__NR_rmdir];
	syscall_tbl[__NR_rmdir] = sys_rmdir1;
	org_syscall_tbl[LINK] = syscall_tbl[__NR_link];
	syscall_tbl[__NR_link] = sys_link1;
	org_syscall_tbl[UNLINK] = syscall_tbl[__NR_unlink];
	syscall_tbl[__NR_unlink] = sys_unlink1;
	err = register_syscall_vector(&syscall_vector2);
	return 0;
}

static void __exit syscall_vector_exit(void)
{
	unregister_syscall_vector(&syscall_vector2);
}

module_init(syscall_vector_init);
module_exit(syscall_vector_exit);
MODULE_LICENSE("GPL");
