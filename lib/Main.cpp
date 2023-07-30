#include <iostream>
#include "../inc/Header.hpp"

int main()
{
	Data person_data;

	person_data.age = 18;
	person_data.weight = 85;

	std::cout << "AGE: " << person_data.age << " WEIGHT: " << person_data.weight << std::endl;
}
