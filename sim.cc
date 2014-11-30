#include "sim.h"

EECSim::EECSim() :
	numNodes(500),
	radius(50),
	algo("Test"),
	phyMode("DsssRate1Mbps"),
	verbose(false)
{
	//constructor
}

EECSim::~EECSim()
{
	//~
	numNodes = 0;
	radius = 0;
	algo = "";
	phyMode = "";
}

uint32_t EECSim::numChannels(10);
std::vector< Ptr<YansWifiChannel> > EECSim::m_ChannelSet;
Ptr<YansWifiChannel> EECSim::GetChannel(uint32_t index)
{
	if (m_ChannelSet.empty())
	{
		//add channels
		//channels&physicallayer
		YansWifiChannelHelper wifiChannel;
		wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
		wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel");

		for (uint32_t i = 0; i < numChannels; ++i)
		{
			m_ChannelSet.push_back(wifiChannel.Create());
		}
		
	}
	return m_ChannelSet[index];
}

void EECSim::ReadCommand(int argc, char *argv[])
{
	CommandLine cmd;
	cmd.AddValue ("phyMode", "Wifi Phy mode", phyMode);
	cmd.AddValue ("numNodes", "number of nodes", numNodes);
	cmd.AddValue ("numChannels", "number of channels", numChannels);
	cmd.AddValue ("algo", "select the algorithm", algo);
	cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
	cmd.Parse (argc, argv);
}

void EECSim::MakeTopology()
{
	//topology
	c.Create (numNodes);

	MobilityHelper mobility;
	mobility.SetPositionAllocator ("ns3::RandomRectanglePositionAllocator",
			"X", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=20.0]"),
			"Y", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=20.0]"));
	mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
	mobility.InstallAll ();
	//Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",MakeCallback (&CourseChange));
	
	//phy
	YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
	wifiPhy.Set ("RxGain", DoubleValue (10) ); 
	//wifiPhy.Set ("TxGain", DoubleValue (offset + Prss));
	//wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO); 
	wifiPhy.SetChannel (GetChannel(0));

	//mac
	NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default ();
	wifiMac.SetType ("ns3::AdhocWifiMac");

	//install wifi netdevice
	WifiHelper wifi;
	wifi.SetStandard (WIFI_PHY_STANDARD_80211b);
	wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
			"DataMode",StringValue (phyMode),
			"ControlMode",StringValue (phyMode));
	// disable fragmentation
	Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue ("2200"));
	Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("2200"));
	NetDeviceContainer devices = wifi.Install (wifiPhy, wifiMac, c);

	//energy
	BasicEnergySourceHelper basicSourceHelper;
	basicSourceHelper.Set ("BasicEnergySourceInitialEnergyJ", DoubleValue (50));
	EnergySourceContainer sources = basicSourceHelper.Install (c);
	WifiRadioEnergyModelHelper radioEnergyHelper;
	radioEnergyHelper.Set ("TxCurrentA", DoubleValue (0.0174));
	DeviceEnergyModelContainer deviceModels = radioEnergyHelper.Install (devices, sources);

	//internet stack
	//none

	if (verbose)
	{
		LogComponentEnable ("YansWifiPhy", LOG_LEVEL_INFO);
		LogComponentEnable ("WifiRadioEnergyModel", LOG_LEVEL_DEBUG);

	}
}

void EECSim::InstallApp()
{
	//install packet socket
	PacketSocketHelper packetSocket;
	packetSocket.Install (c);

	//applications
	string appstr = "Test";
	for (NodeContainer::Iterator j = c.Begin ();j != c.End (); ++j)
	{
		Ptr<Algorithm> app;
		app = CreateObject<Algorithm>();
		app->SetStartTime (Seconds (0));
		app->SetStopTime (Seconds (3));

		Ptr<Node> object = *j;
		object->AddApplication (app);
	}
}

void EECSim::Run()
{
	Simulator::Stop (Seconds (10.0));
	Simulator::Run ();

	Simulator::Destroy ();
	//std::cout << appstr << std::endl;
}

