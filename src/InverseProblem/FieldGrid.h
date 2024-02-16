#pragma once
#include <vector>
#include <Eigen/Dense>

template <typename T>
inline int binarySearchBetween(const std::vector<T>& values, const T& value, int l, int r)
{
	while (l != r)
	{
		int mid = (l + r) / 2 + 1;
		(values[mid] > value) ? r = mid - 1 : l = mid;
	}

	return (values[l] <= value) ? l : -1;
}

class FieldGrid
{
public:

	class Grid2D
	{
	public:
		friend class FieldGrid;

		Grid2D(std::vector<double> _x, std::vector<double> _y) : x(std::move(_x)), y(std::move(_y)) {}

		Grid2D(Grid2D&& grid) noexcept : x(std::move(grid.x)), y(std::move(grid.y))  { }
		Grid2D& operator=(Grid2D&& grid) noexcept 
		{
			if (&grid == this)
				return *this;
			x = std::move(grid.x);
			y = std::move(grid.y);
		}

	private:
		std::vector<double> x;
		std::vector<double> y;
	};

	FieldGrid(Grid2D grid) : m_grid(std::move(grid))
	{
	}


	std::vector<unsigned int> findElementNumbers(const Eigen::VectorXd& coordinates) const;
	void aligningValuesToGrid(std::vector<std::pair<double, double>>& values) const;

	const std::vector<double>& getGridX() const { return m_grid.x; }
	const std::vector<double>& getGridY() const { return m_grid.y; }
private:
	Grid2D m_grid;
};