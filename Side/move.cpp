#include <iostream>

class Object
{
public:

    Object() {}

    Object(int a)
    {
        std::cout << "Construct" << std::endl;
        m_number = new int(a);
    }

    Object(Object const& other)
    {
        std::cout << "Copy" << std::endl;
        m_number = new int(*(other.m_number));
    }

    Object(Object&& other) noexcept
    {
        std::cout << "Move" << std::endl;
        m_number = other.m_number;
        other.m_number = nullptr;
    }

    Object& operator=(Object&& other) noexcept
    {
        std::cout << "Assign" << std::endl;
        
        if (this != &other)
        {
            delete m_number;

            m_number = other.m_number;
            other.m_number = nullptr;
        }
        return *this;
    }

    ~Object()
    {
        delete m_number;
    }

private:
    int *m_number;
};


int main()
{
    Object obj {2};
    Object obj2;
    Object obj3 = std::move(obj);

    obj2 = std::move(obj);
    obj2 = 3;

    Object obj4 = obj2;

    Object obj11 = 2;
}