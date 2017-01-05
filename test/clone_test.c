#include <asm/unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <signal.h>
#include <linux/sched.h>
#include <sys/wait.h>
#include <fcntl.h>

#ifndef __NR_clone
#error clone system call not defined
#endif

int main(int argc, char *argv[])
{
	int rc;

	printf("calling default clone with CLONE_SYSCALLS\n");
	printf("Waiting..change syscall vector\n");
	sleep(20);
	rc = syscall(__NR_clone, SIGCHLD | CLONE_SYSCALLS, 0, NULL, NULL, 0);
	if (rc < 0) {
		perror("clone failed");
		exit(rc);
	}
	if (rc) {
		/* parent process */
		printf("cloned child with pid = %d\n", rc);
		sleep(10);
		printf("Parent process (%d) exiting\n", getpid());
		exit(0);
	} else {
		/* child process */
		sleep(10);
		printf("child process (%d) exiting\n", getpid());
		exit(rc);
	}
}
