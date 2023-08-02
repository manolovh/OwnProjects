#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include "../inc/Header.hpp"

using path_t = std::vector<std::filesystem::__cxx11::path>;

struct Tree
{
	std::string m_name;
	std::vector<Tree> children;

	Tree(std::string name) : m_name{name} {}
};

void print_directory(const Tree& directory)
{
	std::cout << directory.m_name << std::endl;
	for (auto elem: directory.children)
	{
		if (dynamic_cast<Tree*>(&elem.children[0]) == nullptr)
		{
			print_directory(elem);
		}
		else
		{
			std::cout << elem.m_name << std::endl;
		}
	}
}

void collect_data(Tree& current_dir)
{
	static std::string elem_name;
	static std::string previous_dir;
	for (const auto& entry: std::filesystem::directory_iterator(current_dir.m_name))
	{
		// Add all directories that are not hidden
		if (entry.is_directory() && (entry.path().filename().c_str()[0] != '.'))
		{
			elem_name += entry.path().filename().concat("/").c_str();
			current_dir.children.emplace_back(Tree(elem_name));

			previous_dir += current_dir.m_name + "/";
			collect_data(current_dir.children[current_dir.children.size() - 1]);

			elem_name = previous_dir;
		}
		// Add all files that are not hidden
		else if (entry.is_regular_file() && (entry.path().filename().c_str()[0] != '.'))
		{
			elem_name = entry.path().filename();
			current_dir.children.push_back(elem_name);
		}
		// Reset current directory name
		else
		{
			elem_name = previous_dir;
		}
	}
}

std::string name = std::filesystem::current_path();

int main(int argc, char *argv[])
{
	if (argv[1])
	{
		name += '/';
		name += argv[1];
	}

	Tree current_dir = Tree(name);

	collect_data(current_dir);
	print_directory(current_dir);
}
