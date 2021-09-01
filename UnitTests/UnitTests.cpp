#include "pch.h"
#include "../CountProjectCodeLines/ProjectAnalyzer.h"
#include "../CountProjectCodeLines/FileAnalyzer.h"
#include "../CountProjectCodeLines/FileStatistic.h"
#include "../CountProjectCodeLines/Timer.h"

TEST(ProjectAnalyzerTest, ChangeProjectDirectory) {
	ProjectAnalyzer project_analyzer(".");

	project_analyzer.set_project_root_dir("../CountProjectCodeLines/");

	ASSERT_STREQ(project_analyzer.get_project_root_dir().c_str(), "../CountProjectCodeLines/");
}

TEST(ProjectAnalyzerTest, DirectoryAnalyze) {
	ProjectAnalyzer project_analyzer("../UnitTests/");

	project_analyzer.Analyze();

	ASSERT_STREQ(project_analyzer.get_project_root_dir().c_str(), "../UnitTests/");
	ASSERT_EQ(project_analyzer.get_total_files_processed(), 4);
	ASSERT_FALSE(project_analyzer.get_statistics_of_files().empty());
}

TEST(ProjectAnalyzerTest, ResetStatistic) {
	ProjectAnalyzer project_analyzer("../UnitTests/");

	project_analyzer.Analyze();
	project_analyzer.Reset();

	ASSERT_EQ(project_analyzer.get_total_files_processed(), 0);
	ASSERT_EQ(project_analyzer.get_total_files_statistic(), FileStatistic());
	ASSERT_TRUE(project_analyzer.get_statistics_of_files().empty());
}

TEST(FileAnalyzerTest, AnalyzeEquality) {
	std::queue<std::string> filenames;
	std::forward_list<FileStatistic> statistics_of_files;
	bool file_searching_ended = true;
	FileAnalyzer file_analyzer(filenames, std::mutex(), std::condition_variable(), file_searching_ended, statistics_of_files);

	filenames.push("ForTest.cpp");
	file_analyzer.Analyze();

	ASSERT_EQ(file_analyzer.get_file_statistic(), statistics_of_files.front());
}

TEST(FileAnalyzerTest, FileAnalyze) {
	std::queue<std::string> filenames;
	std::forward_list<FileStatistic> statistics_of_files;
	bool file_searching_ended = true;
	FileAnalyzer file_analyzer(filenames, std::mutex(), std::condition_variable(), file_searching_ended, statistics_of_files);

	filenames.push("ForTest.cpp");
	file_analyzer.Analyze();

	ASSERT_STREQ(file_analyzer.get_file_statistic().get_filename().c_str(), "ForTest.cpp");
	ASSERT_EQ(file_analyzer.get_file_statistic().get_blank_lines_count(), 3);
	ASSERT_EQ(file_analyzer.get_file_statistic().get_comment_lines_count(), 15);
	ASSERT_EQ(file_analyzer.get_file_statistic().get_code_lines_count(), 6);
	ASSERT_EQ(file_analyzer.get_file_statistic().get_physical_lines_count(), 20);
}

TEST(FileStatisticTest, DefaultConstructor) {
	FileStatistic file_statistic;

	ASSERT_STREQ(file_statistic.get_filename().c_str(), "");
	ASSERT_EQ(file_statistic.get_blank_lines_count(), 0);
	ASSERT_EQ(file_statistic.get_comment_lines_count(), 0);
	ASSERT_EQ(file_statistic.get_code_lines_count(), 0);
	ASSERT_EQ(file_statistic.get_physical_lines_count(), 0);
}

TEST(FileStatisticTest, ParameterizedConstructor) {
	FileStatistic file_statistic("/path/to/file");

	ASSERT_STREQ(file_statistic.get_filename().c_str(), "/path/to/file");
	ASSERT_EQ(file_statistic.get_blank_lines_count(), 0);
	ASSERT_EQ(file_statistic.get_comment_lines_count(), 0);
	ASSERT_EQ(file_statistic.get_code_lines_count(), 0);
	ASSERT_EQ(file_statistic.get_physical_lines_count(), 0);
}

TEST(FileStatisticTest, IncrementBlankLines) {
	FileStatistic file_statistic;

	file_statistic.increment_blank_lines_count();
	file_statistic.increment_blank_lines_count();
	file_statistic.increment_blank_lines_count();
	file_statistic.increment_blank_lines_count();

	ASSERT_EQ(file_statistic.get_blank_lines_count(), 4);
}

TEST(FileStatisticTest, ResetStatistic) {
	FileStatistic file_statistic("/path/to/file");

	file_statistic.increment_blank_lines_count();
	file_statistic.increment_comment_lines_count();
	file_statistic.increment_code_lines_count();
	file_statistic.increment_physical_lines_count();

	file_statistic.Reset();

	ASSERT_STREQ(file_statistic.get_filename().c_str(), "");
	ASSERT_EQ(file_statistic.get_blank_lines_count(), 0);
	ASSERT_EQ(file_statistic.get_comment_lines_count(), 0);
	ASSERT_EQ(file_statistic.get_code_lines_count(), 0);
	ASSERT_EQ(file_statistic.get_physical_lines_count(), 0);
}

TEST(TimerTest, ZeroMSDuration) {
	Timer timer;

	timer.Start();
	timer.Stop();
	std::ostringstream oss;
	oss << timer;

	ASSERT_STREQ(oss.str().c_str(), "0");
}

std::int32_t main(std::int32_t argc, char** const argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}