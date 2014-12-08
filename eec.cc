#include "protocol.h"

EEC::EEC()
{
	uint32_t total = EECSim::numChannels;
	while(1) {
		for (uint32_t i = 2; i < sqrt(total); ++i)
		{
			if (total % i == 0)
			{
				total++;
				continue;
			}	
		}
		break;
	}
	m_prime = total;
	m_step = (uint32_t)(GetNextRandom() * m_prime);
}

EEC::~EEC()
{
	//
}

void EEC::HandleRecv(DataPacket &data)
{
	//std::cout << "Time: " << Simulator::Now() << "\t"; 
	//std::cout << "Delay: " << Simulator::Now() - data.dt << "\t"; 
	//std::cout << m_node->GetId() << "\trecv packet from " << data.nodeID;
	//std::cout << "(" << data.x << ", " << data.y << ")\t" << std::endl;

	if (m_neighbors.count(data.nodeID)==0)
	{
		m_neighbors[data.nodeID] = &data;
	}
}

void EEC::ScheduleSwitch()
{
	if (m_slot==EECSim::numChannels)
	{
		//stop
		Stop();

		std::cout << m_nodeId << "\tdiscovered " << m_neighbors.size() << " nodes.";
		std::cout << " channels: (";
		//for (std::set<uint32_t>::const_iterator i = m_channels.begin(); i != m_channels.end(); ++i) {
		//	std::cout << *i << ", ";
		//}
		uint32_t total = EECSim::numChannels;
		for (uint32_t i = 0; i < total; ++i)
		{
			if (m_channels.count(i)>0)
			{
				std::cout << *m_channels.find(i) << "\t";
			}
			else
			{
				std::cout << "-" << "\t";
			}
		}
		std::cout << ")." << std::endl;

		Procedure();

		return;
	}

	//switch channel
	m_channelIndex += m_step;
	uint32_t chid = m_channelIndex % EECSim::numChannels;
	if (m_channels.count(chid)>0)
	{
		is_skipSlot = false;
		SwitchChannel(chid);
	}
	else
	{
		is_skipSlot = true;
	}
}

void EEC::ScheduleTx()
{
	if (is_skipSlot)
	{
		return;
	}

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

void EEC::Procedure()
{
	//cluster algorithm runs here
}
