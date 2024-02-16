#pragma once

#include "../InverseProblem/FieldGrid.h"

#include <vector>
#include <filesystem>

class HDPoM
{
public:

    class FieldSettings
    {
    public:
        friend class HDPoM;

        class WellMode
        {
        public:
            friend class HDPoM;

            enum class EPolymerState
            {
                Disable = 1,
                Enable = 2
            };

            WellMode(const unsigned int _duration, const double _wellCapacity, const std::string& _polymerState);

        private:
            unsigned int duration;
            double wellCapacity;
            EPolymerState ePolymerState;
        };

        FieldSettings(const unsigned int _simulationTime, std::string _nameField, std::vector<std::vector<WellMode>> _wellsModes) :
            simulationTime(_simulationTime), nameField(std::move(_nameField)), wellModes(std::move(_wellsModes)) 
        {            
            if(simulationTime == 0) throw std::runtime_error("simulationTime = 0!");
        }

        FieldSettings(FieldSettings&& fieldSettings) noexcept: simulationTime(fieldSettings.simulationTime), nameField(std::move(fieldSettings.nameField)),
            wellModes(std::move(fieldSettings.wellModes))
        {          
        }

        FieldSettings& operator=(FieldSettings&& fieldSettings) noexcept
        {
            if (&fieldSettings == this)
                return *this;

            simulationTime = fieldSettings.simulationTime;
            nameField = std::move(fieldSettings.nameField);
            wellModes = std::move(fieldSettings.wellModes);
        }      
    private:
        unsigned int simulationTime;
        std::string nameField;      
        std::vector<std::vector<WellMode>> wellModes;
    };

    HDPoM() = delete;
    ~HDPoM() = delete;
    HDPoM(const HDPoM&) = delete;
    HDPoM& operator=(const HDPoM&) = delete;
    HDPoM(HDPoM&&) = delete;
    HDPoM& operator=(HDPoM&&) = delete;

    static void calculatePhaseByDay(const unsigned int wellNumber, const unsigned int phaseNumber, std::ostream& os);
    static double calculateSumPhase(const unsigned int modelNumber, const unsigned int wellNumber, const unsigned int phaseNumber);
    static void repositioningWells(const std::vector<unsigned int>& elementNumbers, const unsigned int modelNumber);
    static FieldGrid::Grid2D readFieldGrid();
    static void run(const unsigned int modelNumber);
    static void init(FieldSettings fieldSettings);
    static void setExecutablePath(const std::string& executablePath) { m_executablePath = executablePath; }
    static void terminate();

private:
    static std::string m_executablePath;
    
    static void resetSimulationTime(unsigned int simulationTime);
    static void resetWellModes(const std::vector<std::vector<FieldSettings::WellMode>>& wellModes);
};
