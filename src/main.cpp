#include "InverseProblem/GaussNewton.h"
#include "InverseProblem/FieldGrid.h"

#include "DataManagement/ResultsWriter.h"
#include "DataManagement/HDPoM.h"
#include "DataManagement/ConfigureReader.h"
#include "DataManagement/fileSystemFunctions.h"

#include "System/Timer.h"

#include "Windows.h"

int main(int argc, char** argv)
{
	std::string executablePath = getFileDirectory(argv[0]);
	ConfigureReader::setExecutablePath(executablePath);
	HDPoM::setExecutablePath(executablePath);
	ResultsWriter::setExecutablePath(executablePath);

	try
	{
		HDPoM::init(ConfigureReader::readFieldSettings("res/input/fieldSettings.json"));
		ResultsWriter::init(
			{
				"res/output/residual functional.txt",
				"res/output/well centers.txt",
				"res/output/allowed values.txt",
				"res/output/production",
				"res/output/production by day"
			}
		);
		FieldGrid fieldGrid(HDPoM::readFieldGrid());
		GaussNewton gnm(fieldGrid,
			ConfigureReader::readGaussNewtonSettings("res/input/gaussNewtonSettings.json"),
			ConfigureReader::readWellParameters("res/input/well parameters.json"));

		Timer timer;
		gnm.run();
		auto duration = timer.elapsed();

		std::cout << "Time: " << duration.minutes() << " " << duration.seconds() << std::endl;

		system(("Python \"" + executablePath + "/visualization/ResidualFunctional.py\"").c_str());
		system(("Python \"" + executablePath + "/visualization/Phases.py\"").c_str());
		system(("Python \"" + executablePath + "/visualization/Visualize.py\"").c_str());

	}
	catch (const std::ios_base::failure& e)
	{
		std::cerr << "ios_base::failure: " << e.what() << '\n'
			<< "Error code: " << e.code() << '\n';
	}
	catch (std::filesystem::filesystem_error const& e)
	{
		std::cerr << "filesystem::filesystem_error: " << e.what() << '\n'
			<< "path1: " << e.path1() << '\n'
			<< "path2: " << e.path2() << '\n'
			<< "code message:  " << e.code().message() << '\n';
	}
	catch (const std::exception& e)
	{
		std::cerr << "standard exception: " << e.what() << '\n';
	}
	catch (...)
	{
		std::cerr << "abnormal termination!\n";
	}

	HDPoM::terminate();

	return 0;
}