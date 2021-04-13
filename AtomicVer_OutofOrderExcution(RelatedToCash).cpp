#include <iostream>
#include <thread>
#include <atomic>
using namespace std;
atomic<int> a = 0;
atomic<int> b = 0;

#define MAX 1000000
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
    for (int i = 0; i < MAX; i+=10000)
    {
        int a1value = arr1[i];
        int a2ValueAti = arr2[a1value];
        cout << "Arr1["<<i << "] == " <<a1value<<" arr2["<<a1value<<"] == " << a2ValueAti<<endl;
    }
}
