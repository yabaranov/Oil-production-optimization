#pragma once
#include <array>
#include <map>
#include <string>

#include <Eigen/Dense>


#include "FieldGrid.h"
#include "DenseSLAE.h"

class GaussNewton
{
public:
	static constexpr unsigned int NUMBER_FUNCTIONS = 2;

	class Settings
	{
	public:
		friend class GaussNewton;
		Settings(const std::array<double, NUMBER_FUNCTIONS> _alphas, const double _stepDerivative, const double _maxStep,
			const double _minValueFunctional, const unsigned int _maxIterations, const unsigned int _maxIterationsFunctionalNotReduced,
			const double _desiredVolumeOilProduced);
	private:
		std::array<double, NUMBER_FUNCTIONS> alphas;
		double stepDerivative;
		double maxStep;
		double minValueFunctional;
		unsigned int maxIterations;
		unsigned int maxIterationsFunctionalNotReduced;
		double desiredVolumeOilProduced;
	};

	class WellParameters
	{
	public:
		friend class GaussNewton;

		enum class EType
		{
			Production,
			Injection
		};

		enum class EMovementState
		{
			Disable,
			Enable
		};

		WellParameters(std::vector<std::string> _types, std::vector<std::string> _movementStates,
			Eigen::VectorXd _coordinates, std::vector<std::pair<double, double>> _allowedValues);

		WellParameters(WellParameters&& wellsParameters) noexcept : types(std::move(wellsParameters.types)), movementStates(std::move(wellsParameters.movementStates)), 
			coordinates(std::move(wellsParameters.coordinates)), allowedValues(std::move(wellsParameters.allowedValues))
		{
		}

		WellParameters& operator=(WellParameters&& wellsParameters) noexcept
		{
			if (&wellsParameters == this)
				return *this;
			types = std::move(wellsParameters.types);
			movementStates = std::move(wellsParameters.movementStates);
			coordinates = std::move(wellsParameters.coordinates);
			allowedValues = std::move(wellsParameters.allowedValues);
		}
	private:
		std::vector<EType> types;
		std::vector<EMovementState> movementStates;
		Eigen::VectorXd coordinates;
		std::vector<std::pair<double, double>> allowedValues;
	};

	struct IterData
	{
		IterData(const unsigned int _numberIteration,
		const double _functional, const std::pair<double, double> _production, const std::vector<GaussNewton::WellParameters::EType>& _types,
		const Eigen::VectorXd& _coordinates) :
			numberIteration(_numberIteration), functional(_functional),
			production(_production), types(_types), coordinates(_coordinates)
		{
		}

		unsigned int numberIteration;
		double functional;
		std::pair<double, double> production;
		const std::vector<GaussNewton::WellParameters::EType>& types; 
		const Eigen::VectorXd& coordinates;		
	};

	GaussNewton(const FieldGrid& gridData, Settings methodSettings, WellParameters wellsParameters);
	void run();

private:

	Settings m_methodSettings;

	WellParameters m_wellParameters;
	unsigned int m_numberWells;

	const FieldGrid& m_fieldGrid;

	std::vector<std::vector<double>>              calculateFunctions(const Eigen::VectorXd& coordinates, const unsigned int modelNumber) const;
	std::vector<std::vector<std::vector<double>>> calculateFunctionDerivatives(const std::vector<std::vector<double>>& valuesFunctions) const;
	double										  calculateResidualFunctional(std::vector<std::vector<double>>& valuesFunctions) const;
	DenseSLAE									  assemblySLAE(const std::vector<std::vector<double>>& valuesFunctions, const std::vector<std::vector<std::vector<double>>>& valuesDerivativesFunctions) const;
	Eigen::VectorXd								  calculateParameterSteps(const DenseSLAE& denseSLAE, const std::vector<std::pair<double, double>>& dynamicAllowedValues) const;
	std::vector<std::pair<double, double>>		  calculateNewAllowedValues() const;
	void										  findIntersectionDomains(const std::vector<std::pair<double, double>>& allowedValues, int i, std::multimap<int, int>& intersectionDomains) const;
	std::pair<double, double>				      calculateProduction(const std::vector<std::vector<double>>& valuesFunctions) const;
};


