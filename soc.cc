#include "protocol.h"

SOC::SOC()
{
	//
}

SOC::~SOC()
{
	//
}

void SOC::HandleRecv(DataPacket &data)
{
	std::cout << "Time: " << Simulator::Now() << "\t"; 
	std::cout << "Delay: " << Simulator::Now() - data.dt << "\t"; 
	std::cout << m_node->GetId() << "\trecv packet from " << data.nodeID << "(" << data.x << ", " << data.y << ")\t";
	std::cout << "\tChannel:" << m_channelIndex;
	std::cout << "\tBattery remains " << m_battery->GetRemainingEnergy() << " J." << std::endl;
		
}

void SOC::ScheduleNext()
{
	if (m_slot==1)
	{
		return;
	}

	//switch channel
	SwitchChannel((m_nodeId % EECSim::numChannels)+100);

	//(&YansWifiPhy::m_channelSwitchDelay = 250us)
	m_processEvent = Simulator::Schedule (NanoSeconds (500000), &SOC::ScheduleTx, this);
	//int interval = 500000;
	//if (m_slot == 0)
	//{
	//	interval += 30*m_nodeId;
	//}
	//m_processEvent = Simulator::Schedule (NanoSeconds (interval), &SOC::SendPacket, this);
	++m_slot;
}

void SOC::ScheduleTx()
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

	m_processEvent = Simulator::Schedule (MilliSeconds (200), &SOC::ScheduleNext, this);

}

