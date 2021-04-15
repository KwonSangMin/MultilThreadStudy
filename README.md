# MultilThreadStudy
본 project는 https://popcorntree.tistory.com/15 블로그를 참조하여 진행하였습니다.

만약 아래의 코드를 실행한다면
<pre>
<code>
#include <iostream>
#include <thread>
#include <atomic>

using namespace std;

#define MAX 1000000
volatile int a = 0;
volatile int b = 0;
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
    }
}

</code>
</pre>
<hr/>
<img width="80%" src="https://user-images.githubusercontent.com/38064756/114533768-38a93d00-9c89-11eb-86f9-5a1ac526339b.jpg"/>


위와 같은 결과값을 얻을 수 있습니다.
위의 결과값에서 Thread A가 func1을 실행하면서 x=849일때 y의 값이 891이라는 것을 확인할 수 있습니다. 그리고 Thread B가 func2를 실행하면서 y=891일 때는 x가 842라는 값으로 참조하는 것을 확인할수 있습니다.
이어서, 다음 줄을 확인해보면 x=861일 때 y=894입니다. 그리고, y=894일 때는 x=842입니다.
각각의 결과가 의미하는바는 ThreadB에서는 y가 897의 값을 가질 때까지 x는 값이 업데이트되지 않습니다. 하지만, Thread A에서는 y가 897이 되기전에 x를 849~865까지 계속 업데이트 하는 것을 볼 수 있습니다. 이를 통해, 아래의 이미지에서 보이는 것처럼 메모리 일관성이 깨졋다는 것을 확인할 수 있습니다.


<img width="80%" src="https://user-images.githubusercontent.com/38064756/114533695-229b7c80-9c89-11eb-831f-1297cc638e43.png"/>

이를 해결하기위해 a와 b를 atomic<int>로 바꿔준다면 ( 각 변수들의 값을 갱신할 때 Atomic하게 실행시켜준다면 ) 메모리 일관성을 유지할 수 있습니다.
그 이유는 Atomic 자료형은 메모리를 읽고 갱신하고 다시 해당 메모리에 값을 쓰는 작업을 1개의 명령어로 처리해주도록 합니다. 다시말해서, 메모리를 읽고 쓰는 작업을 하는 동안 다른 쓰레드로 문맥교환이 안된다는 뜻입니다. 이를 통해서, 두 쓰레드가 항상 동일한 값을 참조하도록 해줍니다.
또한 이외에도, Atomic 자료형은 기본적으로 memory_order_seq_cst 매크로를 사용하여 동작하여 
    <pre>
    <code>
    atomic<int> a = 10;
    atomic<int> b = 100;
    atomic<int> c = 1000;
    Thread A func(){
    b=c;
    c=a;
    a=c;
    }
    </code>
    </pre>
위와 같은 코드에서 순차적으로 명령어를 실행하도록 보장해줍니다.
메모리 접근 매크로 :
.operation(memory_order_relaxed) : 메모리 접근 순서를 따로 케어하지 않습니다. cpu의 성능 향상을 고려하여 임의로 순서로 메모리를 접근합니다.
.operation(memory_order_release) : 이 명령어 실행 시 이 명령어보다 위에 선언 모든 메모리 명령어들을 처리하고 다음 명령어를 실행합니다. 따라서, 아래의 명령어들은 위의 명령어들의 결과를 참조할수 있습니다.
.operation(memory_order_acquire) : 이 명령어가 실행완료 되기 전까지 아래 명령어는 실행되지 않습니다. 위에 명령어의 순서는 관여하지 않습니다.
.operation(memory_order_seq_cst) : 이 명령어는 모든 쓰레드가 동일한 메모리 값을 참조하도록 제어합니다

** Single Thread의 경우에도 CPU의 최적화로 인해서 메모리 접근 명령어가 순서대로 실행되지 않을 수 있지만 CPU 파이프라인에서 특정 변수에 대한 값을 확인하는 경우 해당 명령어 실행과정에서 파이프라인에 해당 변수에 대한 작업이 있는지 확인하고 있다면 forwarding을 통해서 해당 값을 불러옵니다. 만약 해당 변수에 대한 작업이 파이프라인에 없다면 메모리를 참조해서 값을 불러오게 됩니다.
이를 통해서 Single Thread에서는 명시적으로 메모리 오더를 조절해주지  프로그래머가 의도한대로 명령어가 실행되도록 합니다.

memory_order 관련 내용은 https://modoocode.com/271 게시글을 참조하였습니다.
