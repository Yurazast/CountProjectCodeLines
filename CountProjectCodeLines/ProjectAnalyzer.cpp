#include "ProjectAnalyzer.h"

const std::string ProjectAnalyzer::FILE_EXTENSION_NAMES[] {
	".cpp", ".c", ".h", ".hpp"
};

ProjectAnalyzer::ProjectAnalyzer(const std::string& project_root_dir)
		: m_project_root_dir(project_root_dir)
		, m_thread_pool(std::thread::hardware_concurrency())
		, m_total_files_processed(0)
{
	if (!std::filesystem::exists(m_project_root_dir))
		throw std::runtime_error("Directory \"" + project_root_dir + "\" doesn't exist");
}

ProjectAnalyzer::~ProjectAnalyzer()
{
}

void ProjectAnalyzer::Analyze()
{
	SearchFiles();
	ProcessFiles();
}

std::size_t ProjectAnalyzer::get_total_files_processed() const
{
	return m_total_files_processed;
}

std::vector<FileStatistic> ProjectAnalyzer::get_files_statistics() const
{
	return m_files_statistics;
}

FileStatistic ProjectAnalyzer::get_total_files_statistic() const
{
	return m_total_files_statistic;
}

void ProjectAnalyzer::SearchFiles()
{
	for (const std::filesystem::directory_entry& dir_entry : std::filesystem::recursive_directory_iterator(m_project_root_dir))
	{
		if (dir_entry.is_regular_file())
		{
			const std::string file_ext = dir_entry.path().extension().string();
			for (const std::string& ext_name : FILE_EXTENSION_NAMES)
			{
				if (file_ext == ext_name)
				{
					m_project_filenames.push(dir_entry.path().string());
					break;
				}
			}
		}
	}
}

void ProjectAnalyzer::ProcessFiles()
{
	std::mutex mutex;
	std::condition_variable cond_var;

	while (!m_project_filenames.empty())
	{
		for (std::size_t i = 0; i < m_thread_pool.size(); ++i)
		{
			std::unique_lock<std::mutex> lock(mutex);

			FileAnalyzer file_analyzer(m_project_filenames.front(), mutex, cond_var);
			m_project_filenames.pop();
			m_thread_pool[i] = std::thread{ &FileAnalyzer::Analyze, &file_analyzer };

			cond_var.wait(lock);

			FileStatistic file_statistic = file_analyzer.get_file_statistic();
			m_files_statistics.push_back(file_statistic);
			m_total_files_statistic += file_statistic;

			++m_total_files_processed;

			if (m_project_filenames.empty()) break;
		}

		JoinThreads();
	}
}

void ProjectAnalyzer::JoinThreads()
{
	for (std::vector<std::thread>::iterator it = m_thread_pool.begin(); it != m_thread_pool.end(); ++it)
	{
		if (it->joinable()) it->join();
	}
}

std::ostream& operator<<(std::ostream& os, const ProjectAnalyzer& project_analyzer)
{
	os << "-------------------------" << std::endl;
	os << project_analyzer.get_total_files_statistic();
	os << "Total files     |  " << project_analyzer.m_total_files_processed << std::endl;
	os << "-------------------------" << std::endl << std::endl;

	std::vector<FileStatistic> file_statistic = project_analyzer.get_files_statistics();
	for (std::size_t i = 0; i < file_statistic.size(); ++i)
	{
		//os << "File: \"" << project_analyzer.m_project_filenames.front() << "\"" << std::endl;
		os << file_statistic[i] << std::endl;
	}

	return os;
}
