#include <iostream>
using namespace std;
int main() {
    int a = 5;
    int b = 6;
    int* c = &a;
    c = c+5;
    cout << *c << endl;
    int* d = nullptr;
    cout << *d << endl;
    return 0;   
}