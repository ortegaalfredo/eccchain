#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <fstream>
#define NDEBUG
#define BOOST_UBLAS_NDEBUG
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix.hpp>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>


  boost::mt19937 rng;                 // produces randomness out of thin air
                                      // see pseudo-random number generators
  boost::uniform_int<> six(0,256*256*256);  // distribution that maps to 1..6
                                      // see random number distributions
  boost::variate_generator<boost::mt19937&, boost::uniform_int<> >
           irand(rng, six);             // glues randomness with mapping

using namespace boost::numeric::ublas ;


// Evencol algorithm:
// Modifies the number of ones on the columns to make them the same as the number of ones in the rows.

void evenCol(matrix<int> &Gt)	{
 int x=Gt.size1(); 
 int y=Gt.size2(); 
 int j,q,i;
//count ones per column
 int ones_per_column=0;
 for (q=0;q<y;q++)	
 	for (j=0;j<x;j++)
		ones_per_column+=Gt(q,j);
 ones_per_column/=y;
 int ones_this_column;
 for (q=0;q<y;q++)	{
	ones_this_column=0;
 	for (j=0;j<x;j++) ones_this_column+=Gt(q,j);
	int cont=0;
	// Mas de la media
	while (ones_this_column>ones_per_column)	{
 		for (j=0;j<x;j++) 
		//j = irand() % x;
		if (Gt(q,j))	{ //swap
				i=irand() % (q-y);
				i+=q;
				if (!Gt(i,j))	{
					Gt(i,j)=1;
					Gt(q,j)=0;
					}
			break;
			}
		ones_this_column=0;
 		for (j=0;j<x;j++) ones_this_column+=Gt(q,j);
		if (cont++>1000) break;
		//std::cout<<"cont: "<<cont<<" "<<ones_this_column<<std::endl;
		}
	// Menos de la media
	while (ones_this_column<ones_per_column)	{
		j = irand() % x;
		if (!Gt(q,j))	{ //swap
			i=irand() % (q-y);
			i+=q;
			if (Gt(i,j))	{
				Gt(i,j)=0;
				Gt(q,j)=1;
				}
			}
		ones_this_column=0;
 		for (j=0;j<x;j++) ones_this_column+=Gt(q,j);
		if (cont++>10000) break;
		//std::cout<<"cont: "<<cont<<" "<<ones_this_column<<std::endl;
		}

	} 
 
}
// Show ones on the matrix
void showOnes(matrix<int> &Gt)	{
 int x=Gt.size1(); 
 int y=Gt.size2(); 
 int j,q;
 
 // ones per row
 for (j=0;j<x;j++)
	{
	int index=0;
	for (q=0;q<y;q++)	{
		index+=Gt(q,j);
		}
	std::cerr<<index<<" ";
	} 
 std::cerr<<std::endl;

 // ones per column
 for (q=0;q<y;q++)	{
	int index=0;
 	for (j=0;j<x;j++){
		index+=Gt(q,j);
		}
	std::cerr<<index<<" ";
	} 
 std::cerr<<std::endl;
}

int main(int argc, char **argv)	
{
  // Get random seed
  std::ifstream rando("/dev/urandom");
  unsigned int r;
  rando>>r;
  rando.close();
  rng.seed(r);


  int i,j,q;
  int x =400;
  int y =100;
  int ones=5;
  if (argc>1) x = atoi(argv[1]);
  if (argc>2) y = atoi(argv[2]);
  if (argc>3) ones = atoi(argv[3]);
  if (argc==1)	{
	printf("Usage: %s <width> <height> <ones per row>\n",argv[0]);
	exit(-1);
	}
  

// Constructing H = (At|I(x-y))
 matrix<int> H(y+(x-y),x-y);

  // init matrix
 for(i=0;i<(int)H.size2();i++)
 	for(j=0;j<(int)H.size1();j++)
		H(j,i)=0;

 // I(x-y)
 for (i=y;i<y+(x-y);i++)
		H(i,i-y)=1;
 // At
 matrix<int> Gt(y,x-y);

  // init matrix
 for(i=0;i<(int)Gt.size2();i++)
 	for(j=0;j<(int)Gt.size1();j++)
		Gt(j,i)=0;

 
 // Place "ones" 1 per row
 for (j=0;j<(x-y);j++)
	for(q=0;q<ones;q++)	{
		int index=irand() % y;
		if (Gt(index,j)) q--;
		Gt(index,j)=1;
		}
 
 //showOnes(Gt);
 evenCol(Gt);
 //showOnes(Gt);
 // Copy At to H
 for (i=0;i<y;i++)
	for (j=0;j<(x-y);j++)
		H(i,j)=Gt(i,j);

 std::cout<<trans(H)<<std::endl;
 
  // Constructing G = (Iy|A)
  matrix<int> G(x,y); //generator

  // init matrix
 for(i=0;i<(int)G.size2();i++)
 	for(j=0;j<(int)G.size1();j++)
		G(j,i)=0;
 
  // Iy
 for(i=0;i<y;i++)
	G(i,i)=1;

 matrix<int> Gp = trans(Gt);
 for(i=0;i<y;i++)
	for (j=0;j<(x-y);j++)
		G(j+y,i)=Gp(j,i);
 std::cout<<trans(G)<<std::endl;

 // Check, R must be all 0
 /*
 matrix<int> R;
 q=0;
 R=prod(trans(G),H);
 for(i=0;i<(int)R.size1();i++)
 	for(j=0;j<(int)R.size2();j++) {
		R(i,j)%=2;
		q+=R(i,j);
		}
 std::cerr<<"Sum R: "<<q<<std::endl;
 */
}
