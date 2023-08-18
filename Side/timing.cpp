#include <iostream>
#include <chrono>
#include <thread>

using namespace std::literals::chrono_literals;

struct Timer
{
    std::chrono::_V2::system_clock::time_point start, end;
    std::chrono::duration<float> duration;

    Timer()
    {
        start = std::chrono::high_resolution_clock::now();
    }

    ~Timer()
    {
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        std::cout << "Total duratuion: " << duration.count() * 1000.0 << "ms" << std::endl;
    }
};

void PrintNums(int count)
{
    for (int i = 0; i < count; i++)
    {
        std::cout << i << std::endl;
    }
}

int main()
{
    Timer t;
    std::thread thr(PrintNums, 10000);
    thr.join();
}