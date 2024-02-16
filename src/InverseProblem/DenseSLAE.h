#pragma once
#include <Eigen/Dense>

class DenseSLAE
{
public:
	DenseSLAE(Eigen::MatrixXd matrix, Eigen::VectorXd rightSide) : m_matrix(std::move(matrix)), m_rightSide(std::move(rightSide)) {}

	DenseSLAE(unsigned int n) : m_matrix(n, n), m_rightSide(n) {}
	static Eigen::VectorXd solve(const Eigen::MatrixXd& matrix, const Eigen::VectorXd& rightSide) { return matrix.completeOrthogonalDecomposition().solve(rightSide); }

	void setMatrix(Eigen::MatrixXd matrix) { m_matrix = std::move(matrix); }
	void setMatrixElement(const unsigned int i, const unsigned int j, const double value) { m_matrix(i, j) = value; }
	void addToMatrixElement(const unsigned int i, const unsigned int j, const double value) { m_matrix(i, j) += value; }
	void setRightSide(Eigen::VectorXd rightSide) { m_rightSide = std::move(rightSide); }
	void setRightSideElement(const unsigned int i, const double value) { m_rightSide[i] = value; }
	void addToRightSideElement(const unsigned int i, const double value) { m_rightSide[i] += value; }
	

	double getMatrixElement(const unsigned int i, const unsigned int j) const { return m_matrix(i, j); }
	double getRightSideElement(const unsigned int i) const { return m_rightSide[i]; }

	const Eigen::MatrixXd& getMatrix() const { return m_matrix; }
	const Eigen::VectorXd& getRightSide() const { return m_rightSide; }

private:
	Eigen::MatrixXd m_matrix;
	Eigen::VectorXd m_rightSide;
  
};