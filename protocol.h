#ifndef __MY_PROTOCOL_H__
#define __MY_PROTOCOL_H__

#include "app.h"

class EEC : public Algorithm 
{
public:
	EEC ();
	virtual ~EEC ();

	virtual void ScheduleNext();
	virtual void ScheduleTx();
	virtual void HandleRecv(DataPacket &data);

private:
	//
};

class SOC : public Algorithm
{
public:
	SOC ();
	virtual ~SOC ();

	virtual void ScheduleNext();
	virtual void ScheduleTx();
	virtual void HandleRecv(DataPacket &data);

private:
	//
};

class CogMesh : public Algorithm
{
public:
	CogMesh ();
	virtual ~CogMesh ();

	virtual void ScheduleTx();
	virtual void HandleRecv(DataPacket &data);

private:
	//
};

#endif
