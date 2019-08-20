#include <iostream>
#include <sstream>
#include <cstdlib>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int fib(int n);

int serfib(int n){
  if (n < 2) return n;
  int x = fib(n-1);
  int y = fib(n-2);
  return x+y;
}

int main(int argc, char* argv[])
{

  #pragma omp parallel
  {
    #pragma omp single
    {
      fib(47);
    }
  }

}



int fib(int n)
{
  if (n < 2) return n;
  //if (n <= 30) return serfib(n);
  int x,y;
  #pragma omp task shared(x)
  {
    x = fib(n-1);
  }
  #pragma omp task shared(y)
  {
    y = fib(n-2);
  }
  #pragma omp taskwait
  {
    return x+y;
  }
}
