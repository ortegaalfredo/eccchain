#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "reedsolomon.h"

int main(int argc, char **argv) 
{
int q,len;
unsigned char raw;
unsigned char bin[8];
if (strstr(argv[0],"raw2bin"))
	{ // convierte entrada raw en salida en bits
	while((len=fread(&raw,1,1,stdin))==1)	{
        	for(q=0;q<8;q++)
                	bin[q]=raw&(1<<q)?'1':'0';
		write(1,&bin,8);
		}
	}
if (strstr(argv[0],"bin2raw"))
	{// convierte entrada en bits en salida raw
	while((len=fread(bin,1,8,stdin))==8)	{
        	raw=0;
        	for(q=0;q<8;q++)
                	raw |= (bin[q]-0x30)?(1<<q):0;
		write(1,&raw,1);
		}
	}
if (strstr(argv[0],"bin2bin"))
	{ // introduce error asimetrico
	float rnd,err = atof(argv[1]);
	while((len=fread(&raw,1,1,stdin))==1)	{
		rnd = rand();
		rnd /= (float)RAND_MAX;
		if (rnd<err) raw='1'; 
		write(1,&raw,1);
		}
	}
return 0;
}
