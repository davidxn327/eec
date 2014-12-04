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

void EEC::ScheduleSwitch()
{
	if (m_slot==1)
	{
		Stop();
		return;
	}

	//switch channel
	SwitchChannel((m_nodeId % EECSim::numChannels)+100);
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
}

