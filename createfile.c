#include <stdio.h>
#define FIVE_MEGABYTES 5242880

char alphabets[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '\n' };

int main(int argc, char** argv)
{
	FILE *five_megabytes_file = fopen("fivemb.txt", "a+");

	for (long i = 0; i < FIVE_MEGABYTES - 1; i++)
	{
		fputc(alphabets[i%27], five_megabytes_file);
	}

	fputc('\0', five_megabytes_file);

	fclose(five_megabytes_file);
}
