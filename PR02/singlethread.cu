#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <thrust/sort.h>
using namespace std;
/**********************************************************
***********************************************************
error checking stufff
***********************************************************
***********************************************************/

// Enable this for error checking
#define CUDA_CHECK_ERROR
#define CudaSafeCall( err ) __cudaSafeCall( err, __FILE__, __LINE__ )
#define CudaCheckError() __cudaCheckError( __FILE__, __LINE__ )



inline void __cudaSafeCall( cudaError err, const char *file, const int line )
{
	#ifdef CUDA_CHECK_ERROR
	#pragma warning( push )
	#pragma warning( disable: 4127 ) // Prevent warning on do-while(0);
	do
	{
		if ( cudaSuccess != err )
		{
			fprintf( stderr,"cudaSafeCall() failed at %s:%i : %s\n",file, line, cudaGetErrorString( err ) );
			exit( -1 );
		}
	} while ( 0 );
	#pragma warning( pop )
	#endif // CUDA_CHECK_ERROR
	return;
}


inline void __cudaCheckError( const char *file, const int line )
{
	#ifdef CUDA_CHECK_ERROR
	#pragma warning( push )
	#pragma warning( disable: 4127 ) // Prevent warning on do-while(0);
	do
	{
		cudaError_t err = cudaGetLastError();
		if ( cudaSuccess != err )
		{
			fprintf( stderr,"cudaCheckError() failed at %s:%i : %s.\n",file, line, cudaGetErrorString( err ) );
			exit( -1 );
		}
		// More careful checking. However, this will affect performance.
		// Comment if not needed.
		err = cudaThreadSynchronize();
		if( cudaSuccess != err )
		{
			fprintf( stderr,"cudaCheckError() with sync failed at %s:%i : %s.\n",file, line, cudaGetErrorString( err ) );
			exit( -1 );
		}
	} while ( 0 );
	#pragma warning( pop )
	#endif // CUDA_CHECK_ERROR
	return;
}




/***************************************************************
***************************************************************
end of error checking stuff
****************************************************************
***************************************************************/





// function takes an array pointer, and the number of rows and cols in the array, and
// allocates and intializes the array to a bunch of random numbers
// Note that this function creates a 1D array that is a flattened 2D array
// to access data item data[i][j], you must can use data[(i*rows) + j]
int * makeRandArray( const int size, const int seed )
{
	srand( seed );
	int * array = new int[ size ];
	for( int i = 0; i < size; i ++ ) {
		array[i] = std::rand() % 1000000;
	}
	return array;
}




//*******************************//
// In place radix sort           //
//*******************************//
__global__ void matavgKernel(int* array, const int S, int *out)
{

	const int size = S;
	int max = array[0];

	//find largest item in array so we know how many digits there are
	for(int i = 1; i < size; i++)
	{
		if(array[i] > max)
			max = array[i];
	}


	//go through each digit
	for(int exp = 1; max/exp > 0; exp *= 10){
		int *output = (int *)malloc(sizeof(int)*size);

		int i = 0;
		
		//array of size ten, for each possible value at this digit		
		int *count = (int *)malloc(sizeof(int)*10);

		//get the count of how many items have each possible value at this digit
		//(ie number of "2"s, "3"s, etc.)
		//this is the part that will be parallelized for the multithreading version
		for(i = 0; i < size; i++){
			count[ (array[i]/exp)%10 ]++;
		}

		//aggregate previous values into count, getting a count of values that size and smaller at digit exp.
		for(i = 1; i < 10; i++){
			count[i] += count[i - 1];
		}

		//based off known number of items in each location, copy things into output sorted by index exp
		for(i = size-1; i >= 0; i--){
			output[count[ (array[i]/exp)%10 ] - 1] = array[i];
			count[ (array[i]/exp)%10 ]--;
		}

		//copy back into original array so ordering of this digit is preserved for future iterations
		for(i = 0; i < size; i++){
			array[i] = output[i];
			
		}
	}
}






int main( int argc, char * argv[] )
{
	int * array; // the poitner to the array of rands
	int size, seed; // values for the size of the array
	bool printSorted = false;
	// and the seed for generating
	// random numbers
	// check the command line args
	if( argc < 4 ){
		std::cerr << "usage: " << argv[0] << " [amount of random nums to generate] [seed value for rand]" << " [1 to print sorted array, 0 otherwise]" << std::endl;
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

	{
		int sortPrint;
		std::stringstream ss1( argv[3] );
		ss1 >> sortPrint;
		if( sortPrint == 1 )
			printSorted = true;
	}

	// get the random numbers
	array = makeRandArray( size, seed );


	//print out initial array
	//for(int index = 0; index < size-1; index++)
	//{
	//	printf("%d, ", array[index]);
	//}
	//printf("%d\n", array[size-1]);

	/***********************************
	create a cuda timer to time execution
	*
	**********************************/

	cudaEvent_t startTotal, stopTotal;
	float timeTotal;
	cudaEventCreate(&startTotal);
	cudaEventCreate(&stopTotal);
	cudaEventRecord( startTotal, 0 );

	/***********************************
	end of cuda timer creation
	*
	**********************************/



	/////////////////////////////////////////////////////////////////////
	///////////////////////    YOUR CODE HERE     ///////////////////////
	/////////////////////////////////////////////////////////////////////

	//STEP 1
	int *device_nums;
	cudaMalloc( &device_nums, size * sizeof(int) );	//this will be the array on the GPU

	//STEP 2
	// copy our array into our array on GPU
	cudaMemcpy(device_nums, &(array[0]), size * sizeof(int), cudaMemcpyHostToDevice);


	//STEP 3
	int* device_count;
	cudaMalloc( &device_count, sizeof(int) );
	{
	  int startCount = 0;
	  cudaMemcpy(device_count, &(startCount), sizeof(int), cudaMemcpyHostToDevice);
	}


	//STEP 4
	//dim3 threadsPerBlock(1, 1);
	//dim3 numBlocks((size +threadsPerBlock.x-1) / threadsPerBlock.x, (size +threadsPerBlock.y-1) / threadsPerBlock.y);


	//STEP 5
	matavgKernel<<< 1, 1>>> ( device_nums, size, device_count );


	int totalFound;
	cudaMemcpy( array, device_nums, size*sizeof(int), cudaMemcpyDeviceToHost );

	






	/*
	You need to implement your kernel as a function at the top of this file.
	Here you must
	1) allocate device memory
	2) set up the grid and block sizes
	3) call your kenrnel
	4) get the result back from the GPU
	to use the error checking code, wrap any cudamalloc functions as follows:
	CudaSafeCall( cudaMalloc( &pointer_to_a_device_pointer,
	length_of_array * sizeof( int ) ) );
	Also, place the following function call immediately after you call your kernel
	( or after any other cuda call that you think might be causing an error )
	CudaCheckError();
	*/



	/***********************************
	Stop and destroy the cuda timer
	*
	**********************************/

	cudaEventRecord( stopTotal, 0 );
	cudaEventSynchronize( stopTotal );
	cudaEventElapsedTime( &timeTotal, startTotal, stopTotal );
	cudaEventDestroy( startTotal );
	cudaEventDestroy( stopTotal );

	/***********************************
	end of cuda timer destruction
	*
	**********************************/



	std::cerr << "Total time in seconds: "	<< timeTotal / 1000.0 << std::endl;


	if( printSorted ){
		//print out sorted array
		for(int index = 0; index < size-1; index++)
		{
			printf("%d, ", array[index]);
	        }
		printf("%d\n", array[size-1]);

	}
}
