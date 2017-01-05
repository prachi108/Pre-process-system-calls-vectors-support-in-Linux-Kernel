#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>

int main(void)
{
	int fp, mod;
	int user_input, count = 0;
	char *buff = (char *)malloc(50);
	char *buff2 = (char *)malloc(50);
	int loop = 1;

	while (loop) {
		printf("Loop count: %d\n", count);
		printf("Enter 1 to test syscall chmod\n");
		printf("Enter 2 to test syscall mkdir\n");
		printf("Enter 3 to test syscall rmdir\n");
		printf("Enter 4 to test syscall link\n");
		printf("Enter 5 to test syscall unlink\n");
		printf("Enter 0 to exit testing\n");
		scanf("%d", &user_input);
		if (user_input == 1) {
			scanf("%s %d", buff, &mod);
			printf("chmod %s %d\n", buff, mod);
			fp = syscall(__NR_chmod, buff, mod);
			if (fp < 0)
				perror("chmod failed");
		} else if (user_input == 2) {
			scanf("%s", buff);
			printf("mkdir %s\n", buff);
			fp = syscall(__NR_mkdir, buff, 0644);
			if (fp < 0)
				perror("mkdir failed");
		} else if (user_input == 3) {
			scanf("%s", buff);
			printf("rmdir %s\n", buff);
			fp = syscall(__NR_rmdir, buff);
			if (fp < 0)
				perror("rmdir failed");
		} else if (user_input == 4) {
			scanf("%s %s", buff, buff2);
			printf("link %s %s\n", buff, buff2);
			fp = syscall(__NR_link, buff, buff2);
			if (fp < 0)
				perror("Link failed");
		} else if (user_input == 5) {
			scanf("%s", buff);
			printf("unlink %s\n", buff);
			fp = syscall(__NR_unlink, buff);
			if (fp < 0)
				perror("unlink failed");
		} else if (user_input == 0) {
			loop = 0;
		} else {
			printf("Invalid Input\n");
		}
		count++;
	}

	if (buff)
		free(buff);
	if (buff2)
		free(buff2);

	return 0;
}
