#pragma once
#include <vector>
#include <fstream>
#include <filesystem>
#include <iostream>

inline void clearTextFile(const std::string& path)
{
	std::ofstream textFileBad(path);
}

inline std::vector<std::filesystem::path> getFilesFromDirectory(const std::string& directory)
{
	std::vector<std::filesystem::path> files;
	std::filesystem::directory_iterator begin(directory);
	std::filesystem::directory_iterator end;
	std::copy_if(begin, end, std::back_inserter(files), [](const std::filesystem::path& path) {return std::filesystem::is_regular_file(path); });
	return files;
}

inline void deleteFilesFromDirectory(const std::string& directory)
{
	std::vector<std::filesystem::path> files = getFilesFromDirectory(directory);
	for (int i = 0; i < files.size(); i++)
		std::filesystem::remove(files[i].string());
}

inline std::vector<std::filesystem::path> getSubdirectories(const std::string& directory)
{
	std::vector<std::filesystem::path> subdirs;
	std::filesystem::directory_iterator begin(directory);
	std::filesystem::directory_iterator end;
	std::copy_if(begin, end, std::back_inserter(subdirs), [](const std::filesystem::path& path) {return std::filesystem::is_directory(path); });
	return subdirs;
}

inline void deleteSubdirectories(const std::string& directory)
{
	std::vector<std::filesystem::path> subdirectories = getSubdirectories(directory);
	for (int i = 0; i < subdirectories.size(); i++)
		std::filesystem::remove_all(subdirectories[i].string());
}

inline std::stringstream getFileStringStream(const std::string& path)
{
	std::ifstream f(path);

	try 
	{
		f.exceptions(f.failbit);
	}
	catch (const std::ios_base::failure& e)
	{
		std::cerr << "can't open file " + path + "!\n";
		throw;
	}

	std::stringstream buffer;
	buffer << f.rdbuf();
	return buffer;
}

inline std::string getFileDirectory(const std::string& filePath)
{
	size_t found = filePath.find_last_of("/\\");
	return filePath.substr(0, found);
}