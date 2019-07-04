#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAXBLOCKSIZE 65536

int main(int argc, char **argv) 
{
int len1,len2,i,diff=0;
unsigned char buf1[MAXBLOCKSIZE],buf2[MAXBLOCKSIZE];
FILE *file1,*file2;

if (argc<3)	{
	printf("Usage: %s file1 file2 [-d]\n",argv[0]);
	exit(1);
	}

float errors=0,count=0;

file1=fopen(argv[1],"r");
file2=fopen(argv[2],"r");

if (argc==4)
	if (strcmp(argv[3],"-d")==0)
		diff=1;//activate diff mode

if (strstr(argv[0],"ber"))
	{ // ber of bits
	int b;
	while((len1=fread(buf1,1,MAXBLOCKSIZE,file1))>0)	{
		len2=fread(buf2,1,MAXBLOCKSIZE,file2);
		if (len2<len1) len1=len2;
		for (i=0;i<len1;i++)	{
			count++;
			for (b=0;b<8;b++)
				if ((buf1[i] & (1<<b)) != (buf2[i] & (1<<b))) {
					errors++;
					if (diff) putc('@',stdout);
					}
				else	if (diff) putc('.',stdout); 
			}
		}
	printf("%f (%.0f/%.0f)\n",(float)errors/(count*8.0),errors,count*8);
	}
return 0;
}
