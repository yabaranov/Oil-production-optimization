#include <iomanip>
#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <omp.h>
#include <limits>
#include <algorithm>
#include <execution>
#include "GNM.h"
#include "functions.h"
#include "HDPoM.h"

void GNM::TestGeometry()
{
	m_gridData.AligningValuesToGrid(m_allowedValuesQ);
	OutputAllowedValuesQ("../res/output/Allowed values.txt", m_allowedValuesQ);

	//вывод центров скважин в файл
	std::ofstream fileWellCenters("../res/output/Well centers.txt", std::ios::app);
	for (int i = 0; i < m_Q.size(); i += 2)
		fileWellCenters << std::fixed << std::setw(10) << std::setprecision(3) << m_Q[i]
		<< std::fixed << std::setw(10) << std::setprecision(3) << m_Q[i + 1] << std::endl;
	fileWellCenters.close();

	//WinExec("Python ../src/visualization/GeometryBefore.py", 1);
	std::vector<std::array<double, 2>> dynamicAllowedValuesQ = m_allowedValuesQ;
	CalculateDynamicAllowedValuesQ(dynamicAllowedValuesQ);
	OutputAllowedValuesQ("../res/output/Dynamic allowed values.txt", dynamicAllowedValuesQ);
	//WinExec("Python ../src/visualization/GeometryAfter.py", 1);
}

void GNM::IterativeProcess()
{
	std::vector<Vector> valuesFunctions(m_numberFunctions, Vector(m_numberWells));
	double residualFunctional = 0.0;
	int numberIteration = 0;
	double minResidualFunctional = std::numeric_limits<double>::infinity();
	int numberIterationsResidualFunctionalNotReduced = 0;

	m_slau.GenerateSLAE(m_Q.size());	
	m_gridData.AligningValuesToGrid(m_allowedValuesQ);
	OutputAllowedValuesQ("../res/output/Allowed values.txt", m_allowedValuesQ);
	
	while (((std::abs((residualFunctional = ResidualFunctional(valuesFunctions)))) > m_minValueResidualFunctional) && ++numberIteration < m_maxIterations && numberIterationsResidualFunctionalNotReduced < m_maxIterationsResidualFunctionalNotReduced)
	{			
		if (residualFunctional < minResidualFunctional)
		{
			minResidualFunctional = residualFunctional;
			numberIterationsResidualFunctionalNotReduced = 0;
		}			
		else
			numberIterationsResidualFunctionalNotReduced++;

		Output(valuesFunctions, residualFunctional, numberIteration);
		AssemblySLAU(valuesFunctions);
		std::vector<std::array<double, 2>> dynamicAllowedValuesQ = m_allowedValuesQ;
		CalculateDynamicAllowedValuesQ(dynamicAllowedValuesQ);
		ComputingNewValuesParameters(dynamicAllowedValuesQ);			
	} 

	Output(valuesFunctions, residualFunctional, numberIteration);
}

void GNM::ComputingNewValuesParameters(const std::vector<std::array<double, 2>>& dynamicAllowedValuesQ)
{	
	//ƒиагональна€ матрица коэффициентов регул€ризации
	Matrix gamma(m_slau.Size());
	for (int i = 0; i < gamma.size(); i++)
		gamma[i][i] = 1e-9;

	bool needIter;
	//коэффициент роста коэффициента регул€ризации
	double k = 2;
	do
	{
		needIter = false;
		DenseSLAE::Gauss(m_slau.GetMatrix() + gamma, m_slau.GetRightHand(), m_slau.SetUnknowns());
		
		for (int i = 0; i < m_Q.size(); i++)			
			if (!(std::abs(m_slau.GetUnknownsElement(i)) < m_maxStepQ && dynamicAllowedValuesQ[i][0] < (m_Q[i] + m_slau.GetUnknownsElement(i)) && (m_Q[i] + m_slau.GetUnknownsElement(i)) < dynamicAllowedValuesQ[i][1]))
			{
				gamma[i][i] *= k;
				needIter = true;
			}				
	} while (needIter);

	m_Q += m_slau.GetUnknowns();
}

void GNM::CalculateDynamicAllowedValuesQ(std::vector<std::array<double, 2>>& dynamicAllowedValuesQ)
{
	std::multimap<int, int> intersectionDomains;
	for (int i = 0; i < m_allowedValuesQ.size(); i++)
		FindIntersectionDomains(m_allowedValuesQ, 2 * i, intersectionDomains);

	while (!intersectionDomains.empty())
	{			
		int i = intersectionDomains.begin()->first;
		int j = intersectionDomains.begin()->second;

		int k_1 = binarySearchBetween(m_gridData.GetGrid()[0], m_Q[2 * i], 0, m_gridData.GetGrid()[0].size() - 1);
		int k_2 = binarySearchBetween(m_gridData.GetGrid()[0], m_Q[2 * j], 0, m_gridData.GetGrid()[0].size() - 1);
		int n = 0;
		if (k_2 == k_1)
		{
			n = 1;
			k_1 = binarySearchBetween(m_gridData.GetGrid()[1], m_Q[2 * i + 1], 0, m_gridData.GetGrid()[1].size() - 1);
			k_2 = binarySearchBetween(m_gridData.GetGrid()[1], m_Q[2 * j + 1], 0, m_gridData.GetGrid()[1].size() - 1);
		}

		std::vector<std::array<double, 2>> centerBetweenWells{ {(m_Q[2 * i] + m_Q[2 * j]) / 2.0, (m_Q[2 * i + 1] + m_Q[2 * j + 1]) / 2.0} };
		m_gridData.AligningValuesToGrid(centerBetweenWells);

		std::array<int, 2> numbers{ i, j };
		if (k_2 < k_1) std::swap(numbers[0], numbers[1]);

		double center = centerBetweenWells[0][n];

		for (int k = 0; k < numbers.size(); k++)
		{
			std::vector<double> borders{ dynamicAllowedValuesQ[2 * numbers[k] + n][0], dynamicAllowedValuesQ[2 * numbers[k] + n][1] };
			int l = binarySearchBetween(borders, center, 0, borders.size() - 1);
			borders.insert(borders.begin() + (l + 1), center);

			int index = binarySearchBetween(borders, m_Q[2 * numbers[k] + n], 0, borders.size() - 1);
			dynamicAllowedValuesQ[2 * numbers[k] + n][0] = borders[index];
			dynamicAllowedValuesQ[2 * numbers[k] + n][1] = borders[index + 1];
		}
		
		if (intersectionDomains.count(j) > 0)
		{
			intersectionDomains.erase(j);
			FindIntersectionDomains(dynamicAllowedValuesQ, 2 * j, intersectionDomains);
		}			
		intersectionDomains.erase(i);
		FindIntersectionDomains(dynamicAllowedValuesQ, 2 * i, intersectionDomains);								
	}		
}

void GNM::FindIntersectionDomains(std::vector<std::array<double, 2>>& allowedValuesQ, int i, std::multimap<int, int>& intersectionDomains)
{	
	for (int j = i + 2; j < allowedValuesQ.size(); j += 2)
		if (!(allowedValuesQ[i][0] >= allowedValuesQ[j][1] || allowedValuesQ[i][1] <= allowedValuesQ[j][0]
			|| allowedValuesQ[i + 1][1] <= allowedValuesQ[j + 1][0] || allowedValuesQ[i + 1][0] >= allowedValuesQ[j + 1][1]))			
			intersectionDomains.insert({ i / 2, j / 2 });
							
}

void GNM::ReadData()
{	
	std::ifstream textFile;

	textFile.open("../res/input/param.txt");
	textFile >> m_numberFunctions;
	for (int i = 0; i < m_alpha.size(); i++)
		textFile >> m_alpha[i];
	textFile >> m_stepDerivative;
	textFile >> m_maxStepQ;
	textFile >> m_minValueResidualFunctional;	
	textFile >> m_maxIterations;
	textFile >> m_maxIterationsResidualFunctionalNotReduced;
	textFile >> m_desiredVolumeOilProduced;
	
	textFile.close();

	textFile.open("../res/input/well task parameters.txt");
	textFile >> m_numberWells;
	m_typesWells.resize(m_numberWells);
	m_allowedValuesQ.resize(2 * m_numberWells);
	m_Q.resize(2 * m_numberWells);

	for (int i = 0; i < m_numberWells; i++)
	{
		textFile >> m_typesWells[i].first;
		textFile >> m_typesWells[i].second;

		for (int j = 0; j < 2; j++)		
			textFile >> m_Q[2 * i + j];
			
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < m_allowedValuesQ[2 * i + j].size(); k++)			
				textFile >> m_allowedValuesQ[2 * i + j][k];
								
	}
	textFile.close();

}

void GNM::CleaningOutputFiles()
{
	deleteFilesFromDirectory("../res/output/Produced phase");
	deleteFilesFromDirectory("../res/output/Produced phase by day");

	clearTextFile("../res/output/Residual functional.txt");
	clearTextFile("../res/output/Produced water.txt");
	clearTextFile("../res/output/Produced oil.txt");
	clearTextFile("../res/output/Well centers.txt");
}

void GNM::AssemblySLAU(const std::vector<Vector>& valuesFunctions)
{	
	std::vector<std::vector<Vector>> valuesDerivativesFunctions(m_Q.size(), std::vector<Vector>(m_numberFunctions, Vector(m_numberWells)));
	CalculateDerivativesFunctions(valuesFunctions, valuesDerivativesFunctions);
		
	for (int i = 0; i < m_Q.size(); i++)
	{
		for (int j = 0; j <= i; j++)
		{
			m_slau.SetMatrixElement(i, j) = 0.0;
			for (int k = 0; k < m_numberFunctions; k++)
			{
				double sum = 0.0;
				for (int w = 0; w < m_numberWells; w++)
					sum += valuesDerivativesFunctions[i][k][w] * valuesDerivativesFunctions[j][k][w];
				m_slau.SetMatrixElement(i, j) += m_alpha[k] * sum;

			}
			m_slau.SetMatrixElement(j, i) = m_slau.GetMatrixElement(i, j);
		}

		m_slau.SetRightHandElement(i) = 0.0;
		for (int k = 0; k < m_numberFunctions; k++)
		{
			double sum = 0.0;
			for (int w = 0; w < m_numberWells; w++)
				sum += valuesDerivativesFunctions[i][k][w] * valuesFunctions[k][w];
			m_slau.SetRightHandElement(i) -= m_alpha[k] * sum;

		}
	}
}

double GNM::ResidualFunctional(std::vector<Vector>& valuesFunctions)
{
	CalculateFunctions(m_Q, m_gridData.GetNameModel() + std::to_string(0), valuesFunctions);

	double result = 0.0;
	for (int k = 0; k < m_numberFunctions; k++)
	{
		double sum = 0.0;
		for (int w = 0; w < m_numberWells; w++)
			sum += valuesFunctions[k][w] * valuesFunctions[k][w];
		result += m_alpha[k] * sum;
	}
	return result;
}

void GNM::RepositioningWells(const Vector& Q, const std::string& nameModel)
{
	std::ofstream textFile("../res/models/" + nameModel + "/mesh/WellsSource.txt");
	textFile << m_numberWells << std::endl;

	for (int i = 0; i < Q.size(); i += 2)
	{
		int numElem = m_gridData.FindElemNumForPoint({ Q[i], Q[i + 1] });

		textFile << i / 2 + 1 << ' ' << 1 << std::endl;
		textFile << 1 << ' ' << -2 << ' ' << 1 << std::endl;
		textFile << ++numElem << std::endl;		
	}

	textFile.close();
}

void GNM::CalculateFunctions(const Vector& Q, const std::string& nameModel, std::vector<Vector>& valuesFunctions)
{
	//изменение положени€ скважин
	RepositioningWells(Q, nameModel);
	//запуск пр€мой задачи
	runHDPoM(nameModel);
		
	std::string path = "../res/models/" + nameModel + "/output/";	
	double sum = 0.0;
	for (int w = 0; w < m_numberWells; w++)
		if (m_typesWells[w].first)
		{
			//вычисление количества добытой воды
			sum = calculateSumPhaseFromFileHDPoM(path + "m_Well" + std::to_string(w + 1) + "_s1.txt");
			valuesFunctions[0][w] = sum;

			//вычисление количества добытой нефти
			sum = calculateSumPhaseFromFileHDPoM(path + "m_Well" + std::to_string(w + 1) + "_s2.txt");
			valuesFunctions[1][w] = sum - m_desiredVolumeOilProduced;
		}
	
}

void GNM::CalculateDerivativesFunctions(const std::vector<Vector>& valuesFunctions, std::vector<std::vector<Vector>>& valuesDerivativesFunctions)
{	

	std::vector<int> inds;
	for (int i = 0; i < m_typesWells.size(); i++)	
		if (m_typesWells[i].second)
		{
			inds.push_back(2 * i);
			inds.push_back(2 * i + 1);
		}
	
	std::for_each(
		std::execution::par_unseq,
		inds.begin(),
		inds.end(),
		[&](int i)
		{
			Vector Q = m_Q;
			Q[i] += m_stepDerivative;
			std::vector<Vector> temp(m_numberFunctions, Vector(m_numberWells));
			CalculateFunctions(Q, m_gridData.GetNameModel() + std::to_string(i), temp);

			for (int k = 0; k < m_numberFunctions; k++)
				for (int w = 0; w < m_numberWells; w++)
					valuesDerivativesFunctions[i][k][w] = (temp[k][w] - valuesFunctions[k][w]) / m_stepDerivative;
		}
	);

}

void GNM::Output(const std::vector<Vector>& valuesFunctions, double residualFunctional, int numberIteration) const
{
	//вычисление количества добытой воды и нефти
	std::array<double, 2> sum = { 0.0, 0.0 };
	
	for (int w = 0; w < m_numberWells; w++)
		if (m_typesWells[w].first)
		{
			//вычисление количества добытой воды по отдельной скважине
			std::ofstream fileProducedWater("../res/output/Produced phase/Produced water Well" + std::to_string(w + 1) + ".txt", std::ios::app);
			fileProducedWater << numberIteration << " " << valuesFunctions[0][w] << std::endl;
			fileProducedWater.close();

			//вычисление количества добытой нефти по отдельной скважине
			std::ofstream fileProducedOil("../res/output/Produced phase/Produced oil Well" + std::to_string(w + 1) + ".txt", std::ios::app);
			fileProducedOil << numberIteration << " " << valuesFunctions[1][w] + m_desiredVolumeOilProduced << std::endl;
			fileProducedOil.close();

			//вычисление количества добытой воды по всем скважинам
			sum[0] += valuesFunctions[0][w];

			//вычисление количества добытой нефти по всем скважинам
			sum[1] += valuesFunctions[1][w] + m_desiredVolumeOilProduced;


			std::string path = "../res/models/" + m_gridData.GetNameModel() + std::to_string(0) + "/output/";
			
			//вычисление количества добытой воды по дн€м по отдельной скважине
			std::ofstream fileProducedWaterByDay("../res/output/Produced phase by day/Produced water by day Well" + std::to_string(w + 1) + " iteration" + std::to_string(numberIteration) + ".txt");						
			calculatePhaseByDayFromFileHDPoM(path + "m_Well" + std::to_string(w + 1) + "_s1.txt", fileProducedWaterByDay);
			fileProducedWaterByDay.close();

			//вычисление количества добытой нефти по дн€м по отдельной скважине
			std::ofstream fileProducedOilByDay("../res/output/Produced phase by day/Produced oil by day Well" + std::to_string(w + 1) + " iteration" + std::to_string(numberIteration) + ".txt");
			calculatePhaseByDayFromFileHDPoM(path + "m_Well" + std::to_string(w + 1) + "_s2.txt", fileProducedOilByDay);
			fileProducedOilByDay.close();

		}	

	//вывод количества добытой воды по всем скважинам
	std::ofstream fileProducedWater("../res/output/Produced water.txt", std::ios::app);
	fileProducedWater << numberIteration << " " << sum[0] << std::endl;
	fileProducedWater.close();

	//вывод количества добытой нефти по всем скважинам
	std::ofstream fileProducedOil("../res/output/Produced oil.txt", std::ios::app);
	fileProducedOil << numberIteration << " " << sum[1] << std::endl;
	fileProducedOil.close();

	//вывод значений минимизируемого функционала по итераци€м
	std::ofstream fileResidualFuctional("../res/output/Residual functional.txt", std::ios::app);
	fileResidualFuctional << numberIteration << " " << std::fixed << std::setw(10) << std::setprecision(3) << residualFunctional << std::endl;
	fileResidualFuctional.close();

	//вывод центров скважин в файл
	std::ofstream fileWellCenters("../res/output/Well centers.txt", std::ios::app);	
	for (int i = 0; i < m_Q.size(); i += 2)
		fileWellCenters << std::fixed << std::setw(10) << std::setprecision(3) << m_Q[i]
		<< std::fixed << std::setw(10) << std::setprecision(3) << m_Q[i + 1] << std::endl;
	fileWellCenters.close();

	//вывод информации на консоль
	std::cout << "Residual functional: " << std::fixed << std::setw(10) << std::setprecision(3) << residualFunctional << std::endl;
	std::cout << "Number iteration: " << numberIteration << std::endl;
	for (int i = 0; i < m_Q.size(); i += 2)
		std::cout << "Center well" + std::to_string(i / 2 + 1) << ": " << std::fixed << std::setw(10) << std::setprecision(3) << m_Q[i]
		<< std::fixed << std::setw(10) << std::setprecision(3) << m_Q[i + 1] << std::endl;
	std::cout << std::endl;
}

void GNM::OutputAllowedValuesQ(const std::string& path, const std::vector<std::array<double, 2>>& allowedValuesQ) const
{
	std::ofstream allowedValues(path);	
	for (int i = 0; i < allowedValuesQ.size(); i++)
		allowedValues << allowedValuesQ[i][0] << " " << allowedValuesQ[i][1] << std::endl;
	
	allowedValues.close();
}
