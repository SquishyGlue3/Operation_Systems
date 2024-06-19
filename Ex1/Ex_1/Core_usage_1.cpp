int foo(int x, int y) {
    x = 4;
    y = 5;
    foo(x, y);
    return x+y;
}

int main() {
    int a = 1;
    int b = 2;
    foo(a, b);
    return 0;
}