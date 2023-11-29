#include <fstream> 
#include "GridData.h"
#include "functions.h"

void GridData::ReadGrid()
{
    std::ifstream textFile;

    textFile.open("../res/input/settings.txt");
    std::getline(textFile, m_nameModel);
    textFile >> m_simulationTime;
    textFile.close();
   
    std::string path = "../res/models/" + m_nameModel + std::to_string(0) + "/mesh/";
    
    textFile.open(path + "x.txt");
    int n;
    textFile >> n;
    m_grid[0].resize(n);
    for (int i = 0; i < m_grid[0].size(); i++)
        textFile >> m_grid[0][i];
              
    textFile.close();

    textFile.open(path + "y.txt");
    textFile >> n;
    m_grid[1].resize(n);
    for (int i = 0; i < m_grid[1].size(); i++)
        textFile >> m_grid[1][i];
            
    textFile.close();

}

int GridData::FindElemNumForPoint(const std::array<double, 2>& point) const
{ 
    int j = binarySearchBetween(m_grid[0], point[0], 0, m_grid[0].size() - 1);
    int k = binarySearchBetween(m_grid[1], point[1], 0, m_grid[1].size() - 1);
    return k * (m_grid[0].size() - 1) + j;
     
}

void GridData::AligningValuesToGrid(std::vector<std::array<double, 2>>& values) const
{
    for (int i = 0; i < values.size(); i++)        
        for (int j = 0; j < m_grid.size(); j++)
        {
            int k = binarySearchBetween(m_grid[j], values[i][j], 0, m_grid[j].size() - 1);
            std::array<double, 2> distance = { std::abs(values[i][j] - m_grid[j][k]), std::abs(values[i][j] - m_grid[j][k + 1]) };
            values[i][j] = (distance[0] < distance[1]) ? m_grid[j][k] : m_grid[j][k + 1];
        }
 
}