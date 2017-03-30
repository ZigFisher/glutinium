#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 64

int main(int argc, char** argv)
{
	if (argc <=  1 ) {
		printf("Usage: shbin <binary>\n");
		exit(0);
	}
	FILE *f;
	int i;
	char buf[BUF_SIZE];
	int readed;
	if ( ! (f = fopen(argv[1], "r")) ) {
		perror("fopen");
		exit(1);
	}
	printf("#!/bin/sh\n\n");
	while ( 1 ) {
		readed = fread(&buf, 1, BUF_SIZE, f);
		//fprintf(stderr, "readed: %d\n", readed);
		printf("echo -ne \"");
		for ( i = 0; i < readed; i++)
			printf("\\%.4hho", buf[i]);
		printf("\";\n");
		if ( readed < BUF_SIZE )
			break;
	}
	fclose(f);

	return 0;
}
