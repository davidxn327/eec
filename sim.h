#ifndef __SIM_H__
#define __SIM_H__ 

#include <iostream>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/energy-module.h"

#include "app.h"

using namespace ns3;
using namespace std;

class EECSim
{
public:
	EECSim ();
	virtual ~EECSim ();

	void ReadCommand(int argc, char *argv[]);
	void MakeTopology();
	void InstallApp();
	void Run();

	static uint32_t numChannels;
	static Ptr<YansWifiChannel> GetChannel(uint32_t index);

private:
	uint32_t numNodes;
	uint32_t radius;//meter
	string algo;
	string phyMode;
	NodeContainer c;
	bool verbose;
	static std::vector< Ptr<YansWifiChannel> > m_ChannelSet;

};


#endif

