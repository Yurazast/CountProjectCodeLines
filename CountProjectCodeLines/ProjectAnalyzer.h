#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <thread>
#include <queue>
#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>

#include "FileAnalyzer.h"

class ProjectAnalyzer
{
public:
	ProjectAnalyzer(const std::string& project_root_dir);
	~ProjectAnalyzer();
	
	void Analyze();

	std::size_t get_total_files_processed() const;
	std::vector<FileStatistic> get_files_statistics() const;
	FileStatistic get_total_files_statistic() const;

private:
	void SearchFiles();
	void ProcessFiles();
	void JoinThreads();

	friend std::ostream& operator<<(std::ostream& os, const ProjectAnalyzer& project_analyzer);	

	const std::string m_project_root_dir;
	std::queue<std::string> m_project_filenames;
	std::vector<std::thread> m_thread_pool;
	std::size_t m_total_files_processed;
	std::vector<FileStatistic> m_files_statistics;
	FileStatistic m_total_files_statistic;

	static const std::string FILE_EXTENSION_NAMES[];
};
