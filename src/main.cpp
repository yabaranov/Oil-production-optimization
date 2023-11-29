#include "GNM.h"
#include "HDPoM.h"
#include <chrono>

int main(void)
{
	GridData gridData;
	gridData.ReadGrid();
	
	GNM gnm(gridData);
	gnm.ReadData();
	gnm.CleaningOutputFiles();
	
	createDirectoriesHDPoM(gridData.GetNameModel(), 2 * gnm.GetNumberWells());
	InitializationHDPoM(gridData.GetSimulationTime());
	auto start = std::chrono::steady_clock::now();
	gnm.IterativeProcess();
	auto end = std::chrono::steady_clock::now();
	
	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
	auto minutes = seconds / 60;
	std::cout << "Time: " << minutes << " minutes " << seconds - 60 * minutes << " seconds.\n ";

	//WinExec("Python ../src/visualization/ResidualFunctional.py", 1);
	//WinExec("Python ../src/visualization/PhaseByIter.py", 1);
	//WinExec("Python ../src/visualization/Visualize.py", 1);
	//WinExec("Python ../src/visualization/InitialPosWells.py", 1);

	//gnm.TestGeometry();
	
}