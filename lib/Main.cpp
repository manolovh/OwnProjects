#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include "../inc/Header.hpp"

std::string name = std::filesystem::current_path();
std::vector<std::filesystem::__cxx11::path> files;
std::vector<std::filesystem::__cxx11::path> directories;

int main()
{
	for (const auto& entry: std::filesystem::directory_iterator(name))
	{
		if (entry.is_directory())
		{
			directories.push_back(entry.path().filename().concat("/").c_str());
		}
	}
	int i = 1;
	for (auto elem: directories)
		std::cout << std::setw(5) << std::left << i++ << elem.c_str() << std::endl;
}
