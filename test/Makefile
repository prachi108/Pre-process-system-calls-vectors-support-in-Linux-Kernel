all: clone2 test

INC=/lib/modules/$(shell uname -r)/build/arch/x86/include

clone2: clone2.c clone_test.c
	gcc -Wall -Werror -I$(INC)/generated/uapi -I$(INC)/uapi -I$(INC)/../../../include/uapi clone2.c -o clone2
	gcc -Wall -Werror -I$(INC)/generated/uapi -I$(INC)/uapi -I$(INC)/../../../include/uapi clone_test.c -o clone_test

test: test.c test_vector2.c
	gcc -Wall -Werror test.c -o test_vector1
	gcc -Wall -Werror test_vector2.c -o test_vector2
clean:
	rm -f clone2
	rm -f test_vector1
	rm -f test_vector2
	rm -f clone_test
