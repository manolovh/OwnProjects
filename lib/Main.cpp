#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include "../inc/Header.hpp"

using path_t = std::vector<std::filesystem::__cxx11::path>;

struct Tree
{
	std::string name;
	std::vector<Tree> children;
	std::string parent;

	Tree(std::string name) : name{name} {}

	Tree(std::string name, std::string parent)
		: name{name}, parent{parent} {}
};

void print_directory(const Tree& directory)
{
	static int directories_in = 0;
	std::string str_copy = directory.name;
	if (directories_in > 0)
		str_copy.pop_back();

	std::string dir_to_print = str_copy.substr(str_copy.find_last_of("/") + 1) + "/";

	std::cout << std::right << std::setw(directories_in * 3) << dir_to_print << std::endl;
	for (auto elem: directory.children)
	{
		if (elem.children.empty())
		{
			std::cout << std::setw(directories_in * 3) << "\t" << elem.name << std::endl;
		}
		else
		{
			directories_in++;
			print_directory(elem);
		}
	}
	directories_in--;;
}

void collect_data(Tree& current_dir)
{
	static std::string element_name;
	for (const auto& entry: std::filesystem::directory_iterator(current_dir.name))
	{
		// Add all directories that are not hidden
		if (entry.is_directory() && (entry.path().filename().c_str()[0] != '.'))
		{
			std::filesystem::current_path(entry.path());
			std::string current_name = std::filesystem::current_path();
			current_dir.children.emplace_back(Tree(current_name + "/"));

			collect_data(current_dir.children[current_dir.children.size() - 1]);
		}
		// Add all files that are not hidden
		else if (entry.is_regular_file() && (entry.path().filename().c_str()[0] != '.'))
		{
			element_name = entry.path().filename().c_str();
			current_dir.children.emplace_back(Tree(element_name));
		}
	}
	std::filesystem::current_path("../");
}

std::string name = std::filesystem::current_path();

int main(int argc, char *argv[])
{
	if (argv[1])
	{
		name += '/' + argv[1];
	}

	Tree current_dir = Tree(name, "");

	collect_data(current_dir);
	print_directory(current_dir);
}
