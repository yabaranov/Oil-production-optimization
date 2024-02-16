#pragma once
#define _WIN32_WINNT 0x0601
#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOSYSCOMMANDS
#define NORASTEROPS
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOKERNEL
#define NONLS
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOTAPE
#define STRICT
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>


#include "HDPoM.h"
#include "fileSystemFunctions.h"

#include <fstream>


static void runExe(const std::filesystem::path& path)
{
    std::filesystem::path dir = std::filesystem::absolute(path).remove_filename();

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    STARTUPINFOW si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdInput = NULL;
    si.hStdError = NULL;
    si.hStdOutput = NULL;
     
    auto pathExe = std::filesystem::absolute(path);

    if (CreateProcessW(pathExe.c_str(), NULL, NULL, NULL, 0, 0, NULL, dir.c_str(), &si, &pi))
    {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

HDPoM::FieldSettings::WellMode::WellMode(const unsigned int _duration, const double _wellCapacity, const std::string& _polymerState) :
    duration(_duration), wellCapacity(_wellCapacity)
{
    if (_polymerState == "Disable")
        ePolymerState = EPolymerState::Disable;
    else if (_polymerState == "Enable")
        ePolymerState = EPolymerState::Enable;
    else
    {
        throw std::runtime_error("incorrent state polymer!");
    }

}

std::string HDPoM::m_executablePath;

FieldGrid::Grid2D HDPoM::readFieldGrid()
{ 
    auto textFileX = getFileStringStream(m_executablePath + "/res/models/model0/mesh" + "/x.txt");
    unsigned int n = 0;
    textFileX >> n;
    std::vector<double> x;
    x.reserve(n);
    for (int i = 0; i < n; i++)
    {
        double value = 0.0;
        textFileX >> value;
        x.emplace_back(value);
    }
  
    auto textFileY = getFileStringStream(m_executablePath + "/res/models/model0/mesh" + "/y.txt");
    textFileY >> n;
    std::vector<double> y;
    y.reserve(n);
    for (int i = 0; i < n; i++)
    {
        double value = 0.0;
        textFileY >> value;
        y.emplace_back(value);
    }

    return FieldGrid::Grid2D{x, y};
}

void HDPoM::run(const unsigned int modelNumber)
{
    std::filesystem::path checkFile = m_executablePath + "/res/models/model" + std::to_string(modelNumber) + "/filtr.log";
    do
    {
        runExe(m_executablePath + "/res/models/model" + std::to_string(modelNumber) + "/Filtr3D.exe");
    } while (std::filesystem::file_size(checkFile) < 2048);
}

double HDPoM::calculateSumPhase(const unsigned int modelNumber, const unsigned int wellNumber, const unsigned int phaseNumber)
{
    auto textFile = getFileStringStream(m_executablePath + "/res/models/model" + std::to_string(modelNumber) + "/output/m_Well" + std::to_string(wellNumber) + "_s" + std::to_string(phaseNumber) + ".txt");
    double sum = 0.0;
    double stepTime = 0.0;
    double value = 0.0;

    textFile >> stepTime;
    textFile >> value;
    textFile >> stepTime;
    textFile >> value;
    sum += value;

    while (!textFile.eof())
    {
        value = 0.0;
        textFile >> value;
        textFile >> value;
        sum += value;
    }

    return stepTime * sum;
}

void HDPoM::calculatePhaseByDay(const unsigned int wellNumber, const unsigned int phaseNumber, std::ostream& os)
{
    auto textFile = getFileStringStream(m_executablePath + "/res/models/model0/output/m_Well" + std::to_string(wellNumber) + "_s" + std::to_string(phaseNumber) + ".txt");
    double sum = 0.0;
    double stepTime = 0.0;
    double value = 0.0;
    double time = 0.0;
  
    textFile >> stepTime;
    textFile >> value;
    os << stepTime << " " << stepTime * sum << std::endl;
    textFile >> stepTime;
    textFile >> value;
    sum += value;
    os << stepTime << " " << stepTime * sum << std::endl;
    while (!textFile.eof())
    {
        value = 0.0;
        textFile >> time;
        textFile >> value;
        sum += value;
        os << time << " " << stepTime * sum << std::endl;
    }
}

void HDPoM::repositioningWells(const std::vector<unsigned int>& elementNumbers, const unsigned int modelNumber)
{
    std::ofstream textFile(m_executablePath + "/res/models/model" + std::to_string(modelNumber) + "/mesh/WellsSource.txt");
    textFile << elementNumbers.size() << std::endl;

    for (int i = 0; i < elementNumbers.size(); i++)
    {
        int numElem = elementNumbers[i];
        textFile << i + 1 << ' ' << 1 << std::endl;
        textFile << 1 << ' ' << -2 << ' ' << 1 << std::endl;
        textFile << ++numElem << std::endl;
    }

}

void HDPoM::resetSimulationTime(unsigned int simulationTime)
{
    const int numberLine = 13;
   
    auto fin = getFileStringStream(m_executablePath + "/res/models/model0/filtr.cfg");
    std::string s;
    std::vector<std::string> strings;
    while (getline(fin, s))
        strings.push_back(s);

    strings[numberLine - 1] = std::to_string(simulationTime);

    std::ofstream fout(m_executablePath + "/res/models/model0/filtr.cfg");
    std::copy(strings.begin(), strings.end(), std::ostream_iterator<std::string>(fout, "\n"));
}

void HDPoM::resetWellModes(const std::vector<std::vector<FieldSettings::WellMode>>& wellsModes)
{
    size_t numberWells = wellsModes.size();
    std::string path = m_executablePath + "/res/models/model0";

    std::ofstream fileWellsRadius(path + "/properties/wellsRadius.txt");
    fileWellsRadius << numberWells << std::endl;
    for (int j = 0; j < numberWells; j++)
        fileWellsRadius << "Well" + std::to_string(j + 1) << ' ' << 1 << ' ' << 1 << std::endl;

    std::ofstream fileWellsPerfMode(path + "/properties/wellsPerfMode.txt");
    fileWellsPerfMode << numberWells << std::endl;
    for (int j = 0; j < numberWells; j++)
    {
        fileWellsPerfMode << "Well" + std::to_string(j + 1) << std::endl;
        fileWellsPerfMode << "Well" + std::to_string(j + 1) + "_1" << std::endl;
        fileWellsPerfMode << "Well_Well" + std::to_string(j + 1) + "_Absorbing1" << std::endl;
        fileWellsPerfMode << 1 << std::endl;
        fileWellsPerfMode << 5000 << std::endl;
        fileWellsPerfMode << 1 << std::endl;
    }

    std::ofstream fileHolesHeight(path + "/mesh/holes_height.txt");
    fileHolesHeight << numberWells << std::endl;
    for (int j = 0; j < numberWells; j++)
    {

        fileHolesHeight << j + 1 << ' ' << 1 << ' ' << "Well" + std::to_string(j + 1) << std::endl;
        fileHolesHeight << 1 << ' ' << 1 << std::endl;
        fileHolesHeight << 1 << ' ' << 0 << ' ' << 0 << ' ' << 0 << ' ' << 0 << std::endl;

    }

    std::ofstream fileWellsCONS(path + "/properties/wellsCONS.txt");
    fileWellsCONS << numberWells << std::endl;
    for (int j = 0; j < numberWells; j++)
    {
        fileWellsCONS << "Well" + std::to_string(j + 1) << std::endl;
        fileWellsCONS << 1 << std::endl;
        fileWellsCONS << "Well_Well" + std::to_string(j + 1) + "_Absorbing1" << std::endl;
        fileWellsCONS << wellsModes[j].size() << std::endl;
        for (int k = 0; k < wellsModes[j].size(); k++)
        {
            fileWellsCONS << wellsModes[j][k].duration << std::endl;
            fileWellsCONS << wellsModes[j][k].wellCapacity << std::endl;
            fileWellsCONS << 1 << std::endl;
            fileWellsCONS << static_cast<unsigned int>(wellsModes[j][k].ePolymerState) << std::endl;
            fileWellsCONS << 20 << std::endl;
        }
    }
    fileWellsCONS << 0 << std::endl;
}

void HDPoM::init(FieldSettings fieldSettings)
{
    std::filesystem::copy(m_executablePath + "/res/fields/" + fieldSettings.nameField, m_executablePath + "/res/models", std::filesystem::copy_options::recursive);
    resetSimulationTime(fieldSettings.simulationTime);
    resetWellModes(fieldSettings.wellModes);
  
    for (int i = 1; i < 2 * fieldSettings.wellModes.size(); i++)   
        std::filesystem::copy(m_executablePath + "/res/models/model0", m_executablePath + "/res/models/model" + std::to_string(i), std::filesystem::copy_options::recursive);   
}

void HDPoM::terminate()
{
    deleteSubdirectories(m_executablePath + "/res/models");
}


