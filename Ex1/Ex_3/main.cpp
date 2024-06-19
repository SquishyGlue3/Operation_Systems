
#include <iostream>
#include "../Ex_2/Poisson.hpp"
using namespace std;
int main() {
    cout << calculatePoisson(2, 1) << endl;
    cout << calculatePoisson(2, 10) << endl;
    cout << calculatePoisson(2, 2) << endl;
    cout << calculatePoisson(3, 3) << endl;
    cout << calculatePoisson(100, 3) << endl;
    return 0;
}