#pragma once

#include <filesystem>
#include <vector>
#include <memory>

#include "FileAnalyzer.h"
#include "Timer.h"

#define FIELD_WIDTH 10

class ProjectAnalyzer
{
public:
	ProjectAnalyzer(const std::string& project_root_dir);
	~ProjectAnalyzer();
	
	void Analyze();
	void Reset();

	std::string get_project_root_dir() const;
	void set_project_root_dir(const std::string& project_root_dir);
	std::forward_list<FileStatistic> get_statistics_of_files() const;
	FileStatistic get_total_files_statistic() const;
	std::size_t get_total_files_processed() const;

private:
	void SearchFiles();
	void CalculateTotalStatistic();
	void CreateFileAnalyzers();
	void DestroyFileAnalyzers();
	void InitThreadPool();
	void JoinThreads();

	friend std::ostream& operator<<(std::ostream& os, const ProjectAnalyzer& project_analyzer);	

	std::string m_project_root_dir;
	std::queue<std::string> m_project_filenames;

	std::vector<std::unique_ptr<FileAnalyzer>> m_file_analyzers;
	std::vector<std::thread> m_thread_pool;

	std::forward_list<FileStatistic> m_statistics_of_files;
	FileStatistic m_total_files_statistic;
	std::size_t m_total_files_processed;

	std::mutex m_mutex;
	std::condition_variable m_cond_var;
	Timer m_timer;

	static const std::string FILE_EXTENSION_NAMES[];
};
