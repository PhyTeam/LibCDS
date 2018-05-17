#include <thread>
#include <future>
#include <functional>
#include <memory>

#include <vector>

#include "priorityqueue.h"

#include <iostream>

using namespace std;
using namespace Concurrency;

struct MyObject
{
    static int instance_counter;
    int i;
    MyObject(int i_) : i(i_)
    {
        cout << "Ctor " << i << endl;
        instance_counter += 1;
    }

    ~MyObject()
    {
        cout << "Dtor " << i << endl;
        instance_counter--;
    }
};

int MyObject::instance_counter = 0;

TEST(PriorityQueue, Init)
{
    PriorityQueue<int, int, SingleThreaded> pq1;
    EXPECT_EQ(pq1.size(), 0);
    PriorityQueue<int, MyObject, SingleThreaded> pq2;
    EXPECT_EQ(pq2.size(), 0);

    PriorityQueue<double, int, SingleThreaded> pq3;
    EXPECT_TRUE(pq3.empty());
}

TEST(PriorityQueue, Init_Mutex)
{
    PriorityQueue<int, int, ObjectLockable> pq1;
    EXPECT_EQ(pq1.size(), 0);
    PriorityQueue<int, MyObject, ObjectLockable> pq2;
    EXPECT_EQ(pq2.size(), 0);

    PriorityQueue<double, int, ObjectLockable> pq3;
    EXPECT_TRUE(pq3.empty());
}

TEST(PriorityQueue, push)
{
    PriorityQueue<int, int, SingleThreaded> pq;
    pq.push(1, 1);
    ASSERT_EQ(pq.size(), 1);
    pq.push(1, 1);
    ASSERT_EQ(pq.size(), 2);

    for (int i = 0; i < 1000; ++i) {
        pq.push(i, i);
    }

    ASSERT_EQ(pq.size() - 2, 1000);
}

TEST(PriorityQueue, pushAndPopShouldWorkProperly)
{
    PriorityQueue<int, int, SingleThreaded> queue;
    queue.push(0, 100);
    queue.push(0, 101);
    queue.push(0, 102);
    cout << queue.size() << endl;
    queue.push(1, 1);
    cout << queue.size() << endl;
    queue.push(1, 2);
    cout << queue.size() << endl;

    queue.push(1, 3);
    cout << queue.size() << endl;

    queue.push(2, 4);
    queue.push(2, 5);
    queue.push(3,10);
    queue.push(3,11);
    queue.push(5,20);
    queue.push(5,21);

    cout << queue.size() << endl;

    cout << "Pop" << endl;

    while (queue.size() > 0) {
        cout << queue.top() << " ";
        queue.pop();
    }
    cout << endl;

    cout << "Testing leak memory" << endl;
    PriorityQueue<int, MyObject*, SingleThreaded> myqueue;
    myqueue.push(1, new MyObject(1));
    myqueue.push(2, new MyObject(2));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
//    ::testing::initgooglemock(&argc, argv);
    return RUN_ALL_TESTS();
}


/*

int main()
{
    PriorityQueue<int, int, SingleThreaded> queue;
    queue.push(0, 100);
    queue.push(0, 101);
    queue.push(0, 102);
    cout << queue.size() << endl;
    queue.push(1, 1);
    cout << queue.size() << endl;
    queue.push(1, 2);
    cout << queue.size() << endl;

    queue.push(1, 3);
    cout << queue.size() << endl;

    queue.push(2, 4);
    queue.push(2, 5);
    queue.push(3,10);
    queue.push(3,11);
    queue.push(5,20);
    queue.push(5,21);

    cout << queue.size() << endl;

    cout << "Pop" << endl;

    while (queue.size() > 0) {
        cout << queue.top() << " ";
        queue.pop();
    }
    cout << endl;

    cout << "Testing leak memory" << endl;
    PriorityQueue<int, MyObject*, SingleThreaded> myqueue;
    myqueue.push(1, new MyObject(1));
    myqueue.push(2, new MyObject(2));

    cout << "Instance " << MyObject::instance_counter << endl;
//    while (!myqueue.empty())
//    {
//        myqueue.pop();
//    }
    myqueue.clear();
    cout << "Instance " << MyObject::instance_counter << endl;

    Container<int, SingleThreaded> con;
    con.get();
    return 0;
}
*/
