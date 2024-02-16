#include "ConfigureReader.h"
#include "fileSystemFunctions.h"

#include <rapidjson/error/en.h>


std::string ConfigureReader::m_executablePath;

HDPoM::FieldSettings ConfigureReader::readFieldSettings(const std::string& path)
{
	rapidjson::Document document = getJSONDocument(path);

	std::string nameField;
	auto nameFieldIt = document.FindMember("nameField");
	if (nameFieldIt != document.MemberEnd())	
		nameField = nameFieldIt->value.GetString();		
	
	unsigned int simulationTime = 0;
	auto simulationTimeIt = document.FindMember("simulationTime");
	if (simulationTimeIt != document.MemberEnd())
		simulationTime = simulationTimeIt->value.GetUint();
		
	std::vector<std::vector<HDPoM::FieldSettings::WellMode>> wellModes;
	auto wellsModesIt = document.FindMember("wellModes");
	if (wellsModesIt != document.MemberEnd())
	{
		const auto wellsModesArray = wellsModesIt->value.GetArray();
		wellModes.reserve(wellsModesArray.Size());
	
		for (const auto& currentWellModes : wellsModesArray)
		{
			const auto wellModesArray = currentWellModes.GetArray();
			std::vector<HDPoM::FieldSettings::WellMode> currentWellModes;
			currentWellModes.reserve(wellModesArray.Size());
	
			for (const auto& currentWellMode : wellModesArray)
			{
				const auto duration = currentWellMode["duration"].GetUint();
				const auto wellCapacity = currentWellMode["wellCapacity"].GetDouble();
				const std::string& polymer = currentWellMode["polymerState"].GetString();
				currentWellModes.emplace_back(duration, wellCapacity, polymer);
			}
	
			wellModes.emplace_back(currentWellModes);
		}
			
	}
		
	return HDPoM::FieldSettings{ simulationTime, std::move(nameField), std::move(wellModes) };
}

GaussNewton::Settings ConfigureReader::readGaussNewtonSettings(const std::string& path)
{
	rapidjson::Document document = getJSONDocument(path);

	std::array<double, GaussNewton::NUMBER_FUNCTIONS> alphas = {0.0, 0.0};
	auto alphasIt = document.FindMember("alphas");
	if (alphasIt != document.MemberEnd())
	{
		const auto alphasArray = alphasIt->value.GetArray();
		for (size_t i = 0; i < alphas.size(); i++)
			alphas[i] = alphasArray[i].GetDouble();
	}
		
	double stepDerivative = 0.0;
	auto stepDerivativeIt = document.FindMember("stepDerivative");
	if (stepDerivativeIt != document.MemberEnd())
		stepDerivative = stepDerivativeIt->value.GetDouble();
	
	double maxStep = 0.0;
	auto maxStepIt = document.FindMember("maxStep");
	if (maxStepIt != document.MemberEnd())
		maxStep = maxStepIt->value.GetDouble();
	
	double minValueFunctional = 0.0;
	auto minValueFunctionalIt = document.FindMember("minValueFunctional");
	if (minValueFunctionalIt != document.MemberEnd())
		minValueFunctional = minValueFunctionalIt->value.GetDouble();
	
	unsigned int maxIterations = 0;
	auto maxIterationsIt = document.FindMember("maxIterations");
	if (maxIterationsIt != document.MemberEnd())
		maxIterations = maxIterationsIt->value.GetUint();
	
	unsigned int maxIterationsFunctionalNotReduced = 0;
	auto maxIterationsFunctionalNotReducedIt = document.FindMember("maxIterationsFunctionalNotReduced");
	if (maxIterationsFunctionalNotReducedIt != document.MemberEnd())
		maxIterationsFunctionalNotReduced = maxIterationsFunctionalNotReducedIt->value.GetUint();
	
	double desiredVolumeOilProduced = 0.0;
	auto desiredVolumeOilProducedIt = document.FindMember("desiredVolumeOilProduced");
	if (desiredVolumeOilProducedIt != document.MemberEnd())
		desiredVolumeOilProduced = desiredVolumeOilProducedIt->value.GetDouble();

	return GaussNewton::Settings{ alphas, stepDerivative, maxStep, minValueFunctional, maxIterations, maxIterationsFunctionalNotReduced, desiredVolumeOilProduced };

}

GaussNewton::WellParameters ConfigureReader::readWellParameters(const std::string& path)
{
	rapidjson::Document document = getJSONDocument(path);

	std::vector<std::string> types;
	std::vector<std::string> movementStates;
	Eigen::VectorXd coordinates;
	std::vector<std::pair<double, double>> allowedValues;

	auto wellsParametersIt = document.FindMember("wellsParameters");
	if (wellsParametersIt != document.MemberEnd())
	{
		const auto wellsParametersArray = wellsParametersIt->value.GetArray();
		size_t sizeArray = wellsParametersArray.Size();
	
		types.reserve(sizeArray);
		movementStates.reserve(sizeArray);
		allowedValues.reserve(2 * sizeArray);
		coordinates.resize(2 * sizeArray);

		unsigned int count = 0;

		for (const auto& currentWell : wellsParametersArray)
		{
			const std::string type = currentWell["type"].GetString();
			types.emplace_back(type);
		
			const std::string movement = currentWell["movementState"].GetString();
			movementStates.emplace_back(movement);
			
			for (const auto& currentElement : currentWell["coordinates"].GetArray())			
				coordinates(count++) = currentElement.GetDouble();
						
			const auto allowedValuesXArray = currentWell["allowedValuesX"].GetArray();
			std::pair<double, double> allowedValuesX{ allowedValuesXArray[0].GetDouble(), allowedValuesXArray[1].GetDouble() };
			allowedValues.emplace_back(allowedValuesX);
			
			const auto allowedValuesYArray = currentWell["allowedValuesY"].GetArray();
			std::pair<double, double> allowedValuesY{ allowedValuesYArray[0].GetDouble(), allowedValuesYArray[1].GetDouble() };
			allowedValues.emplace_back(allowedValuesY);
			
		}
	}
	return GaussNewton::WellParameters{ std::move(types), std::move(movementStates), std::move(coordinates), std::move(allowedValues) };
}

rapidjson::Document ConfigureReader::getJSONDocument(const std::string& path)
{
	const auto JSONString = getFileStringStream(m_executablePath + "/" + path).str();
	
	if (JSONString.empty())		
		throw std::runtime_error("JSON file " + path + " is empty!");
	
	rapidjson::Document document;
	rapidjson::ParseResult parseResult = document.Parse(JSONString.c_str());
	if (!parseResult)
	{
		std::cerr << "JSON parse error: " << rapidjson::GetParseError_En(parseResult.Code()) << "(" << parseResult.Offset() << ")" << std::endl;
		throw std::runtime_error("file " + path + " not parse!");
	}

	return document;
}
