#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include "../inc/Header.hpp"

using path_t = std::vector<std::filesystem::__cxx11::path>;
static const size_t SEPARATOR_COUNT = 20;

struct Tree
{
	std::string name;
	std::vector<Tree> children;
	std::string parent;

	Tree(std::string name) : name{name} {}

	Tree(std::string name, std::string parent)
		: name{name}, parent{parent} {}
};

struct Flags
{
	bool show_only_dirs;
	bool show_only_files;
	bool show_biggest_first;
	bool show_most_recent;
};

// Used for sorting all files before directories
bool operator<(Tree const& obj1, Tree const& obj2)
{
	return (obj1.name.find("/") != std::string::npos) < (obj2.name.find("/") != std::string::npos);
}

void fill_flags(std::vector<std::string> const& input_flags, Flags& all_flags)
{
	all_flags.show_only_dirs = std::find(input_flags.begin(), input_flags.end(), "-dir") != input_flags.end();
	all_flags.show_only_files = std::find(input_flags.begin(), input_flags.end(), "-file") != input_flags.end();
	all_flags.show_most_recent = std::find(input_flags.begin(), input_flags.end(), "-time") != input_flags.end();
	all_flags.show_biggest_first = std::find(input_flags.begin(), input_flags.end(), "-size") != input_flags.end();
}

void print_directory(Tree const& directory, Flags const& flags)
{
	static int directories_in = 0;
	std::string str_copy = directory.name;

	if (directories_in > 0)
	{
		str_copy.pop_back();

		std::string dir_to_print = str_copy.substr(str_copy.find_last_of("/") + 1) + "/";

		std::cout << "\n\n";
		std::cout << std::string(directories_in * 2, ' ') << "/" << dir_to_print;
	}
	else
	{
		std::cout << "===== FILES AND SUB-DIRECTORIES IN CWD =====";
	}

	for (auto elem: directory.children)
	{
		if (elem.name.back() == '/')
		{
			directories_in++;
			print_directory(elem, flags);
		}
		else
		{
			std::cout << "\n" << std::string((directories_in + 1) * 2, ' ') << elem.name;
		}
	}
	directories_in--;
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

	// Sort files to be printed first, then directories
	std::sort(current_dir.children.begin(), current_dir.children.end(),
		[] (Tree obj1, Tree obj2)
			{
				return obj1 < obj2;
			}
	);

	std::filesystem::current_path("../");
}

std::string name = std::filesystem::current_path();

int main(int argc, char *argv[])
{
	std::vector<std::string> input_flags;
	for (int i = 1; i < argc; i++)
	{
		input_flags.emplace_back(argv[i]);
	}

	Flags all_flags;
	fill_flags(input_flags, all_flags);

	if (all_flags.show_only_dirs && all_flags.show_only_files)
	{
		std::cerr << "Invalid option: Incompatible flags (-file, -dir)" << std::endl;
		return -1;
	}

	Tree current_dir = Tree(name, "");
	collect_data(current_dir);
	print_directory(current_dir, all_flags);

	std::cout << std::endl;
}
