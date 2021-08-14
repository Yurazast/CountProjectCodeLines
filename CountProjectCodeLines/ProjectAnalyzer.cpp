#include "ProjectAnalyzer.h"

const std::string ProjectAnalyzer::FILE_EXTENSION_NAMES[] {
	".cpp", ".c", ".h", ".hpp"
};

ProjectAnalyzer::ProjectAnalyzer(const std::string& project_root_dir)
		: m_project_root_dir(project_root_dir)
		, m_project_filenames{}
		, m_file_analyzers(std::thread::hardware_concurrency())
		, m_futures(std::thread::hardware_concurrency())
		, m_statistics_of_files{}
		, m_total_files_statistic()
		, m_total_files_processed(0)
		, m_mutex()
		, m_cond_var()
		, m_timer()
{
	if (!std::filesystem::exists(m_project_root_dir))
		throw std::runtime_error("Directory \"" + project_root_dir + "\" doesn't exist");

	for (std::size_t i = 0; i < m_futures.size(); ++i)
	{
		m_file_analyzers[i] = new FileAnalyzer(m_project_filenames, m_mutex, m_cond_var);
	}

	for (std::size_t i = 0; i < m_futures.size(); ++i)
	{
		m_futures[i] = std::async(&FileAnalyzer::Analyze, m_file_analyzers[i]);
	}
}

ProjectAnalyzer::~ProjectAnalyzer()
{
	for (const FileAnalyzer* file_analyzer : m_file_analyzers)
		delete file_analyzer;
}

void ProjectAnalyzer::Analyze()
{
	m_timer.Start();
	SearchFiles();
	ProcessFiles();
	m_timer.Stop();
}

std::forward_list<FileStatistic> ProjectAnalyzer::get_statistics_of_files() const
{
	return m_statistics_of_files;
}

FileStatistic ProjectAnalyzer::get_total_files_statistic() const
{
	return m_total_files_statistic;
}

std::size_t ProjectAnalyzer::get_total_files_processed() const
{
	return m_total_files_processed;
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
					std::unique_lock<std::mutex> lock(m_mutex);
					m_project_filenames.emplace(dir_entry.path().string());
					lock.unlock();

					m_cond_var.notify_one();
					break;
				}
			}
		}
	}
}

void ProjectAnalyzer::ProcessFiles()
{
	while (!m_project_filenames.empty())
	{
		for (std::size_t i = 0; i < m_futures.size(); ++i)
		{
			GetFileStatisticInFuture(i);

			m_futures[i] = std::async(&FileAnalyzer::Analyze, m_file_analyzers[i]);
		}

		m_cond_var.notify_all();
	}

	for (std::size_t i = 0; i < m_futures.size(); ++i)
	{
		GetFileStatisticInFuture(i);
	}
}

void ProjectAnalyzer::GetFileStatisticInFuture(const std::size_t index)
{
		FileStatistic file_statistic = m_futures[index]._Get_value();
		if (file_statistic.get_filename().empty())
			return;

		m_statistics_of_files.emplace_front(file_statistic);
		m_total_files_statistic += file_statistic;
		++m_total_files_processed;
}

std::ostream& operator<<(std::ostream& os, const ProjectAnalyzer& project_analyzer)
{
	os << "----------------------------------------" << std::endl;
	os.imbue(std::locale(""));
	FileStatistic file_statistic = project_analyzer.m_total_files_statistic;
	os << "| Total blank lines     |  " << std::setw(10) << file_statistic.get_blank_lines_count() << "  |" << std::endl;
	os << "| Total comment lines   |  " << std::setw(10) << file_statistic.get_comment_lines_count() << "  |" << std::endl;
	os << "| Total code lines      |  " << std::setw(10) << file_statistic.get_code_lines_count() << "  |" << std::endl;
	os << "| Total physical lines  |  " << std::setw(10) << file_statistic.get_physical_lines_count() << "  |" << std::endl;
	os << "| Total files processed |  " << std::setw(10) << project_analyzer.m_total_files_processed << "  |" << std::endl;
	os << "| Execution time (ms)   |  " << std::setw(10) << project_analyzer.m_timer << "  |" << std::endl;
	os << "----------------------------------------" << std::endl;

	std::forward_list<FileStatistic> statistics_of_files = project_analyzer.m_statistics_of_files;
	for (const FileStatistic& file_statistic : statistics_of_files)
	{
		os << file_statistic << std::endl;
	}

	return os;
}
