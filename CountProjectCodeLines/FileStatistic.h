#pragma once

#include <ostream>

class FileStatistic
{
public:
	FileStatistic();

	void Reset();

	std::size_t get_blank_lines_count() const;
	std::size_t get_comment_lines_count() const;
	std::size_t get_code_lines_count() const;
	std::size_t get_physical_lines_count() const;
	void increment_blank_lines_count() noexcept;
	void increment_comment_lines_count() noexcept;
	void increment_code_lines_count() noexcept;
	void increment_physical_lines_count() noexcept;

	FileStatistic& operator+=(const FileStatistic& file_statistic);

private:
	friend std::ostream& operator<<(std::ostream& os, const FileStatistic& file_statistic);	

	std::size_t m_blank_lines_count;
	std::size_t m_comment_lines_count;
	std::size_t m_code_lines_count;
	std::size_t m_physical_lines_count;
};

