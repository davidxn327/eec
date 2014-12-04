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
	//std::cout << "Time: " << Simulator::Now() << "\t"; 
	//std::cout << "Delay: " << Simulator::Now() - data.dt << "\t"; 
	//std::cout << m_node->GetId() << "\trecv packet from " << data.nodeID << "(" << data.x << ", " << data.y << ")\t";
	//std::cout << "\tChannel:" << m_channelIndex;
	//std::cout << "\tBattery remains " << m_battery->GetRemainingEnergy() << " J." << std::endl;
	
	//if (!m_neighbors[data.id])
	if (m_neighbors.count(data.nodeID)==0)
	{
		m_neighbors[data.nodeID] = &data;
	}
		
}

void SOC::ScheduleSwitch()
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
		return;
	}

	//switch channel
	uint32_t chid = ++m_channelIndex % EECSim::numChannels;
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

void SOC::ScheduleTx()
{
	if (is_skipSlot)
	{
		return;
	}

	//send packet
	//if (Simulator::Now() < Time(MilliSeconds(2)))
	Vector pos = m_mobility->GetPosition ();

	cout<< m_nodeId <<" SOC begin at "<<Simulator::Now() << std::endl;

	DataPacket data;
	data.nodeID = m_nodeId; 
	data.x = pos.x;
	data.y = pos.y;
	strcpy(data.msg, "hello, this is soc!");
	data.dt = Simulator::Now();

	Broadcast(data);
}

