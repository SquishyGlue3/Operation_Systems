int foo (int num)
{
    int x = 100;
    num = x/num;
    return num;
}

int main(){
    int a = 0;
    int b = 0;
    a = foo(10);
    b = foo(0);
    return 0;
}