# MultilThreadStudy
본 project는 https://popcorntree.tistory.com/15 블로그를 참조하여 진행하였습니다.

만약 아래의 코드를 실행한다면
<pre>
<code>
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


</code>
</pre>

<img width="80%" src="https://user-images.githubusercontent.com/38064756/114533768-38a93d00-9c89-11eb-86f9-5a1ac526339b.jpg"/>
위와 같은 결과값을 얻을 수 있습니다.
위의 결과값에서 Thread A가 func1을 실행하면서 x=849일때 y의 값이 891이라는 것을 확인할 수 있습니다. 그리고 Thread B가 func2를 실행하면서 y=891일 때는 x가 842라는 값으로 참조하는 것을 확인할수 있습니다.
이어서, 다음 줄을 확인해보면 x=861일 때 y=894입니다. 그리고, y=894일 때는 x=842입니다.
각각의 결과가 의미하는바는 ThreadB에서는 y가 897의 값을 가질 때까지 x는 값이 업데이트되지 않습니다. 하지만, Thread A에서는 y가 897이 되기전에 x를 849~865까지 계속 업데이트 하는 것을 볼 수 있습니다. 이를 통해, 아래의 이미지에서 보이는 것처럼 메모리 일관성이 깨졋다는 것을 확인할 수 있습니다.


<img width="80%" src="https://user-images.githubusercontent.com/38064756/114533695-229b7c80-9c89-11eb-831f-1297cc638e43.png"/>
