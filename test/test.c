#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <signal.h>

#define BUF_SIZE	1024
int main(void)
{
	int ret, fp;
	char buf[] = "hello world\n";
	char rd_buf[BUF_SIZE];

	while (1) {
		fp = open("test1", O_RDWR | O_CREAT, 0644);
		if (fp < 0)
			perror("open failed");
		else
			printf("open succeeded\n");

		ret = read(fp, rd_buf, BUF_SIZE);
		if (ret < 0)
			perror("read failed");
		else
			printf("read %d bytes\n", ret);

		ret = write(fp, buf, strlen(buf));
		if (ret < 0)
			perror("write failed");
		else
			printf("wrote %d bytes\n", ret);

		ret = close(fp);
		if (ret < 0)
			perror("close failed");
		else
			printf("close succeeded\n");

		ret = syscall(__NR_clone, SIGCHLD, 0, NULL, NULL, 0);
		if (ret < 0) {
			perror("clone failed");
		} else {
			if (ret == 0) {
				/* exiting child process */
				exit(0);
			} else {
				printf("clone succeeded\n");
			}
		}
		sleep(10);
	}
	return 0;
}
