#include <stdio.h>
#include <stdlib.h>

int cval2count(unsigned char cval) {
	switch(cval) {
		case 0xf0:
			return(2);
		case 0xf1:
			return(4);
		case 0xf2:
			return(8);
		case 0xf3:
			return(16);
		case 0xf4:
			return(32);
		case 0xf5:
			return(64);
		case 0xf6:
			return(128);
		case 0xf7:
			return(256);
	}
}

int main(int argc, char **argv) {
	FILE *infile;
	FILE *outfile;
	unsigned char cval, red, grn, blu;
	int count;
	int i;

	if(argc < 2) {
		fprintf(stderr, "USAGE: %s infile\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "Getting source palette.\n");
	infile = fopen(argv[1], "r");
	if(infile == NULL) {
		fprintf(stderr, "Failed to open %s.\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	fseek(infile, 10, SEEK_SET);
	fread(&cval, 1, 1, infile);
	count = cval2count(cval);
	fseek(infile, 13, SEEK_SET);
	for(i = 0; i < count; i++) {
		fread(&red, 1, 1, infile);
		fread(&grn, 1, 1, infile);
		fread(&blu, 1, 1, infile);
		printf("rgb(%i,%i,%i) rgb(%i,%i,%i)\n", i, i, i, red, grn, blu);
	}
	fclose(infile);

	exit(EXIT_SUCCESS);
}
