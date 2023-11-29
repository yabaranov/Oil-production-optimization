#pragma once
#include <array>
#include <iostream>
#include <map>
#include "DenseSLAE.h"
#include "GridData.h"

class GNM
{
private:

	Vector m_Q;
	double m_minValueResidualFunctional;
	double m_stepDerivative;
	double m_maxStepQ;
	int m_maxIterations;
	int m_maxIterationsResidualFunctionalNotReduced;
	int m_numberFunctions;

	std::vector<std::array<double, 2>> m_allowedValuesQ;
	std::vector<std::pair<bool, bool>> m_typesWells;
	double m_desiredVolumeOilProduced;
	std::array<double, 2> m_alpha;
	int m_numberWells;

	DenseSLAE m_slau;
	const GridData& m_gridData;

	void CalculateFunctions(const Vector& Q, const std::string& nameModel, std::vector<Vector>& valuesFunctions);
	void CalculateDerivativesFunctions(const std::vector<Vector>& valuesFunctions, std::vector<std::vector<Vector>>& valuesDerivativesFunctions);
	double ResidualFunctional(std::vector<Vector>& valuesFunctions);
	void AssemblySLAU(const std::vector<Vector>& valuesFunctions);
	void ComputingNewValuesParameters(const std::vector<std::array<double, 2>>& dynamicAllowedValuesQ);
	void RepositioningWells(const Vector& Q, const std::string& nameModel);
	void CalculateDynamicAllowedValuesQ(std::vector<std::array<double, 2>>& dynamicAllowedValuesQ);
	void FindIntersectionDomains(std::vector<std::array<double, 2>>& allowedValuesQ, int i, std::multimap<int, int>& intersectionDomains);

	void Output(const std::vector<Vector>& valuesFunctions, double residualFuctional, int numberIteration) const;
	void OutputAllowedValuesQ(const std::string& path, const std::vector<std::array<double, 2>>& allowedValuesQ) const;

public:

	GNM(const GridData& gridData) : m_gridData(gridData) {}
	void IterativeProcess();
	void TestGeometry();
	void ReadData();
	int GetNumberWells() { return m_numberWells; }
	void CleaningOutputFiles();
};
