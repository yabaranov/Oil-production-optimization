#include "FieldGrid.h"

#include <array>

std::vector<unsigned int> FieldGrid::findElementNumbers(const Eigen::VectorXd& coordinates) const
{ 
    std::vector<unsigned int> elementsNumbers(coordinates.size() / 2);

    for (size_t i = 0; i < coordinates.size(); i += 2)
    {
        int j = binarySearchBetween(m_grid.x, coordinates[i], 0, m_grid.x.size() - 1);
        int k = binarySearchBetween(m_grid.y, coordinates[i + 1], 0, m_grid.y.size() - 1);

        elementsNumbers[i / 2] = k * (m_grid.x.size() - 1) + j;
    }

    return elementsNumbers;
}

void FieldGrid::aligningValuesToGrid(std::vector<std::pair<double, double>>& values) const
{
    for (int i = 0; i < values.size(); i++)        
    {
        int k = binarySearchBetween(m_grid.x, values[i].first, 0, m_grid.x.size() - 1);
        std::array<double, 2> distance = { std::abs(values[i].first - m_grid.x[k]), std::abs(values[i].first - m_grid.x[k + 1]) };
        values[i].first = (distance[0] < distance[1]) ? m_grid.x[k] : m_grid.x[k + 1];

        k = binarySearchBetween(m_grid.y, values[i].second, 0, m_grid.y.size() - 1);
        distance = { std::abs(values[i].second - m_grid.y[k]), std::abs(values[i].second - m_grid.y[k + 1]) };
        values[i].second = (distance[0] < distance[1]) ? m_grid.y[k] : m_grid.y[k + 1];
    }
 
}