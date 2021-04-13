#include <iostream>
#include <thread>
#include <atomic>

using namespace std;

#define MAX 1000000
int a = 0;
int b = 0;
int arr1[MAX];
int arr2[MAX];
void func1()
{
    for (int i = 0; i < MAX; i++)
    {
        a = i;
        arr1[i] = b;
    }
}
void func2()
{
    for (int i = 0; i < MAX; i++)
    {
        b = i;
        arr2[i] = a;
    }
}
int main()
{
    std::cout << "Hello World!\n";
    thread A(func1), B(func2);
    A.join();
    B.join();
    for (int i = 0; i < MAX-1; i++)
    {
        int Ax = i;
        int Ay = arr1[i];
        int By = i;
        int Bx = arr2[i];
        if(arr2[Ay]!=Ax&&arr1[Ax]!=arr1[Ax+1])
            cout << "x == "<<i<<" y == "<<arr1[i] << ", y == " << Ay << " x == " << arr2[Ay] << endl;
    }
}
