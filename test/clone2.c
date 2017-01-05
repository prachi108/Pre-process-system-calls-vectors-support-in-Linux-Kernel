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

#ifndef __NR_clone2
#error clone2 system call not defined
#endif

int main(int argc, char *argv[])
{
	int rc, fp, status;

	printf("calling clone2 with CLONE_SYSCALLS\n");
	rc = syscall(__NR_clone2, SIGCHLD | CLONE_SYSCALLS, 0, NULL, NULL, 0,
		     0);
	if (rc < 0) {
		perror("clone2 failed");
		exit(rc);
	}
	if (rc) {
		/* parent process */
		printf("cloned child with pid = %d\n", rc);
		printf("cloning another child with syscall vector 4\n");
		rc = syscall(__NR_clone2, SIGCHLD, 0, NULL, NULL, 0, 4);
		if (rc < 0)
			perror("clone2 failed");
		printf("cloning another child with syscall vector 1\n");
		rc = syscall(__NR_clone2, SIGCHLD, 0, NULL, NULL, 0, 1);
		if (rc < 0) {
			perror("clone2 failed");
		} else {
			if (rc) {
				printf("cloned child with pid = %d\n", rc);
				waitpid(rc, &status, 0);
			} else {
				fp = open("test1", O_RDWR | O_CREAT, 0644);
				if (fp < 0) {
					fprintf(stderr,
						"open failed for process %d\n",
						getpid());
				} else {
					printf
					    ("open succeeded for process %d\n",
					     getpid());
					close(fp);
				}
				printf("child process (%d) exiting\n",
				       getpid());
				sleep(5);
				exit(rc);
			}
		}
		printf("parent process exiting\n");
		exit(0);
	} else {
		/* child process */
		fp = open("test1", O_RDWR | O_CREAT, 0644);
		if (fp < 0) {
			fprintf(stderr, "open failed for process %d\n",
				getpid());
		} else {
			printf("open succeeded for process %d\n", getpid());
			close(fp);
		}
		printf("child process (%d) exiting\n", getpid());
		sleep(5);
		exit(rc);
	}
}
