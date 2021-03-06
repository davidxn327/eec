
#ifndef __APP_H__
#define __APP_H__

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/application.h"
#include "ns3/stats-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/energy-module.h"
#include "ns3/wifi-module.h"

#include "sim.h"

using namespace ns3;
using namespace std;

extern int sum_rxdrop;
extern int sum_txdrop;
void RxDrop(string context, Ptr<const Packet> p);
void TxDrop(string context, Ptr<const Packet> p);

typedef struct _dataPacket
{
	uint32_t nodeID;
	int flag;
	double cost;
	uint32_t ch;
	double x;
	double y;
	Time dt;
	uint16_t channel;
	void *dummy;
	char msg[32];
} DataPacket;

class Algorithm : public Application
{
public:
	Algorithm ();
	virtual ~Algorithm ();

	static TypeId GetTypeId(void);

	static double GetNextRandom();
	void ScheduleNextSlot();
	virtual void ScheduleSwitch();
	virtual void ScheduleTx();
	virtual void HandleRecv(DataPacket &data);
	virtual void Procedure();

protected:
	virtual void DoDispose(void);

	void Send(string addr, DataPacket &data);
	void Broadcast(DataPacket &data);
	void SwitchChannel(uint32_t index);
	void Stop();

	Ptr<Socket> m_socket;
	PacketSocketAddress m_psAddr;
	EventId m_processEvent;

	Ptr<Node> m_node;
	Ptr<YansWifiPhy> m_phy;
	Ptr<WifiNetDevice> m_device;
	Ptr<EnergySource> m_battery;
	Ptr<MobilityModel> m_mobility;

	uint32_t m_nodeId;
	uint32_t m_channelIndex;
	std::set<uint32_t> m_channels;
	uint32_t m_switchtime;//500us
	uint32_t m_txtime;//200ms
	uint32_t m_slot;

	bool is_skipSlot;
	bool is_stop;

private:
	static Ptr<UniformRandomVariable> urv;
	virtual void StartApplication (void);
	virtual void StopApplication (void);
	void Receive( Ptr<Socket> socket);
};

#endif
