#include <iostream>
#include <limits>
#include <algorithm>
#include <execution>
#include "GaussNewton.h"
#include "../DataManagement/HDPoM.h"
#include "../DataManagement/ResultsWriter.h"

GaussNewton::Settings::Settings(const std::array<double, NUMBER_FUNCTIONS> _alphas, const double _stepDerivative, const double _maxStep,
	const double _minValueFunctional, const unsigned int _maxIterations, const unsigned int _maxIterationsFunctionalNotReduced,
	const double _desiredVolumeOilProduced) :
	alphas(_alphas), stepDerivative(_stepDerivative), maxStep(_maxStep), minValueFunctional(_minValueFunctional),
	maxIterations(_maxIterations), maxIterationsFunctionalNotReduced(_maxIterationsFunctionalNotReduced), desiredVolumeOilProduced(_desiredVolumeOilProduced)
{
	if (alphas[0] <= 0 || alphas[1] <= 0)
		throw std::runtime_error("one of alphas < 0!");
	if (stepDerivative <= 0)
		throw std::runtime_error("step derivative < 0!");
	if (maxStep <= 0)
		throw std::runtime_error("max step < 0!");
	if (minValueFunctional <= 0)
		throw std::runtime_error("min value functional < 0!");
	if (desiredVolumeOilProduced <= 0)
		throw std::runtime_error("desired volume oil produced < 0!");
}

GaussNewton::WellParameters::WellParameters(std::vector<std::string> _types, std::vector<std::string> _movementStates,
	Eigen::VectorXd _coordinates, std::vector<std::pair<double, double>> _allowedValues) :
	coordinates(std::move(_coordinates)), allowedValues(std::move(_allowedValues))
{
	for (const auto& currentAllowedValues : allowedValues)
		if (currentAllowedValues.first > currentAllowedValues.second)
			throw std::runtime_error("incorrent range of allowed values for well!");

	for (size_t i = 0; i < coordinates.size(); i++)
		if (coordinates[i] < allowedValues[i].first || coordinates[i] > allowedValues[i].second)
			throw std::runtime_error("well coordinates do not fall within the allowed values!");

	types.reserve(_types.size());
	for (const auto& currentType : _types)
	{
		if (currentType == "Production")
			types.emplace_back(EType::Production);
		else if (currentType == "Injection")
			types.emplace_back(EType::Injection);
		else
		{
			throw std::runtime_error("incorrent type of well!");
		}
	}

	movementStates.reserve(_movementStates.size());
	for (const auto& currentMovementState : _movementStates)
	{
		if (currentMovementState == "Disable")
			movementStates.emplace_back(EMovementState::Disable);
		else if (currentMovementState == "Enable")
			movementStates.emplace_back(EMovementState::Enable);
		else
		{
			throw std::runtime_error("incorrent movement state of well!");
		}
	}
}

GaussNewton::GaussNewton(const FieldGrid& fieldGrid, Settings methodSettings, WellParameters wellsParameters) :
	m_fieldGrid(fieldGrid), m_methodSettings(std::move(methodSettings)), m_wellParameters(std::move(wellsParameters))
{
	m_numberWells = m_wellParameters.types.size();
}

void GaussNewton::run()
{
	std::vector<std::vector<double>> valuesFunctions;
	double functional = 0.0;
	unsigned int numberIteration = 0;
	double minFunctional = std::numeric_limits<double>::infinity();
	unsigned int numberIterationsFunctionalNotReduced = 0;

	m_fieldGrid.aligningValuesToGrid(m_wellParameters.allowedValues);
	ResultsWriter::writeAllowedValues(m_wellParameters.allowedValues);
	
	while (((std::abs((functional = calculateResidualFunctional(valuesFunctions = calculateFunctions(m_wellParameters.coordinates, 0))))) > m_methodSettings.minValueFunctional) &&
		numberIteration++ < m_methodSettings.maxIterations)
	{			
		if (functional < minFunctional)
		{
			minFunctional = functional;
			numberIterationsFunctionalNotReduced = 0;
		}
		else if (++numberIterationsFunctionalNotReduced >= m_methodSettings.maxIterationsFunctionalNotReduced) break;
			
		ResultsWriter::writeIterData({ numberIteration, functional, calculateProduction(valuesFunctions), m_wellParameters.types, m_wellParameters.coordinates });
		m_wellParameters.coordinates += calculateParameterSteps(assemblySLAE(valuesFunctions, calculateFunctionDerivatives(valuesFunctions)), calculateNewAllowedValues());
	} 
	ResultsWriter::writeIterData({ numberIteration, functional, calculateProduction(valuesFunctions), m_wellParameters.types, m_wellParameters.coordinates });
}

Eigen::VectorXd GaussNewton::calculateParameterSteps(const DenseSLAE& slae, const std::vector<std::pair<double, double>>& newAllowedValues) const
{	
	Eigen::VectorXd regularization = Eigen::VectorXd::Constant(m_wellParameters.coordinates.size(), 1e-9);
	//коэффициент роста коэффициента регул€ризации
	double k = 2;
	bool needIter;	
	Eigen::VectorXd step(m_wellParameters.coordinates.size());

	do
	{
		needIter = false;
		step = slae.solve(slae.getMatrix() + static_cast<Eigen::MatrixXd>(regularization.asDiagonal()), slae.getRightSide());
		for (int i = 0; i < m_wellParameters.coordinates.size(); i++)
			if (!(std::abs(step(i)) < m_methodSettings.maxStep && newAllowedValues[i].first < (m_wellParameters.coordinates[i] + step(i)) &&
				(m_wellParameters.coordinates[i] + step(i)) < newAllowedValues[i].second))
			{
				regularization[i] *= k;
				needIter = true;
			}				
	} while (needIter);

	return step;
}

std::vector<std::pair<double, double>> GaussNewton::calculateNewAllowedValues() const
{
	std::vector<std::pair<double, double>> newAllowedValues = m_wellParameters.allowedValues;

	std::multimap<int, int> intersectionDomains;
	for (int i = 0; i < newAllowedValues.size(); i++)
		findIntersectionDomains(newAllowedValues, 2 * i, intersectionDomains);

	while (!intersectionDomains.empty())
	{			
		int i = intersectionDomains.begin()->first;
		int j = intersectionDomains.begin()->second;

		int k_1 = binarySearchBetween(m_fieldGrid.getGridX(), m_wellParameters.coordinates[2 * i], 0, m_fieldGrid.getGridX().size() - 1);
		int k_2 = binarySearchBetween(m_fieldGrid.getGridX(), m_wellParameters.coordinates[2 * j], 0, m_fieldGrid.getGridX().size() - 1);
		int n = 0;
		if (k_2 == k_1)
		{
			n = 1;
			k_1 = binarySearchBetween(m_fieldGrid.getGridY(), m_wellParameters.coordinates[2 * i + 1], 0, m_fieldGrid.getGridY().size() - 1);
			k_2 = binarySearchBetween(m_fieldGrid.getGridY(), m_wellParameters.coordinates[2 * j + 1], 0, m_fieldGrid.getGridY().size() - 1);
		}

		std::vector<std::pair<double, double>> centerBetweenWells{ {(m_wellParameters.coordinates[2 * i] + m_wellParameters.coordinates[2 * j]) / 2.0, 
			(m_wellParameters.coordinates[2 * i + 1] + m_wellParameters.coordinates[2 * j + 1]) / 2.0} };

		m_fieldGrid.aligningValuesToGrid(centerBetweenWells);

		std::array<int, 2> numbers{ i, j };
		if (k_2 < k_1) std::swap(numbers[0], numbers[1]);

		double center = (n) ? centerBetweenWells[0].second : centerBetweenWells[0].first;

		for (int k = 0; k < numbers.size(); k++)
		{
			std::vector<double> borders{ newAllowedValues[2 * numbers[k] + n].first, newAllowedValues[2 * numbers[k] + n].second };
			int l = binarySearchBetween(borders, center, 0, borders.size() - 1);
			borders.insert(borders.begin() + (l + 1), center);

			int index = binarySearchBetween(borders, m_wellParameters.coordinates[2 * numbers[k] + n], 0, borders.size() - 1);
			newAllowedValues[2 * numbers[k] + n].first = borders[index];
			newAllowedValues[2 * numbers[k] + n].second = borders[index + 1];
		}
		
		if (intersectionDomains.count(j) > 0)
		{
			intersectionDomains.erase(j);
			findIntersectionDomains(newAllowedValues, 2 * j, intersectionDomains);
		}			
		intersectionDomains.erase(i);
		findIntersectionDomains(newAllowedValues, 2 * i, intersectionDomains);								
	}	

	return newAllowedValues;
}

void GaussNewton::findIntersectionDomains(const std::vector<std::pair<double, double>>& allowedValues, int i, std::multimap<int, int>& intersectionDomains) const
{	
	for (int j = i + 2; j < allowedValues.size(); j += 2)
		if (!(allowedValues[i].first >= allowedValues[j].second || allowedValues[i].second <= allowedValues[j].first
			|| allowedValues[i + 1].second <= allowedValues[j + 1].first || allowedValues[i + 1].first >= allowedValues[j + 1].second))
			intersectionDomains.insert({ i / 2, j / 2 });
							
}

std::pair<double, double> GaussNewton::calculateProduction(const std::vector<std::vector<double>>& valuesFunctions) const
{
	double producedWater = 0.0;
	double producedOil = 0.0;

	for (int w = 0; w < m_wellParameters.types.size(); w++)
		if (m_wellParameters.types[w] == GaussNewton::WellParameters::EType::Production)
		{
			// вычисление количества добытой воды и нефти
			producedWater += valuesFunctions[0][w];
			producedOil += valuesFunctions[1][w] + m_methodSettings.desiredVolumeOilProduced;
		}


	return { producedWater, producedOil };
}

DenseSLAE GaussNewton::assemblySLAE(const std::vector<std::vector<double>>& valuesFunctions, const std::vector<std::vector<std::vector<double>>>& valuesDerivativesFunctions) const
{			
	DenseSLAE slae(m_wellParameters.coordinates.size());

	for (int i = 0; i < m_wellParameters.coordinates.size(); i++)
	{
		for (int j = 0; j <= i; j++)
		{
			slae.setMatrixElement(i, j, 0.0);
			for (int k = 0; k < NUMBER_FUNCTIONS; k++)
			{
				double sum = 0.0;
				for (int w = 0; w < m_numberWells; w++)
					sum += valuesDerivativesFunctions[i][k][w] * valuesDerivativesFunctions[j][k][w];	
				slae.addToMatrixElement(i, j, m_methodSettings.alphas[k] * sum);
			}
			slae.setMatrixElement(j, i, slae.getMatrixElement(i, j));
		}

		slae.setRightSideElement(i, 0.0);

		for (int k = 0; k < NUMBER_FUNCTIONS; k++)
		{
			double sum = 0.0;
			for (int w = 0; w < m_numberWells; w++)
				sum += valuesDerivativesFunctions[i][k][w] * valuesFunctions[k][w];
			slae.addToRightSideElement(i, -m_methodSettings.alphas[k] * sum);
		}
	}

	return slae;
}

double GaussNewton::calculateResidualFunctional(std::vector<std::vector<double>>& valuesFunctions) const 
{
	double result = 0.0;
	for (int k = 0; k < NUMBER_FUNCTIONS; k++)
	{
		double sum = 0.0;
		for (int w = 0; w < m_numberWells; w++)
			sum += valuesFunctions[k][w] * valuesFunctions[k][w];
		result += m_methodSettings.alphas[k] * sum;
	}
	return result;
}

std::vector<std::vector<double>> GaussNewton::calculateFunctions(const Eigen::VectorXd& coordinates, const unsigned int modelNumber) const
{
	HDPoM::repositioningWells(m_fieldGrid.findElementNumbers(coordinates), modelNumber);
	HDPoM::run(modelNumber);
		
	std::vector<std::vector<double>> valuesFunctions(NUMBER_FUNCTIONS, std::vector<double>(m_numberWells));
	for (int w = 0; w < m_numberWells; w++)
		if (m_wellParameters.types[w] == WellParameters::EType::Production)
		{
			//вычисление количества добытой воды (фаза 1)
			valuesFunctions[0][w] = HDPoM::calculateSumPhase(modelNumber, w + 1, 1);

			//вычисление количества добытой нефти (фаза 2)
			valuesFunctions[1][w] = HDPoM::calculateSumPhase(modelNumber, w + 1, 2) - m_methodSettings.desiredVolumeOilProduced;
		}
	return valuesFunctions;
}

std::vector<std::vector<std::vector<double>>> GaussNewton::calculateFunctionDerivatives(const std::vector<std::vector<double>>& valuesFunctions) const 
{	
	std::vector<std::vector<std::vector<double>>> valuesDerivativesFunctions(m_wellParameters.coordinates.size(), std::vector<std::vector<double>>(NUMBER_FUNCTIONS, std::vector<double>(m_numberWells)));

	std::vector<int> inds;
	for (int i = 0; i < m_wellParameters.movementStates.size(); i++)	
		if (m_wellParameters.movementStates[i] == WellParameters::EMovementState::Enable)
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
			Eigen::VectorXd coordinates = m_wellParameters.coordinates;
			coordinates[i] += m_methodSettings.stepDerivative;
			std::vector<std::vector<double>> temp = calculateFunctions(coordinates, i);

			for (int k = 0; k < NUMBER_FUNCTIONS; k++)
				for (int w = 0; w < m_numberWells; w++)
					valuesDerivativesFunctions[i][k][w] = (temp[k][w] - valuesFunctions[k][w]) / m_methodSettings.stepDerivative;
		}
	);

	return valuesDerivativesFunctions;
}