#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <queue>
#include <vector>
#include <forward_list>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <future>

#include "FileAnalyzer.h"
#include "Timer.h"

class ProjectAnalyzer
{
public:
	ProjectAnalyzer(const std::string& project_root_dir);
	~ProjectAnalyzer();
	
	void Analyze();

	std::forward_list<FileStatistic> get_statistics_of_files() const;
	FileStatistic get_total_files_statistic() const;
	std::size_t get_total_files_processed() const;

private:
	void SearchFiles();
	void ProcessFiles();
	void GetFileStatisticInFuture(const std::size_t index);

	friend std::ostream& operator<<(std::ostream& os, const ProjectAnalyzer& project_analyzer);	

	const std::string m_project_root_dir;
	std::queue<std::string> m_project_filenames;

	std::vector<FileAnalyzer*> m_file_analyzers;
	std::vector<std::future<FileStatistic>> m_futures;

	std::forward_list<FileStatistic> m_statistics_of_files;
	FileStatistic m_total_files_statistic;
	std::size_t m_total_files_processed;

	std::mutex m_mutex;
	std::condition_variable m_cond_var;
	Timer m_timer;

	static const std::string FILE_EXTENSION_NAMES[];
};
