// dirc.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

constexpr void trim_left(std::string &e, const std::string_view chars)
{
	e.erase(0, e.find_first_not_of(chars));
}

constexpr void trim_right(std::string& e, const std::string_view chars)
{
	e.erase(e.find_last_not_of(chars) + 1);
}

std::string get_rounded_size(const uintmax_t file_size)
{
	int i = 0;
	auto size = static_cast<double>(file_size);
	for (; size >= 1024.; size /= 1024., ++i) {}
	size = std::ceil(size * 10.) / 10.;

	std::string size_string = std::to_string(size);
	trim_right(size_string, "000");
	trim_right(size_string, "."); // Here just in case there is no trailing number after.
	return std::string(size_string + "BKMGTPE"[i]);
}

int main()
{

	for (const fs::path working_path = fs::current_path().generic_string(); const auto& directory_entry : fs::directory_iterator{working_path})
	{
		std::string mb_file_size = get_rounded_size(directory_entry.file_size());
		std::string filename = directory_entry.path().filename().string();
		constexpr std::string_view chars = "\"";
		trim_left(filename, chars);
		trim_right(filename, chars);
		
		std::cout << filename << " | " << mb_file_size << std::endl;
	}



	return 0;
}
