#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

//Needed for IO things. Attention that this is different from kernel mode
int fourmb;
#define FOURMB_IOC_MAGIC 'k'
#define FOURMB_HELLO _IO(FOURMB_IOC_MAGIC, 0)
#define FOURMB_WRITE_MSG _IOW(FOURMB_IOC_MAGIC, 1, int)
#define FOURMB_READ_MSG _IOR(FOURMB_IOC_MAGIC, 2, int)
#define FOURMB_WRITEREAD_MSG _IOWR(FOURMB_IOC_MAGIC, 3, int)

void test()
{
	int k, i, sum;
	char s[3];
	
	memset(s, '2', sizeof(s));
	printf("test begin! \n");
	
	k = write (fourmb, s, sizeof(s));
	printf("written = %d \n", k);
	
	k = ioctl(fourmb, FOURMB_HELLO);
	printf("result = %d \n", k);

	char* data1 = "write this";
	k = ioctl(fourmb, FOURMB_WRITE_MSG, data1);
	printf("result = %d \n", k);

	char* user_msg = malloc(sizeof(char) * 100);
	k = ioctl(fourmb, FOURMB_READ_MSG, user_msg);
	printf("User Message: %s \n", user_msg);

	char data2[] = {'w', 'r', 'i', 't', 'e', ' ', 'a', 'g', 'a', 'i', 'n'};
	k = ioctl(fourmb, FOURMB_WRITEREAD_MSG, data2);
	printf("New User Message = %s \n", data2);
	
}

int main(int argc, char **argv)
{
	fourmb = open("/dev/fourmb", O_RDWR);
	
	if (fourmb == -1) {
		perror("unable to open fourmb");
		exit(EXIT_FAILURE);
	}
	
	test();
	close(fourmb);

	return 0;
}
