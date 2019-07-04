// Codificador/decodificador LDPC basado en matrices boost:ublas

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


/* prng usando rand() */
void lfsr(unsigned short int *reg)	{
	*reg=rand();
	}



// Devuelve la cantidad de errores de paridad del vector cw segun la matriz de paridad H
int checkParity(vector<int> cw, matrix<int> H)	{
	unsigned int errors = 0,q;
   	vector<int> vc;
    	vc = prod(H,cw);
    	for (q=0;q<vc.size();q++) vc(q)=vc(q) % 2;
    	for (q=0;q<vc.size();q++) errors+=vc(q);
	return errors;
}

// LDPC decode
// cw: received codeword 
// H: parity-check matrix
//
vector<int> decode(vector<int> cw, matrix<int> H) {
    int i,q;
    int numVnodes=H.size2(); // == dataLength
    int numFnodes=H.size1();
    
    vector<int> vnode(numVnodes);
    matrix<int> response(numVnodes,numFnodes); //response matrix
    int MAXROUNDS=10;
    H=trans(H);

    char *vnodeI = (char *)calloc(sizeof(char),numVnodes);
    char *cwI = (char *)calloc(sizeof(char),numVnodes);
    char *responseI = (char *)calloc(sizeof(char),numVnodes*numFnodes);
    char *HI = (char *)calloc(sizeof(char),numVnodes*numFnodes);
    

    // converting to a sane array representation
    for(i=0;i<numVnodes;i++) {
	cwI[i]=cw[i];
	for(q=0;q<numFnodes;q++)
		HI[i*numFnodes+q]=H(i,q);
	}

//----------- CUT ---------------
#include "decodeGen.h"
/*
    for (int r=0;r<MAXROUNDS;r++) {
	for(i=0;i<numVnodes;i++) {
		if (!r) vnodeI[i]=cwI[i]; //first round
		else	{ // sum
			if (cwI[i]==0) continue; // assymetric
			int nzeros = 0,nones=0;
			if (vnodeI[i]) nones=1; else nzeros=1; // count the original value
			for(q=0;q<numFnodes;q++)
				if (HI[i*numFnodes+q]) {// Link between f-node and v-node
					if (responseI[i*numFnodes+q]) nones++;
					else nzeros++;
					}
			//decision, is one or zero?
			//if (DEBUG>5) std::cerr<<" 0:"<<nzeros<<" 1:"<<nones<<"   ";
			if (nzeros>nones) vnodeI[i]=0;
			else	if (nzeros<nones) vnodeI[i]=1;
			else	if (r>3) vnodeI[i]=1-vnodeI[i];
			//if (!checkParity(vnode,trans(H))) break; // all parity checks OK
			}
		}
	//if (DEBUG>5) std::cerr<<std::endl;
	int parityerrors=0,parity=0;
	for (int f=0;f<numFnodes;f++) {
		for(i=0;i<numVnodes;i++)	{
			if (  HI[i*numFnodes+f]  ) {// Link between f-node and v-node
				parity=0;
				for (int j=0;j<numVnodes;j++)	{
					if (j==i) continue; //ignore current vnode
					if (HI[j*numFnodes+f]) // Link between f-node and v-node
						parity+=vnodeI[j];
					}
				parity%=2; // Response is the value that we 
					  // need for the sum to be zero
				responseI[i*numFnodes+f]=parity;
				if (cwI[i]==0) responseI[i*numFnodes+f]=0; //assymetric
				if (responseI[i*numFnodes+f] != vnodeI[i]) parityerrors++;
				}
			}
		}
        //if (DEBUG>1) std::cerr<<"Round: "<<r<<" vnodes: "<<vnode<<" parityerrors: "<<parityerrors<<std::endl;
	if (!parityerrors) break; // all parity checks OK
	}
*/
//----------- CUT ---------------

    // converting back to uBlas
    for(i=0;i<numVnodes;i++) 
	vnode[i]=vnodeI[i];
    free (vnodeI);
    free (cwI);
    free (responseI);
    free (HI);
return vnode;
};

void usage(void)	{
	std::cerr<<"Usage: ldpcenc <matrix> (takes input from stdin and outputs to stdout)"<<std::endl;
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

int main(int argc, char **argv)	
{
  srand(0);

  if (argc<2)	usage();
  readMatrixes(argv[1]);

   unsigned int dataLenght = G.size1();
   unsigned int codewordLenght = G.size2();

   vector<int> v(dataLenght);
   vector<int> cw(codewordLenght);
   matrix<int> Gt = trans(G);
   unsigned int q;
   
    while(true)	{

	  if (strstr(argv[0],"ldpcenc"))
		{ // codifica ldpc

		// lee vector v()
		char *rbuf = (char *)malloc(dataLenght+1);
		q=fread(rbuf,1,dataLenght,stdin);
		if (q!=dataLenght)
			exit(0); //no more data
	    	for (q=0;q<dataLenght;q++)	
			v(q)= (rbuf[q]-0x30);
		free(rbuf);
    	
		// CodeWord = Gt*v
	    	cw = prod(Gt,v);
	    	for (q=0;q<cw.size();q++) {
			cw(q)=cw(q) % 2; 
			std::cout<<(cw(q)?'1':'0');
			}
		}

	else	{// decodifica ldpc

		// lee codeword cw()
		char *rbuf = (char *)malloc(codewordLenght+1);
		q=fread(rbuf,1,codewordLenght,stdin);
		if (q!=codewordLenght)
			exit(0); //no more data
	    	for (q=0;q<codewordLenght;q++)	
			cw(q)= (rbuf[q]-0x30);
		free(rbuf);

    		vector<int> deco = decode(cw,H);
    		for (q=0;q<dataLenght;q++) {
			deco(q)=deco(q) % 2;
			std::cout<<(deco(q)?'1':'0');
			}
		}

	}
}
