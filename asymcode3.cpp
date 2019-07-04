#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
//#define NDEBUG
//#define BOOST_UBLAS_NDEBUG
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

#define DEBUG  0

using namespace boost::numeric::ublas ;

#define WIDTH 3

#define CODEWIDTH 2

int main(int argc, char **argv) 
{
vector<int> V1(WIDTH),V2(WIDTH);
unsigned int len,i,val;
char in[CODEWIDTH+1];
char out[WIDTH+1];

if (strstr(argv[0],"asymenc"))
	{ // codifica un canal
	while((len=fread(in,1,CODEWIDTH,stdin))==CODEWIDTH)	{
			val=0;
			// bits->val
			for (i=0,val=0;i<CODEWIDTH;i++)
				val+=(in[i]-0x30)<<i;
			switch(val)	{
				case 0: strcpy(out,"000");break;
				case 1: strcpy(out,"100");break;
				case 2: strcpy(out,"010");break;
				case 3: strcpy(out,"111");break;
				}

			if ((len=write(1,out,WIDTH)) !=WIDTH)
				fprintf(stderr,"Error: write\n");
		}
	}
else	{// decodifica un bloomfilter en un canal
	int total=0,ones=0;
	while (1)	{
			len=fread(out,1,WIDTH,stdin);
			if (len!=WIDTH) //exit 
				{
				fprintf(stderr,"relacion unos/ceros: %f\n",((float)ones)/((float)total));
				return 0;
				}
			// Buscamos cual codigo se parece mas buscando el hamming minimo
			// bits->val
			for (i=0,val=0;i<WIDTH;i++)
				val+=(out[i]-0x30)<<i;
			switch (val)	{
				case 0: strcpy(in,"00");break;
				case 1: strcpy(in,"10");break;
				case 2: strcpy(in,"01");break;
				case 3: strcpy(in,"01");break;
				case 4: strcpy(in,"00");break;
				case 5: strcpy(in,"10");break;
				case 6: strcpy(in,"01");break;
				case 7: strcpy(in,"11");break;
				}

			total+=WIDTH;

			if ((len=write(1,in,CODEWIDTH)) !=CODEWIDTH)
				fprintf(stderr,"Error: write\n");
		}
	}
return 0;
} 
