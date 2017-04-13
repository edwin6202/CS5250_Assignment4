#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int fourmb;
void test() {
	int k, i, sum;
	char s[3];
	memset(s, '2', sizeof(s));
	s[3] = '\0';
	
	printf("test begin!\n");
	
	k = lseek(fourmb, 4, SEEK_CUR);
	printf("lseek = %d\n", k);
	
	k = write(fourmb, s, sizeof(s));
	printf("written = %d\n", k);
	
	k = lseek(fourmb, 0, SEEK_END);
	printf("lseek = %d\n", k);
	
	k = lseek(fourmb, -4, SEEK_END);
	printf("lseek = %d\n", k);
	
	k = lseek(fourmb, -4, -1);
	printf("lseek = %d\n", k);
}

void initial(char i) {
	char s[10];
	memset(s, i, sizeof(s));
	write(fourmb, s, sizeof(s));
	
	char c[20] = "";
	
	int k = lseek(fourmb, 0, SEEK_SET);
	printf("lseek = %d\n", k);
}
int main(int argc, char **argv) {
	fourmb = open("/dev/fourmb", O_RDWR);
	
	if (fourmb == -1) {
		printf("unable to open fourmb");
		exit(EXIT_FAILURE);
	}
	
	initial('1');
	test();
	close(fourmb);
	return 0;
}
