#pragma once

#include <string>
#include "HDPoM.h"
#include "../InverseProblem/GaussNewton.h"
#include <rapidjson/document.h>

class ConfigureReader
{
public:
	
	static HDPoM::FieldSettings readFieldSettings(const std::string& path);
	static GaussNewton::Settings readGaussNewtonSettings(const std::string& path);
	static GaussNewton::WellParameters readWellParameters(const std::string& path);
	static void setExecutablePath(const std::string& executablePath) { m_executablePath = executablePath; }

private:
	static std::string m_executablePath;
	static rapidjson::Document getJSONDocument(const std::string& path);
	
};