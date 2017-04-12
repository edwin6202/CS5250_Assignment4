#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FIVE_MEGABYTES 5242880
#define FOUR_MEGABYTES 4194304

int main(int argc, char** argv)
{
	char *fivemb_chars = malloc(FIVE_MEGABYTES);
	char *fourmb_chars = malloc(FOUR_MEGABYTES);

	char head[50];
	char tail[50];
	int numBytes = 0;

	FILE *fivemb_file = fopen("fivemb.txt", "r+");
	FILE *fourmb_file = fopen("/dev/fourmb", "a+");

	//Reading fromm fivemb file
	while (fgets(fivemb_chars, FIVE_MEGABYTES, fivemb_file) != 0) {

		int length = strlen(fivemb_chars);

		int flag = 0;
		for (int i = 0; i < length; i++)
		{
			
			if (fivemb_chars[i] >= 'a' && fivemb_chars[i] <= 'z')
			{
				numBytes++;
				flag = fputc(fivemb_chars[i], fourmb_file);

				if (flag < 'a' || flag > 'z')
				{
					numBytes--;
				}
			}
			
			
		}	
	}

	fputc('\0', fourmb_file);

	

	printf("Nummber of bytes written to the file: %i \n", numBytes);

	fclose(fivemb_file);
	fclose(fourmb_file);



	
}
