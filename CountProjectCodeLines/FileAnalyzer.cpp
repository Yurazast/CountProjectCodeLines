#include "FileAnalyzer.h"

FileAnalyzer::FileAnalyzer(std::queue<std::string>& filenames_queue, std::mutex& mutex, std::condition_variable& cond_var,
		bool& is_file_scanning_ended, std::forward_list<FileStatistic>& statistics_of_files)
		: m_filenames_queue(filenames_queue)
		, m_is_multiline_comment(false)
		, m_mutex(mutex)
		, m_cond_var(cond_var)
		, m_is_file_scanning_ended(is_file_scanning_ended)
		, m_statistics_of_files(statistics_of_files)
{}

void FileAnalyzer::Analyze()
{
	while (true)
	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_cond_var.wait(lock, [this] { return !m_filenames_queue.empty() || m_is_file_scanning_ended; });

		if (m_is_file_scanning_ended && m_filenames_queue.empty())
		{
			m_cond_var.notify_one();
			return;
		}

		const std::string filename = m_filenames_queue.front();
		m_filenames_queue.pop();

		m_input_file.open(filename, std::ios::in);
		if (!m_input_file.is_open())
			throw std::runtime_error("Cannot open file: \"" + filename + "\"");

		m_file_statistic = { filename };
		lock.unlock();

		CountLines();

		lock.lock();
		m_statistics_of_files.emplace_front(this->m_file_statistic);
		lock.unlock();

		m_input_file.close();
	}
}

FileStatistic FileAnalyzer::get_file_statistic() const
{
	return this->m_file_statistic;
}

void FileAnalyzer::CountLines()
{
	std::string line;
	std::size_t not_a_whitespace_pos;

	while (std::getline(m_input_file, line))
	{
		if ((not_a_whitespace_pos = line.find_first_not_of(WHITESPACE)) != std::string::npos)
			line = line.substr(not_a_whitespace_pos);

		if (!m_is_multiline_comment)
		{
			if (not_a_whitespace_pos == std::string::npos)
				m_file_statistic.increment_blank_lines_count();

			else if (line.find("//") != 0 && line.find("/*") != 0)
				m_file_statistic.increment_code_lines_count();

			if (line.find("//") != std::string::npos)
				m_file_statistic.increment_comment_lines_count();
		}

		CheckIfMultilineComment(line);

		m_file_statistic.increment_physical_lines_count();
	}
}

void FileAnalyzer::CheckIfMultilineComment(const std::string& line)
{
	if (!m_is_multiline_comment && line.find("/*") != std::string::npos)
		m_is_multiline_comment = true;

	if (m_is_multiline_comment)
		m_file_statistic.increment_comment_lines_count();

	if (m_is_multiline_comment && line.find("*/") != std::string::npos)
	{
		m_is_multiline_comment = false;
		CheckIfCodeAfterComment(line.substr(line.find("*/") + 2));
	}
}

void FileAnalyzer::CheckIfCodeAfterComment(std::string&& line)
{
	std::size_t not_a_whitespace_pos = line.find_first_not_of(WHITESPACE);
	bool is_code_line = false;

	while (not_a_whitespace_pos != std::string::npos)
	{
		line = line.substr(not_a_whitespace_pos);

		if (line.find("//") == 0)
			return;

		if (line.find("/*") == 0)
		{
			m_is_multiline_comment = true;

			const std::size_t end_multiline_comment_pos = line.find("*/");
			if (end_multiline_comment_pos != std::string::npos)
			{
				m_is_multiline_comment = false;
				line = line.substr(end_multiline_comment_pos + 2);
			}
			else
			{
				break;
			}
		}
		else
		{
			is_code_line = true;

			const std::size_t whitespace_pos = line.find_first_of(WHITESPACE);
			if (whitespace_pos != std::string::npos)
			{
				line = line.substr(whitespace_pos);
			}
			else
			{
				break;
			}
		}

		not_a_whitespace_pos = line.find_first_not_of(WHITESPACE);
	}

	if (is_code_line)
		m_file_statistic.increment_code_lines_count();
}
