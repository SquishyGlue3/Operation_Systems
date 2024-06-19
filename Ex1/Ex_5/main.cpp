//using profiling to solve max sub array problem
#include <iostream>
#include "profiling.h"
using namespace std;

int main (int argc , char *argv[])//recieving 2 arguments, first will be the random seed of rand and the other will be the size of the array
{
    if(argc != 3)
    {
        cout << "Error, please enter the random seed and the size of the array" << endl;
        return 1;
    }
    int seed = atoi(argv[1]);//converting the first argument to integer
    srand(seed);//setting the seed of rand
    int size = atoi(argv[2]);//converting the second argument to integer
    int *arr = new int[size];//creating an array of size "size"
    for(int i = 0; i < size; i++)
    {
        arr[i] = rand() % 100 - 25;//filling the array with random numbers between -25 and 74
    }
    algo1(arr, size);
    algo2(arr, size);
    algo3(arr, size);
    return 0;

}
