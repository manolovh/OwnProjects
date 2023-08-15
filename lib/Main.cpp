#include <algorithm>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <iostream>
#include <string>
#include <set>
#include <vector>
#include "../inc/Header.hpp"

using path_t = std::vector<std::filesystem::__cxx11::path>;
static const size_t SEPARATOR_COUNT = 20;

struct Tree
{
	std::string name;
	std::vector<Tree> children;
	unsigned long size;
	int64_t time;


	Tree(std::string name) : name{name} {}
	
	Tree(std::string name, int64_t time)
		: name{name}, time{time} {}

	Tree(std::string name,
		 unsigned long size,
		 int64_t time
		)
		: name{name}, size{size}, time{time} {}
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

void fill_flags(std::set<std::string> const& input_flags, Flags& all_flags)
{
	all_flags.show_only_dirs = std::find(input_flags.begin(), input_flags.end(), "-dir") != input_flags.end();
	all_flags.show_only_files = std::find(input_flags.begin(), input_flags.end(), "-file") != input_flags.end();
	all_flags.show_most_recent = std::find(input_flags.begin(), input_flags.end(), "-time") != input_flags.end();
	all_flags.show_biggest_first = std::find(input_flags.begin(), input_flags.end(), "-size") != input_flags.end();
}

void print_directory(Tree const& directory)
{
	static int directories_in = 0;
	std::string str_copy = directory.name;

	if (directories_in > 0)
	{
		str_copy.pop_back();

		std::string dir_to_print = str_copy.substr(str_copy.find_last_of("/") + 1) + "/";

		std::cout << "\n\n";
		std::cout << std::string(directories_in * 2, ' ') << "/" << dir_to_print << " (" << directory.size << "B)";
	}
	else
	{
		std::cout << "===== FILES AND SUB-DIRECTORIES IN CWD =====";
	}

	for (auto& elem: directory.children)
	{
		if (elem.name.back() == '/')
		{
			directories_in++;
			print_directory(elem);
		}
		else
		{
			std::cout << "\n" << std::string((directories_in + 1) * 2, ' ')
					  << elem.name << " (" << elem.size << "B)";
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

			current_dir.children.emplace_back(
				Tree(
					current_name + "/",
					entry.last_write_time().time_since_epoch().count()
				)
			);

			collect_data(current_dir.children[current_dir.children.size() - 1]);
		}
		// Add all files that are not hidden
		else if (entry.is_regular_file() && (entry.path().filename().c_str()[0] != '.'))
		{
			element_name = entry.path().filename().c_str();
			current_dir.children.emplace_back(
				Tree(
					element_name,
					entry.file_size(),
					entry.last_write_time().time_since_epoch().count()
				)
			);
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

void sort_by_size(Tree& current_dir)
{
	unsigned long current_dir_size = 0;
	for (auto& elem: current_dir.children)
	{
		if (elem.name[elem.name.size() - 1] == '/')
		{
			sort_by_size(elem);
			current_dir_size += elem.size;
		}
		else
		{
			current_dir_size += elem.size;
		}
	}

	std::sort(
		current_dir.children.begin(),
		current_dir.children.end(),
		[](Tree elem1, Tree elem2)
		{
			return (elem2.size < elem1.size);
		}
	);

	current_dir.size = current_dir_size;
}

void sort_by_time(Tree& current_dir)
{
	std::sort(
	current_dir.children.begin(),
	current_dir.children.end(),
	[](Tree elem1, Tree elem2)
	{
		return (elem2.time < elem1.time);
	}
	);
}

std::string name = std::filesystem::current_path();

int main(int argc, char *argv[])
{
	std::set<std::string> input_flags;
	for (int i = 1; i < argc; i++)
	{
		input_flags.emplace(argv[i]);
	}

	Tree current_dir = Tree(name);
	collect_data(current_dir);

	Flags all_flags;
	fill_flags(input_flags, all_flags);

	if (all_flags.show_only_dirs && all_flags.show_only_files)
	{
		std::cerr << "Invalid option: Incompatible flags (-file, -dir)" << std::endl;
		return -1;
	}
	else if (all_flags.show_most_recent && all_flags.show_biggest_first)
	{
		std::cerr << "Invalid option: Incompatible flags (-time, -size)" << std::endl;
		return -1;
	}
	
	if (all_flags.show_biggest_first)
	{
		sort_by_size(current_dir);
	}
	else if (all_flags.show_most_recent)
	{
		sort_by_time(current_dir);
	}

	print_directory(current_dir);

	std::cout << std::endl;
}
