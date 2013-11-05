#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>

#include "gfx.h"

#define OUT_WIDTH (450)
#define SCROLL_STEP (30)
const char BGINDEX = 0;
char emptycolumn[GFX_HEIGHT];
const double framestep = 0.02;
const double columnstep = 0.01;

int main(int argc, char **argv) {
	char outfilename[] = "out####.data";
	int i, x, y;
	int gfx_start, gfx_this_width;
	int outfile;
	double columnstart = 0;
	double columnval;
	double yoffset[4];
	double swap;
	double finaly;
	int multiplier[2];
	int least, least2;
	unsigned char gfxval;

	memset(emptycolumn, BGINDEX, GFX_HEIGHT);
	for(i = 0; i < GFX_WIDTH + OUT_WIDTH; i+=SCROLL_STEP) {
		fprintf(stderr, "\r%04i/%04i", i / SCROLL_STEP, (GFX_WIDTH + OUT_WIDTH) / SCROLL_STEP);
		snprintf(outfilename, sizeof(outfilename), "out%04i.data", i / SCROLL_STEP);
		outfile = open(outfilename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if(outfile == -1) {
			fprintf(stderr, "\nFailed to open file %s (%s).\n", outfilename, strerror(errno));
			exit(EXIT_FAILURE);
		}

		gfx_start = OUT_WIDTH - i;

		columnstart = (double)i * framestep;
		for(x = 0; x < OUT_WIDTH; x++) { 
			columnval = (double)x * columnstep + columnstart;
			if(i < OUT_WIDTH && x < OUT_WIDTH - i) {
				write(outfile, &emptycolumn, GFX_HEIGHT);
			} else if (i > GFX_WIDTH && x > GFX_WIDTH + OUT_WIDTH - i ) {
				write(outfile, &emptycolumn, GFX_HEIGHT);
			} else {
				/* calculate the bend for this column */
				yoffset[0] = (sin(columnval + (M_PI * 0.0)) + 1.0) * (double)GFX_HEIGHT / 2.0; /*   0 degrees */
				yoffset[1] = (sin(columnval + (M_PI * 0.5)) + 1.0) * (double)GFX_HEIGHT / 2.0; /*  90 degrees */
				yoffset[2] = (sin(columnval + (M_PI * 1.0)) + 1.0) * (double)GFX_HEIGHT / 2.0; /* 180 degrees */
				yoffset[3] = (sin(columnval + (M_PI * 1.5)) + 1.0) * (double)GFX_HEIGHT / 2.0; /* 270 degrees */
				/* we only want the visible faces */
				least  = yoffset[0] < yoffset[1] ? 0 : 1;
				least2 = yoffset[2] < yoffset[3] ? 2 : 3;
				least  = yoffset[least] < yoffset[least2] ? least : least2;
				switch(least) {
					case 0:
						break;
					case 1:
						yoffset[0] = yoffset[1];
						yoffset[1] = yoffset[2];
						yoffset[2] = yoffset[3];
						break;
					case 2:
						swap = yoffset[0];
						yoffset[0] = yoffset[2];
						yoffset[1] = yoffset[3];
						yoffset[2] = swap;
						break;
					case 3:
						swap = yoffset[0];
						yoffset[0] = yoffset[3];
						yoffset[2] = yoffset[1];
						yoffset[1] = swap;
				}

				multiplier[0] = yoffset[1] - yoffset[0];
				multiplier[1] = yoffset[2] - yoffset[1];
				for(y = 0; y < GFX_HEIGHT; y++) {
					if(y < yoffset[0]) {
						write(outfile, &BGINDEX, 1);
					} else if(y < yoffset[1]) {
						finaly = (double)y - (double)(yoffset[0]); /* fix lookup bias */
						finaly = (GFX_HEIGHT / ((double)(yoffset[1]) - (double)(yoffset[0]))) * finaly;
						gfxval = GFX_XY(x - gfx_start, (int)finaly) * multiplier[0];
						if(gfxval != 0)
							gfxval += 71;
						else
							gfxval = multiplier[0];
						//gfxval = (char)finaly;
						write(outfile, &gfxval, 1);
					} else if(y < yoffset[2]) {
						finaly = (double)y - (double)(yoffset[1]); /* fix lookup bias */
						finaly = (GFX_HEIGHT / ((double)(yoffset[2]) - (double)(yoffset[1]))) * finaly;
						gfxval = GFX_XY(x - gfx_start, (int)finaly) * multiplier[1];
						if(gfxval != 0)
							gfxval += 71;
						else
							gfxval = multiplier[1];
						//gfxval = (char)finaly;
						write(outfile, &gfxval, 1);
					} else {
						write(outfile, &BGINDEX, 1);
					}
				}
			}
		}

		close(outfile);
	}

	fprintf(stderr, "\n");
	exit(EXIT_SUCCESS);
}
