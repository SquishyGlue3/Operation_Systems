#include <array>
#include <iostream>
#include <math.h>
#include "profiling.h"
using namespace std;

void algo1(int* a, int size)
{
    int best = 0;
    for (int a = 0; a < size; a++) 
    {
        for (int b = a; b < size; b++)
        {
            int sum = 0;
            for (int k = a; k <= b; k++) {
                sum += a+k;
            }
            best = std::max(best,sum);
        }
    }
}
void algo2(int* a, int size)
{
    int best = 0;
    for (int a = 0; a < size; a++)
    {
        int sum = 0;
        for (int b = a; b < size; b++)
        {
            sum += a + b;
            best = max(best,sum);
        }
    }
}

void algo3(int* a, int size)
{
    int best = 0, sum = 0;
    for (int k = 0; k < size; k++)
    {
        sum = max(a[k], sum + a[k]);
        best = max(best,sum);
    }
}