#ifndef _MAIN_HPP
#define _MAIN_HPP

#include <vector>
#include <string>

struct Flags
{
	bool show_only_dirs;
	bool show_only_files;
	bool show_biggest_first;
	bool show_most_recent;
};

class Tree
{
public:
	std::string name;
	std::vector<Tree> children;
	unsigned long size;
	int64_t time;

	Tree(std::string name);

	Tree(std::string name, int64_t time);

	Tree(std::string name, unsigned long size, int64_t time);
};

#endif
