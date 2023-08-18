#include <algorithm>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <iostream>
#include <set>
#include "../inc/TerminalLister.hpp"

using path_t = std::vector<std::filesystem::__cxx11::path>;
static const size_t SEPARATOR_COUNT = 20;

Tree::Tree(std::string name)
	: name{name}
{
}

Tree::Tree(std::string name, int64_t time)
	: name{name}, time{time}
{
}

Tree::Tree(std::string name, unsigned long size, int64_t time)
	: name{name}, size{size}, time{time} 
{
}

void print_directory(Tree const& directory, Flags const& flags)
{
	// Variable, used to specify the depth of the tree indentation
	static int directories_in = 0;
	std::string str_copy = directory.name;

	if (directories_in > 0)
	{
		// Located in sub-directory
		str_copy.pop_back();

		std::string dir_to_print = str_copy.substr(str_copy.find_last_of("/") + 1) + "/";

		if (!flags.show_only_files)
		{
			std::cout << "\n\n";
			if (flags.show_biggest_first)
			{
				std::cout << std::string(directories_in * 2, ' ') << "/" << dir_to_print 
				          << " (" << directory.size << "B)";
			}
			else
			{
				std::cout << std::string(directories_in * 2, ' ') << "/" << dir_to_print;
			}
		}
	}
	else
	{
		// Located in initial directory, print header
		if ((!flags.show_only_files) && (!flags.show_only_dirs))
		{
			std::cout << "===== FILES AND SUB-DIRECTORIES IN CWD =====";
		}
		else if (flags.show_only_files)
		{
			std::cout << "=============== FILES IN CWD ===============";
		}
		else if (flags.show_only_dirs)
		{
			std::cout << "========== SUB-DIRECTORIES IN CWD ==========";
		}
	}

	for (auto& elem: directory.children)
	{
		if ((elem.name.back() == '/'))
		{
			// If elem is directory and flag for listing only files not chosen
			directories_in++;
			print_directory(elem, flags);
		}
		else if (!flags.show_only_dirs)
		{
			if (flags.show_biggest_first)
			{
				// Size sort was chosen, print formatted name with size
				std::cout << "\n" << std::string((directories_in + 1) * 2, ' ')
							<< elem.name << " (" << elem.size << "B)";
			}
			else
			{
				// Size sort was chosen, print formatted name with size
				std::cout << "\n" << std::string((directories_in + 1) * 2, ' ')
						  << elem.name << " (" << elem.size << "B)";
			}
		}
	}

	directories_in--;
}

void collect_data(Tree& current_dir)
{
	// Collect data for current and sub-directories.
	for (const auto& entry: std::filesystem::directory_iterator(current_dir.name))
	{
		if (entry.is_directory() && (entry.path().filename().c_str()[0] != '.'))
		{
			// Add all directories that are not hidden
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
		else if (entry.is_regular_file() && (entry.path().filename().c_str()[0] != '.'))
		{
			// Add all files that are not hidden
			current_dir.children.emplace_back(
				Tree(
					entry.path().filename().c_str(),
					entry.file_size(),
					entry.last_write_time().time_since_epoch().count()
				)
			);
		}
	}

	// Sort alphabetically
	std::sort(current_dir.children.begin(), current_dir.children.end(),
		[] (Tree obj1, Tree obj2)
		{
			return obj1.name < obj2.name;
		}
	);

	std::filesystem::current_path("../");
}

void sort_by_size(Tree& current_dir)
{
	// Sort by size recursively. Directory size is determined by files in current and sub-directories.
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

void fill_flags(std::set<std::string> const& input_flags, Flags& flags)
{
	// Fill all chosen flags
	flags.show_only_dirs = std::find(input_flags.begin(), input_flags.end(), "-dir") != input_flags.end();
	flags.show_only_files = std::find(input_flags.begin(), input_flags.end(), "-file") != input_flags.end();
	flags.show_most_recent = std::find(input_flags.begin(), input_flags.end(), "-time") != input_flags.end();
	flags.show_biggest_first = std::find(input_flags.begin(), input_flags.end(), "-size") != input_flags.end();
}

int main(int argc, char *argv[])
{
	// Collect passed flags
	Flags flags;
	std::set<std::string> input_flags;

	for (int i = 1; i < argc; i++)
	{
		input_flags.emplace(argv[i]);
	}

	fill_flags(input_flags, flags);

	// If incompatible flags exist - terminate the program
	if (flags.show_only_dirs && flags.show_only_files)
	{
		std::cerr << "Invalid option: Incompatible flags (-file, -dir)" << std::endl;
		return -1;
	}
	else if (flags.show_most_recent && flags.show_biggest_first)
	{
		std::cerr << "Invalid option: Incompatible flags (-time, -size)" << std::endl;
		return -1;
	}

	// Create a tree structure and collect data file/directory from current directory -> down
	std::string name = std::filesystem::current_path();
	Tree current_dir = Tree(name);

	collect_data(current_dir);

	// If chosen, sort on either biggest or most recently modified file/directory first
	if (flags.show_biggest_first)
	{
		sort_by_size(current_dir);
	}
	else if (flags.show_most_recent)
	{
		sort_by_time(current_dir);
	}

	// Print files and directories in a tree format
	print_directory(current_dir, flags);

	std::cout << std::endl;
}
