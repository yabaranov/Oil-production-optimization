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
#include <filesystem>
#include <fstream>
#include "functions.h"

inline void runExe(const std::filesystem::path& path)
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

    if (CreateProcessW(path.relative_path().c_str(), NULL, NULL, NULL, 0, 0, NULL, dir.c_str(), &si, &pi))
    {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

inline void runHDPoM(const std::string& nameModel)
{
    std::filesystem::path checkFile = "../res/models/" + nameModel + "/filtr.log";   
    do
    {
        runExe("../res/models/" + nameModel + "/Filtr3D.exe");      
    } while (std::filesystem::file_size(checkFile) < 2048);
}

inline double calculateSumPhaseFromFileHDPoM(const std::string& path)
{
    std::ifstream file;
    double sum = 0.0;
    double stepTime = 0.0;
    double value = 0.0;

    file.open(path);
    file >> stepTime;
    file >> value;
    file >> stepTime;
    file >> value;
    sum += value;

    while (!file.eof())
    {
        value = 0.0;
        file >> value;
        file >> value;
        sum += value;
    }
    file.close();

    return stepTime * sum;
}

inline void calculatePhaseByDayFromFileHDPoM(const std::string& path, std::ostream& os)
{
    std::ifstream file;
    double sum = 0.0;
    double stepTime = 0.0;
    double value = 0.0;
    double time = 0.0;

    file.open(path);
    file >> stepTime;
    file >> value;
    os << stepTime << " " << stepTime * sum << std::endl;
    file >> stepTime;
    file >> value;
    sum += value;
    os << stepTime << " " << stepTime * sum << std::endl;
    while (!file.eof())
    {
        value = 0.0;
        file >> time;
        file >> value;
        sum += value;
        os << time << " " << stepTime * sum << std::endl;
    }
    file.close();
}

inline void createDirectoriesHDPoM(const std::string& nameModel, int numberDirectories)
{
    std::vector<std::filesystem::path> subdirs;
    getSubdirectories("../res/models", subdirs);

    if (subdirs.size() < numberDirectories)    
        for (int i = subdirs.size(); i < numberDirectories; i++)
        {
            std::filesystem::create_directory("../res/models/" + nameModel + std::to_string(i));
            std::filesystem::copy(subdirs[0], "../res/models/" + nameModel + std::to_string(i), std::filesystem::copy_options::recursive);
        }
    
}

inline void setSimulationTimeHDPoM(double simulationTime)
{
    std::vector<std::filesystem::path> subdirs;
    getSubdirectories("../res/models", subdirs);

    std::string newString = std::to_string(simulationTime);
    int numberLine = 13;
    for (int i = 0; i < subdirs.size(); i++)
    {
        std::string s;       
        std::vector <std::string> strings;

        std::ifstream fin(subdirs[i].string() + "/filtr.cfg");
        while (getline(fin, s)) 
            strings.push_back(s);       
        fin.close();
       
        strings[numberLine - 1] = newString;

        std::ofstream fout(subdirs[i].string() + "/filtr.cfg");
        std::copy(strings.begin(), strings.end(), std::ostream_iterator<std::string>(fout, "\n"));
        fout.close();      
    }
}

inline void setWellCapacity()
{
    std::vector<std::filesystem::path> subdirs;
    getSubdirectories("../res/models", subdirs);

    std::ifstream fin("../res/input/well capacity.txt");
    int numberWells = 0;
    fin >> numberWells;
    std::vector<std::vector<std::array<double, 3>>> wellModes(numberWells);

    for (int i = 0; i < numberWells; i++)
    {
        int numberModes = 0;
        fin >> numberModes;
        wellModes[i].resize(numberModes);
        for (int j = 0; j < wellModes[i].size(); j++)
            for(int k = 0; k< wellModes[i][j].size(); k++)
            fin >> wellModes[i][j][k];
    }
    fin.close();

    for (int i = 0; i < subdirs.size(); i++)
    {
        std::ofstream fout;        
        fout.open(subdirs[i].string() + "/properties/wellsRadius.txt");
        fout << numberWells << std::endl;
        for (int j = 0; j < numberWells; j++)
            fout << "Well" + std::to_string(j + 1) << ' ' << 1 << ' ' << 1 << std::endl;
        fout.close();

        fout.open(subdirs[i].string() + "/properties/wellsPerfMode.txt");
        fout << numberWells << std::endl;
        for (int j = 0; j < numberWells; j++)
        {
            fout << "Well" + std::to_string(j + 1) << std::endl;
            fout << "Well" + std::to_string(j + 1) + "_1" << std::endl;
            fout << "Well_Well" + std::to_string(j + 1) + "_Absorbing1" << std::endl;
            fout << 1 << std::endl;
            fout << 5000 << std::endl;
            fout << 1 << std::endl;
        }
        fout.close();

        fout.open(subdirs[i].string() + "/mesh/holes_height.txt");
        fout << numberWells << std::endl;
        for (int j = 0; j < numberWells; j++)
        {

            fout << j + 1 << ' ' << 1 << ' ' << "Well" + std::to_string(j + 1) << std::endl;
            fout << 1 << ' ' << 1 << std::endl;
            fout << 1 << ' ' << 0 << ' ' << 0 << ' ' << 0 << ' ' << 0 << std::endl;
          
        }
        fout.close();

        fout.open(subdirs[i].string() + "/properties/wellsCONS.txt");
        fout << numberWells << std::endl;
        for (int j = 0; j < numberWells; j++)
        {
            fout << "Well" + std::to_string(j + 1) << std::endl;
            fout << 1 << std::endl;
            fout << "Well_Well" + std::to_string(j + 1) + "_Absorbing1" << std::endl;
            fout << wellModes[j].size() << std::endl;
            for (int k = 0; k < wellModes[j].size(); k++)
            {
                fout << wellModes[j][k][0] << std::endl;
                fout << wellModes[j][k][1] << std::endl;
                fout << 1 << std::endl;
                fout << wellModes[j][k][2] << std::endl;
                fout << 20 << std::endl;
            }                          
        }
        fout << 0 << std::endl;
        fout.close();
    }
}

inline void InitializationHDPoM(double simulationTime)
{
    setSimulationTimeHDPoM(simulationTime);
    setWellCapacity();
}
