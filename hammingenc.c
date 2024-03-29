#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void usage(void)	{
	printf("Usage: hammingenc (takes input from stdin and outputs to stdout)\n");
	exit(1);
}
#define DATA_VALUES 16
#define CODE_VALUES 128
/* Tables from hamming library of Michael Dipperstein */
/* table of Hamming codes hammingCodes[x] is the x encoded */
const unsigned char hammingCodes[DATA_VALUES] =
{
    0x00,   /* 0 */
    0x71,   /* 1 */
    0x62,   /* 2 */
    0x13,   /* 3 */
    0x54,   /* 4 */
    0x25,   /* 5 */
    0x36,   /* 6 */
    0x47,   /* 7 */
    0x38,   /* 8 */
    0x49,   /* 9 */
    0x5A,   /* A */
    0x2B,   /* B */
    0x6C,   /* C */
    0x1D,   /* D */
    0x0E,   /* E */
    0x7F    /* F */
};
/* table convering encoded value (with error) to original data */
/* hammingDecodeValues[code] = original data */
const unsigned char hammingDecodeValues[CODE_VALUES] =
{
    0x00, 0x00, 0x00, 0x03, 0x00, 0x05, 0x0E, 0x07,     /* 0x00 to 0x07 */
    0x00, 0x09, 0x0E, 0x0B, 0x0E, 0x0D, 0x0E, 0x0E,     /* 0x08 to 0x0F */
    0x00, 0x03, 0x03, 0x03, 0x04, 0x0D, 0x06, 0x03,     /* 0x10 to 0x17 */
    0x08, 0x0D, 0x0A, 0x03, 0x0D, 0x0D, 0x0E, 0x0D,     /* 0x18 to 0x1F */
    0x00, 0x05, 0x02, 0x0B, 0x05, 0x05, 0x06, 0x05,     /* 0x20 to 0x27 */
    0x08, 0x0B, 0x0B, 0x0B, 0x0C, 0x05, 0x0E, 0x0B,     /* 0x28 to 0x2F */
    0x08, 0x01, 0x06, 0x03, 0x06, 0x05, 0x06, 0x06,     /* 0x30 to 0x37 */
    0x08, 0x08, 0x08, 0x0B, 0x08, 0x0D, 0x06, 0x0F,     /* 0x38 to 0x3F */
    0x00, 0x09, 0x02, 0x07, 0x04, 0x07, 0x07, 0x07,     /* 0x40 to 0x47 */
    0x09, 0x09, 0x0A, 0x09, 0x0C, 0x09, 0x0E, 0x07,     /* 0x48 to 0x4F */
    0x04, 0x01, 0x0A, 0x03, 0x04, 0x04, 0x04, 0x07,     /* 0x50 to 0x57 */
    0x0A, 0x09, 0x0A, 0x0A, 0x04, 0x0D, 0x0A, 0x0F,     /* 0x58 to 0x5F */
    0x02, 0x01, 0x02, 0x02, 0x0C, 0x05, 0x02, 0x07,     /* 0x60 to 0x67 */
    0x0C, 0x09, 0x02, 0x0B, 0x0C, 0x0C, 0x0C, 0x0F,     /* 0x68 to 0x6F */
    0x01, 0x01, 0x02, 0x01, 0x04, 0x01, 0x06, 0x0F,     /* 0x70 to 0x77 */
    0x08, 0x01, 0x0A, 0x0F, 0x0C, 0x0F, 0x0F, 0x0F      /* 0x78 to 0x7F */
};

int main(int argc, char **argv) 
{
int len,i,inval,outval;
char in[4];
char out[7];

if (argc>1)	usage();

if (strstr(argv[0],"hammingenc"))
	{ // codifica 4 bits en 7 hamming bits
	while((len=fread(&in,1,4,stdin))==4)	{

		/* encode input bits */
		inval=0;
		for (i=0;i<4;i++)
			if (in[i]=='1') inval+=1<<i;
		/* hamming */
		outval=hammingCodes[inval];
		/* encode output bits */
		for (i=0;i<7;i++)
			if (outval & (1<<i)) out[i]='1';
			else out[i]='0';

		if ((len=write(1,&out,7)) !=7)
			fprintf(stderr,"Error: write\n");
		}
	}
else	{// decodifica un bloomfilter en un canal
	while((len=fread(out,1,7,stdin))==7)	{

		/* encode input bits */
		outval=0;
		for (i=0;i<7;i++)
			if (out[i]=='1') outval+=1<<i;
		/* hamming */
		inval= hammingDecodeValues[outval];
		/* encode output bits */
		for (i=0;i<4;i++)
			if (inval & (1<<i)) in[i]='1';
			else in[i]='0';

		if ((len=write(1,&in,4)) !=4)
			fprintf(stderr,"Error: write\n");
		}
	}
return 0;
}
