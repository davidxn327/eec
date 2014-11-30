
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
	m_slot(0)
{
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

void Algorithm::StartApplication(void)
{
	m_node = GetNode();
	m_mobility = m_node->GetObject<MobilityModel> ();
	m_device = DynamicCast<WifiNetDevice> (m_node->GetDevice(0));
	m_phy = DynamicCast<YansWifiPhy> (m_device->GetPhy());

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

	//m_processEvent = Simulator::ScheduleNow (&Algorithm::ScheduleNext, this);
	m_processEvent = Simulator::Schedule (NanoSeconds (10 * m_nodeId), &Algorithm::ScheduleNext, this);
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

void Algorithm::ScheduleNext()
{
	if (m_slot==1)
	{
		return;
	}

	//switch channel
	SwitchChannel((m_nodeId % EECSim::numChannels)+100);

	//(&YansWifiPhy::m_channelSwitchDelay = 250us)
	m_processEvent = Simulator::Schedule (NanoSeconds (500000), &Algorithm::ScheduleTx, this);
	//int interval = 500000;
	//if (m_slot == 0)
	//{
	//	interval += 30*m_nodeId;
	//}
	//m_processEvent = Simulator::Schedule (NanoSeconds (interval), &Algorithm::SendPacket, this);
	++m_slot;
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

	m_processEvent = Simulator::Schedule (MilliSeconds (200), &Algorithm::ScheduleNext, this);

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

