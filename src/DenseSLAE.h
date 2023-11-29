#pragma once
#include "MatrixVector.h"
class DenseSLAE
{
private:

	Matrix m_M;
	Vector m_b;
	Vector m_x;

public:

	DenseSLAE() {}

	void GenerateSLAE(int n);
	static void Gauss(Matrix M, Vector b, Vector& x);
	int Size() { return m_M.size(); }
	const Matrix& GetMatrix() { return m_M; }
	const Vector& GetRightHand() { return m_b; }
	const Vector& GetUnknowns() { return m_x; }
	Matrix& SetMatrix() { return m_M; }
	Vector& SetRightHand() { return m_b; }
	Vector& SetUnknowns() { return m_x; }

	double GetMatrixElement(int i, int j) { return m_M[i][j]; }
	double GetRightHandElement(int i) { return m_b[i]; }
	double GetUnknownsElement(int i) { return m_x[i]; }
	double& SetMatrixElement(int i, int j) { return m_M[i][j]; }
	double& SetRightHandElement(int i) { return m_b[i]; }
	double& SetUnknownsElement(int i) { return m_x[i]; }
};