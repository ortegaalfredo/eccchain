#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAXBLOCKSIZE 100000

#include "MersenneTwister.h"
MTRand mtrand(123);
unsigned int irand(void)	{ return mtrand.randInt();}



int main(int argc, char **argv) 
{
int len,i;
unsigned int seed=0;
unsigned char buf[MAXBLOCKSIZE],c;
unsigned int perm[MAXBLOCKSIZE];

int blocksize=4096;

// Read lenght of input block (max MAXBLOCKSIZE)
if (argc>1)
	blocksize=atoi(argv[1]);

// Read seed
if (argc>2)
	seed =atoi(argv[2]);

// create permutations
for (i=0;i<MAXBLOCKSIZE;i++)
	perm[i]=irand() % blocksize;	

if (strstr(argv[0],"scrambler"))
	{ // scrambler
	//fprintf(stderr," scrambling block: %d\n",blocksize);
	while((len=fread(buf,1,blocksize,stdin))>0)	{
		for (i=0;i<MAXBLOCKSIZE;i++)	{
			// swap
			int newpos= (i + (perm[i]-(blocksize/2))) % blocksize ;
			c=buf[newpos];
			buf[newpos]=buf[i % blocksize];
			buf[i % blocksize]=c;
			}
		write(1,buf,blocksize);
		}
	}
else	{// descrambler
	while((len=fread(buf,1,blocksize,stdin))==blocksize)	{
		for (i=MAXBLOCKSIZE-1;i>=0;i--)	{
			// swap
			int newpos= (i + (perm[i]-(blocksize/2))) % blocksize ;
			c=buf[newpos];
			buf[newpos]=buf[i % blocksize];
			buf[i % blocksize]=c;
			}
		write(1,buf,blocksize);
		}
	}
return 0;
}
