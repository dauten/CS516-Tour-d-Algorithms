#include <iostream>
#include <sstream>
#include <cstdlib>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>

int partition(int* array, int low, int high){
    int pivot = array[high]; //set last item as pivot
    int i = low;
    int t = 0;
    int j;
    for(j = low; j <= high - 1; j++){
      if(array[j] < pivot){
        t = array[i];
        array[i] = array[j];
        array[j] = t;
        i++;
      }
    }
    t = array[i];
    array[i] = array[j];
    array[j] = t;
    return i;
}

void quickSort(int* array, int low, int high){

  if( low < high )
  {
    int p = partition(array, low, high);
    if(high > 1000)
    {
      #pragma omp task shared(low)
      {
        quickSort(array, low, p-1);
      }
      #pragma omp task shared(high)
      {
        quickSort(array, p+1, high);
      }
    }
    else{
      quickSort(array, low, p-1);
      quickSort(array, p+1, high);
    }
  }
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

  #pragma omp parallel
  {
    #pragma omp single
    {
      quickSort(array, 0, size-1);
    }
  }

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
