#pragma once
#include <vector>
#include <array>
#include <fstream>
#include <filesystem>

template <typename T>
inline int binarySearchBetween(const std::vector<T>& values, const T& value, int l, int r)
{
	while (l != r)
	{
		int mid = (l + r) / 2 + 1;
		(values[mid] > value) ? r = mid - 1: l = mid;
	}

	return (values[l] <= value) ? l : -1;
}

template <typename T, int n>
inline int binarySearchBetween(const std::array<T, n>& values, const T& value, int l, int r)
{
	while (l != r)
	{
		int mid = (l + r) / 2 + 1;
		(values[mid] > value) ? r = mid - 1 : l = mid;
	}

	return (values[l] <= value) ? l : -1;
}

inline void clearTextFile(const std::string& path)
{
	std::ofstream textFileBad;
	textFileBad.open(path);
	textFileBad.close();
}

inline void getSubdirectories(const std::string& directory, std::vector<std::filesystem::path>& subdirs)
{
	std::filesystem::directory_iterator begin(directory);
	std::filesystem::directory_iterator end;
	std::copy_if(begin, end, std::back_inserter(subdirs), [](const std::filesystem::path& path) {return std::filesystem::is_directory(path); });
}

inline void getFilesFromDirectory(const std::string& directory, std::vector<std::filesystem::path>& files)
{
	std::filesystem::directory_iterator begin(directory);
	std::filesystem::directory_iterator end;
	std::copy_if(begin, end, std::back_inserter(files), [](const std::filesystem::path& path) {return std::filesystem::is_regular_file(path); });
}

inline void deleteFilesFromDirectory(const std::string& directory)
{
	std::vector<std::filesystem::path> files;
	getFilesFromDirectory(directory, files);

	for (int i = 0; i < files.size(); i++)
		std::filesystem::remove(files[i].string());
}
