#include <iostream>
#include <cmath>
#include "Poisson.hpp"
using namespace std;

long double calculatePoisson(double lambda, double k)
{
    return exp(-lambda) * pow(lambda, k) / tgamma(k + 1);//tgamma is the gamma function which is used to calculate the factorial of a number, exp is the exponential function
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "Error, You have entered more or less than 2 arguments" << endl;
        return 1;
    }
    double lambda = atof(argv[1]);//converts a string to a double
    double k = atof(argv[2]);//converts a string to a double
    long double result = calculatePoisson(lambda, k);//calculates the Poisson probability
    printf("The Poisson probability of %f for %f is %Lf\n", lambda, k, result);
    return 0;
}
