#include "FileAnalyzer.h"

FileAnalyzer::FileAnalyzer(const std::string& filename, std::mutex& mutex, std::condition_variable& cond_var)
		: m_input_filename(filename)
		, m_input_file(filename, std::ios::in)
		, m_file_statistic{}
		, m_mutex(mutex)
		, m_cond_var(cond_var)
{
	if (!m_input_file.is_open())
		throw std::runtime_error("Cannot open file: \"" + filename + "\"");
}

FileAnalyzer::~FileAnalyzer()
{
	m_input_file.close();
}

void FileAnalyzer::Analyze()
{
	std::string line;
	std::size_t not_a_white_space_pos;
	bool is_multi_line_comment = false;

	std::unique_lock<std::mutex> lock(m_mutex);

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

	m_cond_var.notify_one();
}

FileStatistic FileAnalyzer::get_file_statistic() const
{
	return this->m_file_statistic;
}
