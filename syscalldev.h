#ifndef __SYSCALLDEV_H_
#define __SYSCALLDEV_H_

#include <linux/sys_vec.h>

struct svctl_args {
	int pid;
	unsigned int vid;
};

#define SYSCALLDEV_MAGIC		's'
#define SYSCALLDEV_GET_VECTOR_ID	_IOR(SYSCALLDEV_MAGIC, 0,\
						struct svctl_args)
#define SYSCALLDEV_GET_VECTOR_LIST	_IOR(SYSCALLDEV_MAGIC, 1,\
						struct syscall_vector *)
#define SYSCALLDEV_SET_VECTOR_ID	_IOW(SYSCALLDEV_MAGIC, 2,\
						struct svctl_args)

#endif
