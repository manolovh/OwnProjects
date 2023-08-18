#include <iostream>
#include <thread>

struct A
{
    void operator() () const
    {
        std::cout << std::this_thread::get_id() << std::endl;
        for (int i = 1; i < 500; i++)
        {
            std::cout << "Thread 1 >  " << i << std::endl;
        }
    }
};
struct B
{
    void operator() () const
    {
        std::cout << std::this_thread::get_id() << std::endl;
        for (int i = 500; i < 1000; i++)
        {
            std::cout << "Thread 2 >  " << i << std::endl;
        }
    }
};

int main()
{
    A a;
    B b;
    std::thread worker(a);
    std::thread worker2(b);

    worker.join();
    worker2.join();

    std::cout << std::this_thread::get_id() << std::endl;

    std::cin.get();
}