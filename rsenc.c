#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "reedsolomon.h"

int main(int argc, char **argv) 
{
int correrr=0,toterr=0,len,rounds=0,i;
unsigned char buf[256];
int maxdata=223;

// Read lenght of input block (max 223)
if (argc==2)	
	maxdata=atoi(argv[1]);
	

if (strstr(argv[0],"rsenc"))
	{ // codifica reed-solomon gf(8)
	while((len=fread(buf,1,maxdata,stdin))>0)	{
		memset(&buf[maxdata],0,256-maxdata);
		encode_rs_8_c(buf,&buf[223],0);
		write(1,buf,maxdata);
		write(1,&buf[223],32);
		}
	}
else	{// decodifica reed-solomon gf(8)
	while((len=fread(buf,1,maxdata+32,stdin))==maxdata+32)	{
		for(i=0;i<33;i++)	
			buf[255-i]=buf[maxdata+32-i];
		memset(&buf[maxdata],0,223-maxdata);

		i=decode_rs_8(buf,NULL,0,0);
		if (i<0) toterr+=16;
		else { toterr+=i;correrr+=i;}
		write(1,buf,maxdata);
		rounds++;
		}
	fprintf(stderr,"reed-solomon decode: Errors detected: %d corrected: %d (Max. %d)\n",toterr,correrr,rounds*16);
	}
return 0;
}
