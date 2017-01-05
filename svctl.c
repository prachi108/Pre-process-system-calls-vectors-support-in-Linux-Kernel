#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include "syscalldev.h"

static int execute_ioctl(int pidflag, int pid, int vidflag, unsigned int vid);
static int ioctl_get_vector_list(int fd);
static int ioctl_get_vector_id(int fd, int pid);
static int ioctl_set_vector_id(int fd, int pid, unsigned int vid);
static void print_vectorlist(struct syscall_vector *sysvec_list);
static void print_help(char *);
static int validate_int(char *str);

int main(int argc, char *argv[])
{
	int ret;
	int opt;
	int pid;
	unsigned int vid;
	int pidflag = 0;
	int vidflag = 0;

	while ((opt = getopt(argc, argv, "p:v:h")) != -1) {
		switch (opt) {
		case 'p':
			pidflag = 1;
			pid = validate_int(optarg);
			if (pid <= 0) {
				fprintf(stderr, "%s: invalid process id\n",
					argv[0]);
				ret = pid;
				goto out;
			}
			break;

		case 'v':
			vidflag = 1;
			vid = validate_int(optarg);
			if (vid < 0) {
				fprintf(stderr, "%s: invalid vector id\n",
					argv[0]);
				ret = vid;
				goto out;
			}
			break;

		case 'h':
			print_help(argv[0]);
			ret = 0;
			goto out;

		default:
			print_help(argv[0]);
			ret = -EINVAL;
			goto out;
		}
	}

	if (argv[optind]) {
		fprintf(stderr, "%s: invalid argument %s\n", argv[0],
			argv[optind]);
		print_help(argv[0]);
		return -EINVAL;
		goto out;
	}

	ret = execute_ioctl(pidflag, pid, vidflag, vid);
out:
	return ret;
}

int execute_ioctl(int pidflag, int pid, int vidflag, unsigned int vid)
{
	char *devname = "/dev/syscalldev";
	int ret = 0;
	int fd;

	fd = open(devname, O_RDWR);
	if (fd < 0) {
		perror("failed to open device");
		return fd;
	}

	if (pidflag && vidflag) {
		ret = ioctl_set_vector_id(fd, pid, vid);
	} else if (pidflag) {
		ret = ioctl_get_vector_id(fd, pid);
	} else if (!pidflag && !vidflag) {
		ret = ioctl_get_vector_list(fd);
	} else {
		fprintf(stderr, "Unsupported options!\n");
		ret = -EINVAL;
	}

	close(fd);
	return ret;
}

int ioctl_set_vector_id(int fd, int pid, unsigned int vid)
{
	int ret = 0;
	struct svctl_args svargs = {
		.pid = pid,
		.vid = vid
	};

	ret = ioctl(fd, SYSCALLDEV_SET_VECTOR_ID, &svargs);
	if (ret < 0)
		perror("set vector id failed");
	else
		printf("Successfully set vector id: %d to process id: %d\n",
		       svargs.vid, svargs.pid);
	return ret;
}

int ioctl_get_vector_id(int fd, int pid)
{
	int ret = 0;
	struct svctl_args svargs = {
		.pid = pid
	};

	ret = ioctl(fd, SYSCALLDEV_GET_VECTOR_ID, &svargs);
	if (ret < 0) {
		perror("get vector id failed");
		goto out;
	}
	printf("%u\n", svargs.vid);

out:
	return ret;
}

int ioctl_get_vector_list(int fd)
{
	int ret = 0;
	struct syscall_vector sysvec_list[MAX_VECTOR_LEN];

	ret = ioctl(fd, SYSCALLDEV_GET_VECTOR_LIST, sysvec_list);
	if (ret < 0) {
		perror("get list of syscall vectors failed");
		goto out;
	}
	print_vectorlist(sysvec_list);

out:
	return ret;
}

int validate_int(char *str)
{
	long val;
	char *endp = NULL;

	if (!str)
		return -EINVAL;
	val = strtol(str, &endp, 0);
	/* check if str contains chars other than digits */
	if (endp && *endp)
		return -EINVAL;
	if (val < 0)
		return -EINVAL;
	return (int)val;
}

void print_vectorlist(struct syscall_vector *sv)
{
	int i = 0;

	for (i = 0; i < MAX_VECTOR_LEN && sv[i].name[0]; i++) {
		printf("\n");
		printf("Syscall Vector ID : %u\n", sv[i].id);
		printf("Syscall Vector Name: %s\n", sv[i].name);
		printf("Number of processes using this vector : %u\n",
		       sv[i].nr_process);
	}
}

void print_help(char *exe)
{
	printf("Usage: %s [-p <process id>] [-v <vector id>] [-h]\n", exe);
	printf("Displays and controls syscall vectors\n");
	printf("\nExamples:\n");
	printf("\t1. List all syscall vectors\n");
	printf("\t\t%s\n", exe);
	printf("\t2. Print syscall vector id of a process\n");
	printf("\t\t%s -p <process id>\n", exe);
	printf("\t3. Change syscall vector of a process\n");
	printf("\t\t%s -p <process id> -v <vector id>\n", exe);
	printf("\t4. Print this help\n");
	printf("\t\t%s -h\n", exe);
}
