
#include "app.h"
#include "ns3/wifi-net-device.h"

Ptr<UniformRandomVariable> Algorithm::urv = NULL;
double Algorithm::GetNextRandom()
{
	static uint32_t seed = (uint32_t)time(0);
	srand(seed++);
	return (rand()%1000)/1000.0;
}

TypeId Algorithm::GetTypeId(void)
{
	static TypeId tid = TypeId ("Algorithm")
		.SetParent<Application> ()
		.AddConstructor<Algorithm> ();

	return tid;
}

Algorithm::Algorithm() :
	m_nodeId(0),
	m_channelIndex(0),
	m_switchtime(500),
	m_txtime(200),
	m_slot(0)
{
	uint32_t total = EECSim::numChannels;
	for (uint32_t i = 0; i < total; ++i)
	{
		double r = GetNextRandom();
		if (r<0.9)
		{
			m_channels.insert(i);
		}
	}
}

Algorithm::~Algorithm()
{
	//
}

void Algorithm::DoDispose(void)
{
	m_socket = 0;
	// chain up
	Application::DoDispose ();
}

void PDrop(Ptr<const Packet> p);
void Algorithm::StartApplication(void)
{
	m_node = GetNode();
	m_mobility = m_node->GetObject<MobilityModel> ();
	m_device = DynamicCast<WifiNetDevice> (m_node->GetDevice(0));
	m_phy = DynamicCast<YansWifiPhy> (m_device->GetPhy());

	//m_phy->TraceConnectWithoutContext("PhyRxDrop", MakeCallback(&PDrop));
	//m_phy->TraceConnectWithoutContext("PhyRxDrop", MakeCallback(&PDrop));

	Ptr<EnergySourceContainer> esc = m_node->GetObject<EnergySourceContainer> ();	
	m_battery = esc->Get(0);

	m_nodeId = m_node->GetId();

	if(m_socket == 0)
	{
		TypeId tid = TypeId::LookupByName ("ns3::PacketSocketFactory");

		m_psAddr.SetSingleDevice (m_device->GetIfIndex ());
		m_psAddr.SetPhysicalAddress (m_device->GetBroadcast ());
		m_psAddr.SetProtocol (1);

		m_socket = Socket::CreateSocket (m_node, tid);
		m_socket->Bind ();

		m_socket->SetAllowBroadcast(true);
		m_socket->SetRecvCallback(MakeCallback(&Algorithm::Receive, this));
	}

	//m_socket->TraceConnectWithoutContext("Drop", MakeCallback(&PDrop));

	m_processEvent = Simulator::ScheduleNow (&Algorithm::ScheduleNextSlot, this);
	//m_processEvent = Simulator::Schedule (NanoSeconds (1000 * m_nodeId), &Algorithm::ScheduleNextSlot, this);
}

void Algorithm::StopApplication(void)
{
	Simulator::Cancel (m_processEvent);
}

void Algorithm::Receive( Ptr<Socket> socket)
{
	//std::cout << m_nodeId << "recv a packet." << std::endl;
	Ptr<Packet> packet;
	Address from;
	while(packet = socket->RecvFrom(from))
	{
		uint8_t buffer[1024];
		packet->CopyData(buffer, 1024);
		DataPacket *d = (DataPacket *)buffer;
		HandleRecv(*d);
	}
}

void Algorithm::HandleRecv(DataPacket &data)
{
	std::cout << "Time: " << Simulator::Now() << "\t"; 
	std::cout << "Delay: " << Simulator::Now() - data.dt << "\t"; 
	std::cout << m_node->GetId() << "\trecv packet from " << data.nodeID << "(" << data.x << ", " << data.y << ")\t";
	std::cout << "\tChannel:" << m_channelIndex;
	std::cout << "\tBattery remains " << m_battery->GetRemainingEnergy() << " J." << std::endl;
		
}

void Algorithm::ScheduleNextSlot()
{
	// |--switch--|-----------tx-----------|
	// ^          ^      ^                 ^
	// |          |      |                 |
	// start      wait   start             next
	// switching  random tx                slot
	// |                 |                 |
	// time_switch       time_tx           time_next
	
	// switching
	ScheduleSwitch();
	
	if (is_stop)
	{
		return;
	}

	//(&YansWifiPhy::m_channelSwitchDelay = 250us)
	Time time_switch = MicroSeconds (m_switchtime);
	Time time_tx = time_switch + NanoSeconds (5000 * m_nodeId);
	Time time_next  = time_switch + MilliSeconds (m_txtime);

	//wait & tx
	m_processEvent = Simulator::Schedule (time_tx, &Algorithm::ScheduleTx, this);

	//next slot
	m_processEvent = Simulator::Schedule (time_next, &Algorithm::ScheduleNextSlot, this);

	++m_slot;
}

void Algorithm::ScheduleSwitch()
{
	if (m_slot==1)
	{
		Stop();
		return;
	}

	//switch channel
	SwitchChannel((m_nodeId % EECSim::numChannels)+100);

}

void Algorithm::ScheduleTx()
{
	//send packet
	//if (Simulator::Now() < Time(MilliSeconds(2)))
	Vector pos = m_mobility->GetPosition ();

	cout<< m_nodeId <<" Procedure begin at "<<Simulator::Now();
	std::cout << "\tBattery remains " << m_battery->GetRemainingEnergy() << " J." << std::endl;

	DataPacket data;
	data.nodeID = m_nodeId; 
	data.x = pos.x;
	data.y = pos.y;
	strcpy(data.msg, "hello");
	data.dt = Simulator::Now();

	Broadcast(data);

}

void Algorithm::Send(string addr, DataPacket &data)
{
	Ptr<Packet> packet = Create<Packet>((uint8_t*)&data, sizeof(DataPacket));
	m_psAddr.SetPhysicalAddress(Mac48Address(addr.c_str()));
	m_socket->SendTo(packet, 0, m_psAddr);
}

void Algorithm::Broadcast(DataPacket &data)
{
	Send("ff:ff:ff:ff:ff:ff", data);
}

void Algorithm::SwitchChannel(uint32_t index)
{
	//if (m_channelIndex > 0)
	//{
	//	m_phy->SetChannelNumber(index);
	//}
	m_phy->SetChannelNumber(index);
	//m_phy->SetChannel(EECSim::GetChannel(index));
	m_channelIndex = index;
}

void Algorithm::Stop()
{
	is_stop = true;
}

