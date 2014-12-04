#include "sim.h"

int main(int argc, char *argv[])
{
	EECSim sim;
	sim.ReadCommand(argc, argv);
	sim.MakeTopology();
	sim.InstallApp();

	sim.Run();

	std::cout << "rxdrop: " << sum_rxdrop << std::endl;
	std::cout << "txdrop: " << sum_txdrop << std::endl;

	return 0;
}

