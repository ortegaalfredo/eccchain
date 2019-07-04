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


void usage(void)	{
	std::cerr<<"Usage: genDecoder <matrix> (outputs to stdout)"<<std::endl;
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
  int i,q,j;

  if (argc<2)	usage();
  readMatrixes(argv[1]);

    int numVnodes=H.size2(); // == dataLength
    int numFnodes=H.size1();
    H=trans(H);

    char *HI = (char *)calloc(sizeof(char),numVnodes*numFnodes);

    // converting to a sane array representation
    for(i=0;i<numVnodes;i++) 
	for(q=0;q<numFnodes;q++)
		HI[i*numFnodes+q]=H(i,q);

  std::cout<<"int nzeros = 0,nones=0;"<<std::endl;
  std::cout<<"int parityerrors=0,parity=0;"<<std::endl;
  std::cout<<"for (int r=0;r<MAXROUNDS;r++) {"<<std::endl;
  std::cout<<"memcpy(vnodeI,cwI,numVnodes);"<<std::endl;

  	for(i=0;i<numVnodes;i++) {
		std::cout<<"if (cwI["<<i<<"]!=0) { // assymetric"<<std::endl;
		std::cout<<"nzeros = 0;nones=0;"<<std::endl;
	
		std::cout<<"if (vnodeI["<<i<<"]) nones=1; else nzeros=1; // count the original value"<<std::endl;
		for(q=0;q<numFnodes;q++) 
				if (HI[i*numFnodes+q]) {// Link between f-node and v-node
					std::cout<<"if (responseI["<<(i*numFnodes+q)<<"]) nones++;"<<std::endl;
					std::cout<<"else nzeros++;"<<std::endl;
					}
			//decision, is one or zero?
		std::cout<<"if (nzeros>nones) vnodeI["<<i<<"]=0;"<<std::endl;
		std::cout<<"	else	if (nzeros<nones) vnodeI["<<i<<"]=1;"<<std::endl;
		std::cout<<"	else	if (r>3) vnodeI[i]=1-vnodeI["<<i<<"];"<<std::endl;
		std::cout<<" }"<<std::endl;
		}
	std::cout<<"parityerrors=0;parity=0;"<<std::endl;
	for (int f=0;f<numFnodes;f++) {
		for(i=0;i<numVnodes;i++)	{
			if (  HI[i*numFnodes+f]  ) {// Link between f-node and v-node
				std::cout<<"parity=0;"<<std::endl;
				for (int j=0;j<numVnodes;j++)	{
					if (j==i) continue; //ignore current vnode
					if (HI[j*numFnodes+f]) // Link between f-node and v-node
						std::cout<<"parity+=vnodeI["<<j<<"];"<<std::endl;
					}
				std::cout<<"parity%=2; // Response is the value that we "<<std::endl;
				std::cout<<"	  // need for the sum to be zero"<<std::endl;
				std::cout<<"responseI["<<(i*numFnodes+f)<<"]=parity;"<<std::endl;
				std::cout<<"if (cwI["<<i<<"]==0) responseI["<<(i*numFnodes+f)<<"]=0; //assymetric"<<std::endl;
				std::cout<<"if (responseI["<<(i*numFnodes+f)<<"] != vnodeI["<<i<<"]) parityerrors++;"<<std::endl;
				}
			}
		}
	std::cout<<"if (!parityerrors) break; // all parity checks OK"<<std::endl;
	std::cout<<"}"<<std::endl;

}
