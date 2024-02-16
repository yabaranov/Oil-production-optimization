#include "ResultsWriter.h"

#include "HDPoM.h"
#include "fileSystemFunctions.h"

std::string ResultsWriter::m_executablePath;
ResultsWriter::Paths ResultsWriter::m_paths;

void ResultsWriter::init(const Paths& paths)
{
	m_paths = paths;

	deleteFilesFromDirectory(m_executablePath + "/" + m_paths.productionByDay);

	clearTextFile(m_executablePath + "/" + m_paths.residualFunctional);
	clearTextFile(m_executablePath + "/" + m_paths.production + " water.txt");
	clearTextFile(m_executablePath + "/" + m_paths.production + " oil.txt");
	clearTextFile(m_executablePath + "/" + m_paths.wellsCenters);
}


void ResultsWriter::writeAllowedValues(const std::vector<std::pair<double, double>>& allowedValues)
{
	std::ofstream textFile(m_executablePath + "/" + m_paths.allowedValues);
	for (int i = 0; i < allowedValues.size(); i++)
		textFile << allowedValues[i].first << " " << allowedValues[i].second << std::endl;
}

void ResultsWriter::writeIterData(const GaussNewton::IterData& iterData)
{
	writeIterDataToConsole(iterData.numberIteration, iterData.functional, iterData.coordinates);
	writeProduction(iterData.numberIteration, iterData.production);
	writeProductionByDay(iterData.numberIteration, iterData.types);
	writeResidualFunctional(iterData.numberIteration, iterData.functional);
	writeWellCoordinates(iterData.coordinates);
}

void ResultsWriter::writeProductionByDay(const unsigned int numberIteration, const std::vector<GaussNewton::WellParameters::EType>& types)
{
	for (int w = 0; w < types.size(); w++)
		if (types[w] == GaussNewton::WellParameters::EType::Production)
		{
			// вывод количества добытой воды по дням по отдельной скважине
			std::ofstream fileProducedWaterByDay(m_executablePath + "/" + m_paths.productionByDay + "/production water well" + std::to_string(w + 1) + " iter" + std::to_string(numberIteration) + ".txt");
			HDPoM::calculatePhaseByDay(w + 1, 1, fileProducedWaterByDay);
			
			// вывод количества добытой нефти по дням по отдельной скважине
			std::ofstream fileProducedOilByDay(m_executablePath + "/" + m_paths.productionByDay + "/production oil well" + std::to_string(w + 1) + " iter" + std::to_string(numberIteration) + ".txt");
			HDPoM::calculatePhaseByDay(w + 1, 2, fileProducedOilByDay);
		}
}

void ResultsWriter::writeProduction(const unsigned int numberIteration, const std::pair<double, double> production)
{
	// вывод количества добытой воды по всем скважинам
	std::ofstream fileProducedWater(m_executablePath + "/" + m_paths.production + " water.txt", std::ios::app);

	fileProducedWater << numberIteration << " " << production.first << std::endl;

	// вывод количества добытой нефти по всем скважинам
	std::ofstream fileProducedOil(m_executablePath + "/" + m_paths.production + " oil.txt", std::ios::app);
	fileProducedOil << numberIteration << " " << production.second << std::endl;
}

void ResultsWriter::writeResidualFunctional(const unsigned int numberIteration, const double functional)
{
	// вывод значений минимизируемого функционала по итерациям
	std::ofstream fileResidualFuctional(m_executablePath + "/" + m_paths.residualFunctional, std::ios::app);
	fileResidualFuctional << numberIteration << " " << std::fixed << std::setw(10) << std::setprecision(3) << functional << std::endl;
}

void ResultsWriter::writeWellCoordinates(const Eigen::VectorXd& coordinates)
{
	// вывод центров скважин в файл
	std::ofstream fileWellCenters(m_executablePath + "/" + m_paths.wellsCenters, std::ios::app);
	for (int i = 0; i < coordinates.size(); i += 2)
		fileWellCenters << std::fixed << std::setw(10) << std::setprecision(3) << coordinates[i]
		<< std::fixed << std::setw(10) << std::setprecision(3) << coordinates[i + 1] << std::endl;
}

void ResultsWriter::writeIterDataToConsole(const int numberIteration, const double functional, const Eigen::VectorXd& coordinates)
{
	std::cout << "Residual functional: " << std::fixed << std::setw(10) << std::setprecision(3) << functional << std::endl;
	std::cout << "Number iteration: " << numberIteration << std::endl;
	for (int i = 0; i < coordinates.size(); i += 2)
		std::cout << "Center well" + std::to_string(i / 2 + 1) << ": " << std::fixed << std::setw(10) << std::setprecision(3) << coordinates[i]
		<< std::fixed << std::setw(10) << std::setprecision(3) << coordinates[i + 1] << std::endl;
	std::cout << std::endl;
}