#include "FileAnalyzer.h"

FileAnalyzer::FileAnalyzer(std::queue<std::string>& filenames_queue, std::mutex& mutex, std::condition_variable& cond_var)
		: m_filenames_queue(filenames_queue)
		, m_mutex(mutex)
		, m_cond_var(cond_var)
{}

FileStatistic FileAnalyzer::Analyze()
{
	std::unique_lock<std::mutex> lock(m_mutex);

	if (!m_cond_var.wait_for(lock, std::chrono::seconds(2), [this] { return !m_filenames_queue.empty(); }))
	{
		return FileStatistic();
	}

	std::string filename = m_filenames_queue.front();
	m_filenames_queue.pop();

	m_input_file.open(filename, std::ios::in);
	if (!m_input_file.is_open())
		throw std::runtime_error("Cannot open file: \"" + filename + "\"");

	m_file_statistic = { filename };
	lock.unlock();

	CountLines();

	m_input_file.close();
	return m_file_statistic;
}

FileStatistic FileAnalyzer::get_file_statistic() const
{
	return this->m_file_statistic;
}

void FileAnalyzer::CountLines()
{
	std::string line;
	std::size_t not_a_white_space_pos;
	bool is_multi_line_comment = false;

	while (std::getline(m_input_file, line))
	{
		if ((not_a_white_space_pos = line.find_first_not_of(" \t\r\n\f\v")) != std::string::npos)
			line = line.substr(not_a_white_space_pos);

		if (!is_multi_line_comment)
		{
			if (not_a_white_space_pos == std::string::npos)
				m_file_statistic.increment_blank_lines_count();

			else if (line.find("//") != 0 && line.find("/*") != 0)
				m_file_statistic.increment_code_lines_count();
		}

		if (!is_multi_line_comment && line.find("/*") != std::string::npos)
			is_multi_line_comment = true;

		if (is_multi_line_comment || line.find("//") != std::string::npos)
			m_file_statistic.increment_comment_lines_count();

		if (is_multi_line_comment && line.find("*/") != std::string::npos)
			is_multi_line_comment = false;

		m_file_statistic.increment_physical_lines_count();
	}
}
