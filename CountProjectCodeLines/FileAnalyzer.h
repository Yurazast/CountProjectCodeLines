#pragma once

#include <string>
#include <fstream>
#include <queue>
#include <forward_list>
#include <condition_variable>

#include "FileStatistic.h"

#define WHITESPACE " \t\r\n\f\v"

class FileAnalyzer
{
public:
	FileAnalyzer(std::queue<std::string>& filenames_queue, std::mutex& mutex, std::condition_variable& cond_var,
			bool& is_file_scanning_ended, std::forward_list<FileStatistic>& statistics_of_files);

	void Analyze();

	FileStatistic get_file_statistic() const;

private:
	void CountLines();
	void CheckIfMultilineComment(const std::string& line);
	void CheckIfCodeAfterComment(std::string&& line);

	std::queue<std::string>& m_filenames_queue;
	std::ifstream m_input_file;

	FileStatistic m_file_statistic;
	bool m_is_multiline_comment;

	std::mutex& m_mutex;
	std::condition_variable& m_cond_var;

	bool& m_is_file_scanning_ended;
	std::forward_list<FileStatistic>& m_statistics_of_files;
};
