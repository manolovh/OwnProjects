#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <ostream>

struct Person
{
    int age;
    int height;

    Person(int age, int height)
        : age{age}, height{height} {}

    friend std::ostream& operator<<(std::ostream& os, Person const& p);
    bool operator<(Person& p)
    {
        return this->age < p.age;
    }
};

std::ostream& operator<<(std::ostream& os, Person const& p)
{
    os << "Age: " << p.age << " Height: " << p.height;
    return os;
}

int main()
{
    std::vector<Person> data;
    data.emplace_back(20, 180);
    data.emplace_back(27, 180);
    data.emplace_back(25, 165);
    data.emplace_back(18, 175);


    std::sort(data.begin(), data.end());
    for (auto elem: data)
    {
        std::cout << elem << std::endl;
    }
}