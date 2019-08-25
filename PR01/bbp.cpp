#include <iostream>
#include <sstream>
#include <cstdlib>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>



std::vector<int> bubbleSort(std::vector<int> vector){
  int b = 0;
  while(!b)
  {
    b = 1;
    for(int i = 0; i < vector.size()-1; i++)
    {
      if(vector[i+1] < vector[i])
      {
        int t = vector[i];
        vector[i] = vector[i+1];
        vector[i+1] = t;
        b = 0;
      }
    }
  }
  return vector;
}

int* bucketSort(int* array, int k, int size){
    printf("int size is %d\n", sizeof(int));
    printf("l size is %d\n", sizeof(long));
    printf("ll size is %d\n", sizeof(long long unsigned int));
    std::vector<int> buckets[k];  //make k buckets

    printf("allocated vector\n");
    int i = 0;
    int max = -1;
    for(;i < size; i++) //find largest item in array to help determine each bucket's range
    {
      if(array[i] > max)
        max = array[i];
    }

    max++;

    printf("beginning distributing\n");

    #pragma omp parallel for
    for(i = 0; i < size; i++)
    {
      long long unsigned int index = k*((double)array[i]/max);
      buckets[index].push_back(array[i]); //insert item into appropriate bucket
    }

    printf("beginning with sorting\n");

    for(i = 0; i < k; i++){

      if(buckets[i].size() > 1){
        #pragma omp task shared(buckets)
        {
          buckets[i] = bubbleSort(buckets[i]);
        }
      }
    }

    printf("done with sorting\n");

    #pragma omp taskwait
    {
      int t = 0;
      for(i = 0; i < k; i++){
        for(int y = 0; y < buckets[i].size(); y++){

          array[t] = buckets[i][y];
          t++;
        }
      }

      return array;
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

/*
  for(int i = 0; i < size; i++)
  {
    printf("%d ", array[i]);
  }
*/
  //**************************/
  /**************************/
  /**************************/
  ///  YOUR CODE HERE !!!///
  /**************************/
  /**************************/
  /**************************/
  printf("\n");
  //free(array);
  int bsize = 0;
  if(size < 300000){
    bsize = size;
  }
  else{
    bsize = 300000;
  }
  #pragma omp parallel
  {
    #pragma omp single
    {
      array = bucketSort(array, bsize, size);
    }
  }
  printf("\n");

/*
  for(int i = 0; i < size; i++)
  {
    printf("%d ", array[i]);
  }
*/
  // delete the heap memory
  delete [] array;
}
