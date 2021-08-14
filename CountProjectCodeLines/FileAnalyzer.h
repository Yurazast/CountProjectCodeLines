#pragma once

#include <string>
#include <fstream>
#include <condition_variable>

#include "FileStatistic.h"

class FileAnalyzer
{
public:
	FileAnalyzer(const std::string& filename, std::mutex& mutex, std::condition_variable& cond_var);
	~FileAnalyzer();

	void Analyze();

	FileStatistic get_file_statistic() const;

private:
	const std::string m_input_filename;
	std::ifstream m_input_file;
	FileStatistic m_file_statistic;
	std::mutex& m_mutex;
	std::condition_variable& m_cond_var;
};
