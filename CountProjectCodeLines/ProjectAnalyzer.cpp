#include "ProjectAnalyzer.h"

const std::string ProjectAnalyzer::FILE_EXTENSION_NAMES[] {
	".cpp", ".c", ".h", ".hpp"
};

ProjectAnalyzer::ProjectAnalyzer(const std::string& project_root_dir)
		: m_project_root_dir(project_root_dir)
		, m_project_filenames{}
		, m_statistics_of_files{}
		, m_total_files_statistic()
		, m_total_files_processed(0)
		, m_mutex()
		, m_cond_var()
		, m_timer()
{
	if (!std::filesystem::exists(m_project_root_dir))
		throw std::runtime_error("Directory \"" + project_root_dir + "\" doesn't exist");

	CreateFileAnalyzers();
	InitThreadPool();
}

ProjectAnalyzer::~ProjectAnalyzer()
{
	JoinThreads();
	DestroyFileAnalyzers();
}

void ProjectAnalyzer::Analyze()
{
	if (!m_thread_pool.empty())
		Reset();

	m_timer.Start();
	SearchFiles();
	JoinThreads();
	CalculateTotalStatistic();
	m_timer.Stop();
}

void ProjectAnalyzer::Reset()
{
	m_project_filenames = std::queue<std::string>();
	JoinThreads();
	m_thread_pool.clear();
	DestroyFileAnalyzers();
	m_statistics_of_files.clear();
	m_total_files_statistic.Reset();
	m_total_files_processed = 0;
	m_timer.Reset();
	CreateFileAnalyzers();
	InitThreadPool();
}

std::string ProjectAnalyzer::get_project_root_dir() const
{
	return m_project_root_dir;
}

void ProjectAnalyzer::set_project_root_dir(const std::string& project_root_dir)
{
	Reset();
	this->m_project_root_dir = project_root_dir;
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

void ProjectAnalyzer::CalculateTotalStatistic()
{
	for (const FileStatistic& file_statistic : m_statistics_of_files)
	{
		m_total_files_statistic += file_statistic;
		++m_total_files_processed;
	}
}

void ProjectAnalyzer::CreateFileAnalyzers()
{
	m_file_analyzers = std::vector<std::unique_ptr<FileAnalyzer>>(std::thread::hardware_concurrency());
	for (std::size_t i = 0; i < m_file_analyzers.size(); ++i)
	{
		m_file_analyzers[i] = std::make_unique<FileAnalyzer>(m_project_filenames, m_mutex, m_cond_var, m_statistics_of_files);
	}
}

void ProjectAnalyzer::DestroyFileAnalyzers()
{
	for (std::unique_ptr<FileAnalyzer>& file_analyzer : m_file_analyzers)
	{
		file_analyzer.reset();
	}
	m_file_analyzers.clear();
}

void ProjectAnalyzer::InitThreadPool()
{
	m_thread_pool = std::vector<std::thread>(std::thread::hardware_concurrency());
	for (std::size_t i = 0; i < m_thread_pool.size(); ++i)
	{
		m_thread_pool[i] = std::thread{ &FileAnalyzer::Analyze, std::ref(*m_file_analyzers[i]) };
	}
}

void ProjectAnalyzer::JoinThreads()
{
	for (std::vector<std::thread>::iterator it = m_thread_pool.begin(); it != m_thread_pool.end(); ++it)
	{
		if (it->joinable())
			it->join();
	}
}

std::ostream& operator<<(std::ostream& os, const ProjectAnalyzer& project_analyzer)
{
	os << "----------------------------------------" << std::endl;
	os.imbue(std::locale(""));
	FileStatistic file_statistic = project_analyzer.m_total_files_statistic;
	os << "| Total blank lines     |  " << std::setw(FIELD_WIDTH) << file_statistic.get_blank_lines_count() << "  |" << std::endl;
	os << "| Total comment lines   |  " << std::setw(FIELD_WIDTH) << file_statistic.get_comment_lines_count() << "  |" << std::endl;
	os << "| Total code lines      |  " << std::setw(FIELD_WIDTH) << file_statistic.get_code_lines_count() << "  |" << std::endl;
	os << "| Total physical lines  |  " << std::setw(FIELD_WIDTH) << file_statistic.get_physical_lines_count() << "  |" << std::endl;
	os << "| Total files processed |  " << std::setw(FIELD_WIDTH) << project_analyzer.m_total_files_processed << "  |" << std::endl;
	os << "| Execution time (ms)   |  " << std::setw(FIELD_WIDTH) << project_analyzer.m_timer << "  |" << std::endl;
	os << "----------------------------------------" << std::endl;

	std::forward_list<FileStatistic> statistics_of_files = project_analyzer.m_statistics_of_files;
	for (const FileStatistic& file_statistic : statistics_of_files)
	{
		os << file_statistic;
	}

	return os;
}
