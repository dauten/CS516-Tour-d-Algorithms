#include <iostream>
#include <sstream>
#include <cstdlib>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>

int* merge(int* left, int* right, int rl, int ll){
  int* result = new int[rl + ll];

  int r = 0;
  int l = 0;
  for(int i = 0; i < rl+ll; i++){
    if((left[l] < right[r] && l < ll) || r >= rl)
    {
      result[i] = left[l];
      l++;
    }
    else
    {
      result[i] = right[r];
      r++;
    }
  }

  return result;
}

int* mergeSort(int* array, int length){

  if(length <= 1){
    return array;
  }

  int* left = new int[length/2];
  int* right = new int[length/2 + length%2];

  for(int i = 0; i < length; i++){
    if(i < length/2)
      left[i] = array[i];
    else
      right[i-(length/2)] = array[i];
  }

  left = mergeSort(left, length/2);
  right = mergeSort(right, length/2 + length%2);

  return merge(left, right, length/2 + length%2, length/2);
}

// create an array of length size of random numbers// returns a pointer to the array3
// seed: seeds the random number generator
int* randNumArray(const int size,const int seed )
{
  srand( seed );
  int* array = new int[ size ];
  for(int i = 0; i < size; i ++ )
  {
    array[i] = std::rand() % 1000000;
  }
    return array;
  }

int main(int argc, char** argv )
{
  int* array;
  // the poitner to the array of rands
  int size, seed; // values for the size of the array
  // and the seed for generating
  // random numbers
  // check the command line args
  if( argc < 3 )
  {
    std::cerr <<"usage: "<< argv[0]<<" [amount of random nums to generate] [seed value for rand]"<< std::endl;
    exit( -1 );
  }
  // convert cstrings to ints
  {
    std::stringstream ss1( argv[1] );
    ss1 >> size;
  }
  {
    std::stringstream ss1( argv[2] );
    ss1 >> seed;
  }

  // get the random numbers
  array = randNumArray( size, seed );

  printf("Array to sort is:\n");

  /*
  for(int i = 0; i < size; i++)
  {
    printf("%d ", array[i]);
  }
  */

  printf("Beginning sort, and timing.\n");

  auto startTime = std::chrono::high_resolution_clock::now();


  array = mergeSort(array, size);

  auto endTime = std::chrono::high_resolution_clock::now();


  printf("Timing complete.  Printing Sorted Results.\n");

  /*
  for(int i = 0; i < size; i++)
  {
    printf("%d ", array[i]);
  }
  */

  printf("\nTotal execution time is: %d ms\n", (endTime-startTime)/1000000);




  // delete the heap memory
  delete [] array;
}
