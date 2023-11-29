#pragma once
#include <vector>
#include <array>
#include <string>
#include "MatrixVector.h"

class GridData
{
private:
	std::string m_nameModel;
	double m_simulationTime;
	std::array<Vector, 2> m_grid;

public:

	void ReadGrid();
	std::string GetNameModel() const { return m_nameModel; }
	int FindElemNumForPoint(const std::array<double, 2>& point) const;
	const std::array<Vector, 2>& GetGrid() const { return m_grid; }
	void AligningValuesToGrid(std::vector<std::array<double, 2>>& values) const;
	double GetSimulationTime() { return m_simulationTime; }
};