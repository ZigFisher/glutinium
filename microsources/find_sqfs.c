/*
 * Author: Vladislav Moskovets for ZFTLab
 * Licence: GPLv3
 * Date: 20090526
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

void usage()
{
	fprintf(stderr, "Usage: find_sqfs -i infile -o outfile [-s signature]\n");
	exit(1);
}
int main(int argc, char** argv) {
	char *infilename = "";
	char *outfilename = "";
	int optind = 1;
	FILE *infile = NULL;
	FILE *outfile = NULL;
	int fsize;
	char* big_buf = NULL;
	char *signature="sqsh", *header = NULL;
	int signature_len;

	if (argc == 1)
		usage();

	while ( optind < argc ) {
		if (!strcmp("-i", argv[optind]) && optind < argc-1) {
			infilename = argv[++optind];
			optind++;
		} else if ( !strcmp("-o", argv[optind]) && optind < argc-1 ) {
			outfilename = argv[++optind];
			optind++;
		} else if ( !strcmp("-s", argv[optind]) && optind < argc-1 ) {
			signature = argv[++optind];
			optind++;
		} else {
			usage();
		};
	}

	signature_len = strlen(signature);
	//fprintf(stderr, "infile: '%s', outfile: '%s', signature: '%s', signature_len: %d\n", infilename, outfilename, signature, signature_len);

	if ( ! (infile = fopen(infilename, "r")) ) {
		perror(infilename);
		return 1;
	}
	fseek (infile , 0 , SEEK_END);
	fsize = ftell(infile);
	fprintf(stderr, "size: %d\n", fsize);
	big_buf = malloc(fsize+8);

	if (! big_buf) {
		perror("malloc");
		return 1;
	}

	memset(big_buf, 0, fsize);

	fseek (infile , 0 , SEEK_SET);
	if(errno) perror("fseek");
	fread(big_buf, fsize, 1, infile);
	if(errno) perror("fread");
	fclose(infile);

	header = memmem(big_buf, fsize, signature, signature_len);
	if ( header ) { 
		if ( ! (outfile = fopen(outfilename, "w")) ) {
			perror(outfilename);
			exit(1);
		}
		fwrite(header, fsize - (header-big_buf), 1, outfile);

		fclose(outfile);
	} else {
		fprintf(stderr, "signature not found\n");
		exit(1);
	}
	exit(0);
}
