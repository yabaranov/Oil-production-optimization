#pragma once 

#include "../InverseProblem/GaussNewton.h"

class ResultsWriter
{
public:

	struct Paths
	{		
		Paths(const std::string& _residualFunctional = {}, const std::string& _wellsCenters = {},
			const std::string& _allowedValues = {}, const std::string& _productionByIter = {},
			const std::string& _productionByDay = {}) :
			residualFunctional(_residualFunctional), wellsCenters(_wellsCenters),
			allowedValues(_allowedValues), production(_productionByIter), 
			productionByDay(_productionByDay)
		{
		} 

		std::string residualFunctional;
		std::string wellsCenters;
		std::string allowedValues;
		std::string production;
		std::string productionByDay;		
	};


	static void writeAllowedValues(const std::vector<std::pair<double, double>>& allowedValues);
	static void writeIterData(const GaussNewton::IterData& iterData);
	static void init(const Paths& paths);
	static void setExecutablePath(const std::string& executablePath) { m_executablePath = executablePath; }

private:
	static void writeResidualFunctional(const unsigned int numberIteration, const double functional);
	static void writeWellCoordinates(const Eigen::VectorXd& coordinates);
	static void writeIterDataToConsole(const int numberIteration, const double functional, const Eigen::VectorXd& coordinates);
	static void writeProductionByDay(const unsigned int numberIteration, const std::vector<GaussNewton::WellParameters::EType>& types);
	static void writeProduction(const unsigned int numberIteration, const std::pair<double, double> production);

	static std::string m_executablePath;
	static Paths m_paths;

};