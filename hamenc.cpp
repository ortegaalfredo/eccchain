#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#define NDEBUG
#define BOOST_UBLAS_NDEBUG
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

#define DEBUG  0

using namespace boost::numeric::ublas ;



// Devuelve la cantidad de errores de paridad del vector cw segun la matriz de paridad H
int checkParity(vector<int> cw, matrix<int> H)	{
	unsigned int errors = 0,q;
   	vector<int> vc;
    	vc = prod(H,cw);
    	for (q=0;q<vc.size();q++) vc(q)=vc(q) % 2;
    	for (q=0;q<vc.size();q++) errors+=vc(q);
	return errors;
}

void usage(void)	{
	std::cerr<<"Usage: hamenc <matrix> (takes input from stdin and outputs to stdout)"<<std::endl;
	exit(1);
}

// LDPC simulator
  matrix<int> H; //parity check
  matrix<int> G;

void readMatrixes(char *file)	{
   std::ifstream thematrix;
   thematrix.open (file);
   // Cargamos H (Parity Check Matrix)
   thematrix>>H;
   // Cargamos G (Generator Matrix)
   thematrix>>G;
   thematrix.close();
   if (DEBUG)
   	std::cerr<<"Loaded "<<file<<std::endl;
}

// Returns the position of the error
int HammingFindError(vector<int> cw)	{

   unsigned int q,x,y;
   vector<int> vR(10);
	vR = prod(H,cw);
   
   for (q=0;q<vR.size();q++)
	vR(q)=vR(q) % 2;
  
   int count=0;
   for (q=0;q<vR.size();q++)
	count+=vR(q); 

   if (!count) return -1; // no errors

   // segun el algoritmo de hamming, ahora en vR tenemos los errores de
   // paridad, que equivalen a la columna de H donde esta el error,
   // si es que solamente hay un bit de error
   // Buscamos la columna de H:
   
   for (x=0;x<H.size2();x++)	{
	q=0;
	for(y=0;y<H.size1();y++)
		if (vR(y)!=H(y,x))
			q++;
	if (!q) // la columna es igual a vR
		return x;
	}
 
   return -2; // can't find the error
}

void test(void)	
{
   unsigned int dataLenght = G.size1();
   unsigned int codewordLenght = G.size2();
   vector<int> v(dataLenght),vR(dataLenght),T;
   vector<int> cw(codewordLenght);

   unsigned int i,q,epos,ecnt = 0;
   int error,parity,eparity;

  for (i=0;i<100000;i++)	{
	for (q=0;q<dataLenght;q++)	
		v(q)= rand() % 2;

    	cw = prod(v,G);
		
	//paridad
	parity=0;
	for (q=0;q<codewordLenght;q++)
		parity+=cw(q);
		
	epos = rand() % codewordLenght;
	cw(epos) = 1 - cw(epos);
	epos = rand() % codewordLenght;
	cw(epos) = 1 - cw(epos);

	error = HammingFindError(cw);

	if (error==-1) 
		{
		//cw(error)=1-cw(error);
		//eparity=0;
		//for (q=0;q<codewordLenght;q++)
		//	eparity+=cw(q);
		//if (eparity!=parity)	//error detected
			ecnt++;
		}

	}
  printf("error-detected: %d\n",ecnt);
  exit(0);
}

int main(int argc, char **argv)	
{
  int SEC_DED=0;

  srand(0);

  if (argc<2)	usage();
  readMatrixes(argv[1]);
  
  if (argc==3) SEC_DED=1;

   unsigned int dataLenght = G.size1();
   unsigned int codewordLenght = G.size2();

   vector<int> v(dataLenght),vR(dataLenght),T;
   vector<int> cw(codewordLenght);
   unsigned int q;
   
   fprintf(stderr,"\nHamming %d\n",H.size1());
    while(true)	{

	  if (strstr(argv[0],"hamenc"))
		{ // codifica hamming

		// lee vector v()
		char *rbuf = (char *)malloc(dataLenght+1);
		q=fread(rbuf,1,dataLenght,stdin);
		if (q!=dataLenght)
			exit(0); //no more data
	    	for (q=0;q<dataLenght;q++)	
			v(q)= (rbuf[q]-0x30);
		free(rbuf);
    	
		// CodeWord = v*G
	    	cw = prod(v,G);
		int parity=0;
	    	for (q=0;q<cw.size();q++) {
			cw(q)=cw(q) % 2; 
			parity+=cw(q);
			std::cout<<(cw(q)?'1':'0');
			}
		if (SEC_DED)	 // output additional parity bit
			std::cout<<((parity % 2)?'1':'0');
			
		}

	else	{// decodifica hamming

		// lee codeword cw()
		char *rbuf = (char *)malloc(codewordLenght+1);
		int parity;
		char parbit;
		int error;

		q=fread(rbuf,1,codewordLenght,stdin);
		if (q!=codewordLenght)
			exit(0); //no more data
	    	for (q=0;q<codewordLenght;q++)	{
			cw(q)= (rbuf[q]-0x30);
			}
		free(rbuf);
		parbit=0;
		if (SEC_DED) // read additional parity bit
			fread(&parbit,1,1,stdin);

		error = HammingFindError(cw);
		if (error>=0)	{ // error detected
				int bkp = cw(error);
				cw(error)=1-cw(error); // flip bit
				}

		if (error==-1)	fprintf(stderr,".");
		int parityBits = H.size1(); //number of parity bits
    		for (q=0;q<dataLenght;q++) {
			cw(q+parityBits)=cw(q+parityBits) % 2;
			std::cout<<(cw(q+parityBits)?'1':'0');
			}
		}
	}
}
