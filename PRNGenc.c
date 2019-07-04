#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>


  boost::mt19937 rng;                 // produces randomness out of thin air
                                      // see pseudo-random number generators
  boost::uniform_int<> six(0,256*256*256);  // distribution that maps to 1..6
                                      // see random number distributions
 // boost::variate_generator<boost::mt19937&, boost::uniform_int<> >
 //          irand(rng, six);             // glues randomness with mapping

	#define irand rand


        #define M 256
        #define K 1
        #define CLIENTS M
	#define BITCOUNT 8
        #define DEBUG 0


//boost::mt19937 *PRNG;                 // produces randomness out of thin air
boost::mt19937 PRNG[1<<BITCOUNT][K];                 // produces randomness out of thin air
unsigned int lfsr(unsigned data,unsigned client,unsigned k)      {
	// client is unused
	return PRNG[data][k]();
        }

void bloomenc(unsigned int data,unsigned char *bloomFilter, unsigned clients)
	{
        unsigned int i,q,k;
	unsigned int bitcount = (1<<BITCOUNT);
        unsigned int clientData[CLIENTS]; // Datos a transmitir

                /* Inicializamos BloomFilter */
		memset(bloomFilter,'0',M);

                /* Generamos datos random */
                for(i=0;i<clients;i++)
			for (q=0;q<bitcount;q++)
                        	clientData[i]=irand() & (bitcount-1);

                /* El cliente 0 tiene los datos reales */
                	clientData[0]=data;
                /* Simulamos transmision */
                for(i=0;i<clients;i++)
                        for (k=0;k<K;k++)       {
                        unsigned int pos = 0;

                        /*Averiguamos posicion en la que tenemos que marcar */
			for (q=0;q<bitcount;q++)	{ 
				// siempre tenemos que avanzar todos los prngs
				int rnd;
				if (!i)
					rnd = lfsr(q,i,k)  & (M-1);
				else	rnd = irand() & (M-1);
				if (q==clientData[i])
	                        	pos = rnd;
				}

                        /* Marcamos la posicion que corresponda */
        	        bloomFilter[ pos ]='1';
                        }
                if (DEBUG)
                        for(i=0;i<sizeof(bloomFilter);i++)
                                printf("bloomFilter[%d]=%d\n",i,bloomFilter[i]);
	}

void bloomdec(unsigned char *data,unsigned char *bloomFilter, unsigned clients)
        {
        unsigned int i,q,k;
	unsigned int bitcount = 1<<BITCOUNT;

                /* Generamos datos random para balancear */
                for(i=0;i<clients;i++)
			for (q=0;q<bitcount;q++)
                        	irand();


                /* Ahora en el bloomfilter tenemos todos los datos, tratamos de descifrarlos: */
                        /* Caso por defecto: todos a cero */
			for (q=0;q<bitcount;q++)
                        	data[q]=0;
                for(i=0;i<clients;i++)  {
                        for (k=0;k<K;k++)       {
				for (q=0;q<bitcount;q++)	{
					unsigned int pos;
	                                /* La posicion es la misma que la anterior */
					if (!i)
        	                		pos = lfsr(q,i,k) & (M-1);
					else	pos = irand() & (M-1);
					if (i==0) {// Cliente cero es nuestro cliente
						data[q]=bloomFilter[pos]-0x30;
						}
                                	}
                        }
	}
	}

void usage(void)	{
	printf("Usage: bfenc <clients> (takes input from stdin and outputs to stdout)\n");
	exit(1);
}


int main(int argc, char **argv) 
{
int len,q,i,k;
unsigned char in[M];
unsigned char rin[M];
unsigned char out[M];
unsigned int bytein;

/* generate PRNGs */
for (i=0;i<1<<BITCOUNT;i++)
	for (k=0;k<K;k++)
		PRNG[i][k].seed((unsigned)i);

int clients=128;

srand(0);

if (argc<2)	usage();

clients=atoi(argv[1]);

fprintf(stderr,"bloomfilter: %d clients \n",clients);

if (strstr(argv[0],"PRNGenc"))
	{ // codifica un canal en un bloomfilter
	while((len=fread(in,1,BITCOUNT,stdin))==BITCOUNT)	{
		bytein=0;
		for (q=0;q<BITCOUNT;q++)	
			bytein+=(in[q]-0x30)<<q;
		for (q=0;q<BITCOUNT+1;q++)	{
			bloomenc(bytein,out,clients);
			if ((len=write(1,out,M)) !=M)
				fprintf(stderr,"Error: write\n");
			}
		}
	}
else	{// decodifica un bloomfilter en un canal
	while (1)	{
		for (q=0;q<M;q++)	
			in[q]='1';
		for (q=0;q<BITCOUNT+1;q++)	{
			for (i=0;i<M;i++)	
				rin[i]=0;
			len=fread(out,1,M,stdin);
			if (len!=M) { 
				return 0;
				}
			bloomdec(rin,out,clients);
			for (i=0;i<M;i++)
				if (!rin[i]) in[i]='0';
			}

		bytein=0;
		for (q=0;q<M;q++)
			if (in[q]=='1') bytein=q;	
		for (q=0;q<BITCOUNT;q++)	
			if (bytein & (1<<q)) in[q]='1'; else in[q]='0';
		if ((len=write(1,in,BITCOUNT)) !=BITCOUNT)
			fprintf(stderr,"Error: write\n");
		}
	}
return 0;
}
