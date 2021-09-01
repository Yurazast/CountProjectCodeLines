#include <iostream>

#include "ProjectAnalyzer.h"

int main()
{
	try
	{
		std::cout << "Enter absolute path to project root directory: ";
		std::string project_root_dir;
		std::cin >> project_root_dir;
		std::cout << std::endl;

		ProjectAnalyzer project_analyzer(project_root_dir);
		project_analyzer.Analyze();
		std::cout << project_analyzer;

		std::cout << "\nEnter absolute path to output file: ";
		std::string output_filename;
		std::cin >> output_filename;

		std::ofstream output_file(output_filename, std::ios::out | std::ios::trunc);
		if (!output_file.is_open())
			throw std::runtime_error("Cannot open file: \"" + output_filename + "\"");
			
		output_file << project_analyzer;
		output_file.close();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	catch (...)
	{
		std::cerr << "Unexpected error!" << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
