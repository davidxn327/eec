#include "sim.h"

int main(int argc, char *argv[])
{
	EECSim sim;
	sim.ReadCommand(argc, argv);
	sim.MakeTopology();
	sim.InstallApp();
	sim.Run();

	return 0;
}

