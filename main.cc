#include "sim.h"

int sum_drop=0;
void PDrop(string s, Ptr<const Packet> p)
{
	//std::cout << "================================================a packet drop." << std::endl;
	//std::cout << s << std::endl;
	sum_drop++;
}

int main(int argc, char *argv[])
{
	EECSim sim;
	sim.ReadCommand(argc, argv);
	sim.MakeTopology();
	sim.InstallApp();

	//Config::Connect ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/MacTxDrop", MakeCallback(PDrop));
	//Config::Connect ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRxDrop", MakeCallback(PDrop));
	//Config::Connect ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyTxDrop", MakeCallback(PDrop));
	Config::Connect ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyRxDrop", MakeCallback(PDrop));

	sim.Run();

	std::cout << "drop: " << sum_drop << std::endl;

	return 0;
}

