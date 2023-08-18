#include <iostream>
#include <vector>

struct Elements2
{
    int x, y;
};

struct Elements4
{
    union
    {
        struct
        {
            int w, x, y, z;
        };
        struct
        {
            Elements2 a, b;
        };
    };
};

struct Union
{
    union
    {
        int a;
        int b;
    };
};

int main()
{
    Elements4 e {1, 2, 3, 4};
    // Print 1, 2, 3, 4
    std::cout << e.w << std::endl;
    std::cout << e.x << std::endl;
    std::cout << e.y << std::endl;
    std::cout << e.z << std::endl;

    std::cout << "---------------------" << std::endl;
    // Print 1, 2, 3, 4 as well, because you access the same memory
    std::cout << e.a.x << std::endl;
    std::cout << e.a.y << std::endl;
    std::cout << e.b.x << std::endl;
    std::cout << e.a.y << std::endl;
}