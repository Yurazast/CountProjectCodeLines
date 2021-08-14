#include "FileStatistic.h"

FileStatistic::FileStatistic()
		: m_filename("")
		, m_blank_lines_count(0)
		, m_comment_lines_count(0)
		, m_code_lines_count(0)
		, m_physical_lines_count(0)
{}

FileStatistic::FileStatistic(const std::string& filename)
		: m_filename(filename)
		, m_blank_lines_count(0)
		, m_comment_lines_count(0)
		, m_code_lines_count(0)
		, m_physical_lines_count(0)
{}

void FileStatistic::Reset()
{
	m_filename = "";
	m_blank_lines_count = 0;
	m_code_lines_count = 0;
	m_comment_lines_count = 0;
	m_physical_lines_count = 0;
}

std::string FileStatistic::get_filename() const
{
	return this->m_filename;
}

std::size_t FileStatistic::get_blank_lines_count() const
{
	return this->m_blank_lines_count;
}

std::size_t FileStatistic::get_comment_lines_count() const
{
	return this->m_comment_lines_count;
}

std::size_t FileStatistic::get_code_lines_count() const
{
	return this->m_code_lines_count;
}

std::size_t FileStatistic::get_physical_lines_count() const
{
	return this->m_physical_lines_count;
}

void FileStatistic::increment_blank_lines_count() noexcept
{
	++this->m_blank_lines_count;
}

void FileStatistic::increment_comment_lines_count() noexcept
{
	++this->m_comment_lines_count;
}

void FileStatistic::increment_code_lines_count() noexcept
{
	++this->m_code_lines_count;
}

void FileStatistic::increment_physical_lines_count() noexcept
{
	++this->m_physical_lines_count;
}

FileStatistic& FileStatistic::operator+=(const FileStatistic& file_statistic)
{
	this->m_blank_lines_count += file_statistic.get_blank_lines_count();
	this->m_comment_lines_count += file_statistic.get_comment_lines_count();
	this->m_code_lines_count += file_statistic.get_code_lines_count();
	this->m_physical_lines_count += file_statistic.get_physical_lines_count();
	return *this;
}

std::ostream& operator<<(std::ostream& os, const FileStatistic& file_statistic)
{
	os << "\nFile: \"" << file_statistic.m_filename << "\"" << std::endl;
	os << "Blank lines     |  " << file_statistic.m_blank_lines_count << std::endl;
	os << "Comment lines   |  " << file_statistic.m_comment_lines_count << std::endl;
	os << "Code lines      |  " << file_statistic.m_code_lines_count << std::endl;
	os << "Physical lines  |  " << file_statistic.m_physical_lines_count;
	return os;
}
