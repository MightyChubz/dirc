// dirc.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <filesystem>

#if defined(_WIN32) || defined(_WIN64)
	#include <Windows.h>
#endif

#include <array>

#include "dircdef.h"

namespace fs = std::filesystem;

struct file_entry
{
	std::string_view file_type;
	std::string filename;
	std::string size;

	file_entry(const std::string_view file_type, std::string filename, std::string size) : file_type(file_type), filename(
		std::move(filename)), size(
		std::move(size)) {}
};

constexpr void trim_left(std::string &e, const std::string_view chars)
{
	e.erase(0, e.find_first_not_of(chars));
}

constexpr void trim_right(std::string& e, const std::string_view chars)
{
	e.erase(e.find_last_not_of(chars) + 1);
}

std::string get_rounded_size(const u64 file_size)
{
	u8 i = 0;
	f64 size = static_cast<f64>(file_size);
	for (; size >= 1024.; size /= 1024., ++i) {}
	size = std::ceil(size * 10.) / 10.;

	std::string size_string = std::to_string(size);
	trim_right(size_string, "000");
	trim_right(size_string, "."); // Here just in case there is no trailing number after.
	return std::string(size_string + "BKMGTPE"[i]);
}

std::list<file_entry> get_working_entries(const fs::path& path)
{
	std::list<file_entry> entries;
	for (const auto & directory_entry : fs::directory_iterator{ path })
	{
		const std::string_view file_type = directory_entry.is_directory() ? "<DIR>" : "<FILE>";
		const std::string file_size = get_rounded_size(directory_entry.file_size());
		std::string filename = directory_entry.path().filename().string();
		constexpr std::string_view chars = "\"";
		trim_left(filename, chars);
		trim_right(filename, chars);

		if (directory_entry.is_directory())
			entries.emplace_front(file_type, filename, file_size);
		else
			entries.emplace_back(file_type, filename, file_size);
	}

	return entries;
}

void print_rainbow(const std::list<file_entry> &entries)
{
#if defined(_WIN32) || defined(_WIN64)
	typedef const HANDLE const_handle;
	const_handle h_console = GetStdHandle(STD_OUTPUT_HANDLE);
	constexpr std::array<WORD, 12> colors {1, 2, 3, 4, 5, 6, 9, 10, 11, 12, 13, 15};

	u8 i = 0;
	u8 k = 0;
	for (const file_entry & entry : entries)
	{
		if (i % 2 == 0)
		{
			SetConsoleTextAttribute(h_console, colors[k]);
			++k;
		}
		
		std::cout << std::setw(30) << entry.filename << " | " << std::setw(6) << entry.size << " | " << std::setw(6) << entry.file_type << std::endl;

		++i;

		if (k > colors.size())
		{
			i = 0;
			k = 0;
		}
	}
#else
	constexpr std::array<std::string_view, 7> color_start{
		"\\x1B[31m",
		"\\x1B[32m",
		"\\x1B[33m",
		"\\x1B[34m",
		"\\x1B[35m",
		"\\x1B[36m",
		"\\x1B[37m",
	};
	constexpr std::string_view reset = "\\x1B[0m";

	u8 i = 0;
	u8 k = 0;
	for (const file_entry& entry : entries)
	{
		if (i % 2 == 0)
		{
			++k;
		}

		std::cout << color_start[k] << std::setw(30) << entry.filename << " | " << std::setw(6) << entry.size << " | " << std::setw(6) << entry.file_type << std::endl << reset;

		++i;

		if (k > colors.size())
		{
			i = 0;
			k = 0;
		}
	}
#endif
}

int main()
{
	const std::list<file_entry> entries = get_working_entries(fs::current_path());
	print_rainbow(entries);
	return 0;
}
