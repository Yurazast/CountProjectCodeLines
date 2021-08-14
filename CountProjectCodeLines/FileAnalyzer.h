#pragma once

#include <string>
#include <fstream>
#include <condition_variable>
#include <queue>

#include "FileStatistic.h"

class FileAnalyzer
{
public:
	FileAnalyzer(std::queue<std::string>& filenames_queue, std::mutex& mutex, std::condition_variable& cond_var);

	FileStatistic Analyze();

	FileStatistic get_file_statistic() const;

private:
	void CountLines();

	std::queue<std::string>& m_filenames_queue;
	std::ifstream m_input_file;
	FileStatistic m_file_statistic;
	std::mutex& m_mutex;
	std::condition_variable& m_cond_var;
};
