#include "protocol.h"

EEC::EEC()
{
	//
}

EEC::~EEC()
{
	//
}

void EEC::HandleRecv(DataPacket &data)
{
	std::cout << "Time: " << Simulator::Now() << "\t"; 
	std::cout << "Delay: " << Simulator::Now() - data.dt << "\t"; 
	std::cout << m_node->GetId() << "\trecv packet from " << data.nodeID;
	std::cout << "(" << data.x << ", " << data.y << ")\t" << std::endl;
}

void EEC::ScheduleNext()
{
	if (m_slot==1)
	{
		return;
	}

	//switch channel
	SwitchChannel((m_nodeId % EECSim::numChannels)+100);

	//(&YansWifiPhy::m_channelSwitchDelay = 250us)
	m_processEvent = Simulator::Schedule (NanoSeconds (500000), &EEC::ScheduleTx, this);
	++m_slot;
}

void EEC::ScheduleTx()
{
	//send packet
	//if (Simulator::Now() < Time(MilliSeconds(2)))
	Vector pos = m_mobility->GetPosition ();

	cout<< m_nodeId <<" EEC begin at "<<Simulator::Now() << std::endl;

	DataPacket data;
	data.nodeID = m_nodeId; 
	data.x = pos.x;
	data.y = pos.y;
	strcpy(data.msg, "hello");
	data.dt = Simulator::Now();

	Broadcast(data);

	m_processEvent = Simulator::Schedule (MilliSeconds (200), &EEC::ScheduleNext, this);

}

