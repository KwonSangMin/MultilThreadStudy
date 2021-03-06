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

** Memory_order_seq_cst와 memory_order_acquire, release 동기화의 차이점은 Memory_order_acquire 동기화의 경우 다른 CPU코어에서의 값은 케어해주지 않습니다. 즉, Core A에서 해당 동기화를 통해 메모리 작업 순서를 동기화해주어도 Core B에서는 해당 결과 값들이 정상적으로 반영되지 않을 수 있습니다. 하지만, Memory_order_seq_cst는 모든 Core에서 해당 결과 값을 참조할 수 있도록 해줍니다. 따라서, Memory_order_seq_cst는 다른 메모리 동기화 매크로보다 느립니다.

** CPU의 최적화는 비순차적 명령어 처리 매커니즘을 통해 이루어집니다. 컴퓨터 CPU는 성능 향상을 위해서 아래와 같이 파이프라이닝 기법을 사용하고 있습니다.

<img width="80%" src="https://user-images.githubusercontent.com/38064756/115823014-81f34c80-a440-11eb-943b-7ccde55d9b99.png"/>

이와같은 과정에서, 명령어 퍼포먼스 향상을 위해서 비순차적 명령어 처리 매커니즘에서는 일반적으로 가장 최근에 완료된 명령어 이후 100개 가량의 명령어를 Window에 올려두고 명령어의 OPERAND(피연산자)가 준비 완료(해당 OPRERAND에 대한 선행연산이 완료되어야 준비완료로 처리됩 되는 순서대로 명령어를 실행하도록 합니다. 이를 통해서, CPU의 가동률을 상승시킬 수 있습니다. 하지만, 이러한 방법으로는 명령어 실행순서가 뒤죽박죽이 되어 문제가 발생합니다. 이를 해결하기 위해서 Reorder Buffer를 활용하여 실행 완료된 작업의 결과물을 순서대로 레지스터에 반영시켜줍니다. 이를 통해, 데이터 동기화를 이뤄냅니다.

** Single Thread의 경우에도 CPU의 최적화로 인해서 메모리 접근 명령어가 순서대로 실행되지 않을 수 있지만 CPU 파이프라인에서 특정 변수에 대한 값을 확인하는 경우 해당 명령어 실행과정에서 파이프라인에 해당 변수에 대한 작업이 있는지 확인하고 있다면 forwarding을 통해서 해당 값을 불러옵니다. 만약 해당 변수에 대한 작업이 파이프라인에 없다면 메모리를 참조해서 값을 불러오게 됩니다.
이를 통해서 Single Thread에서는 명시적으로 메모리 오더를 조절해주지 않아도 프로그래머가 의도한대로 명령어가 실행되도록 합니다.


memory_order 관련 내용은 https://modoocode.com/271 게시글을 참조하였습니다.
