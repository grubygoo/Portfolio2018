#pragma once
#include "Object.h"
class CNPC :public CObject
{
	static const int  NUM_OF_LOCK = 4;
public:
	CNPC();
	virtual ~CNPC();
	void  SetAwake(bool check) { tbb::queuing_rw_mutex::scoped_lock lock(npcLock[0], true); awake = check; }
	void  SetTarget(unsigned int target) { tbb::queuing_rw_mutex::scoped_lock lock(npcLock[1], true); Target = target; }
	void  SetEType(EV_TYPE type) { tbb::queuing_rw_mutex::scoped_lock lock(npcLock[2], true); eType = type; }
	void  SetMType(BYTE type) { tbb::queuing_rw_mutex::scoped_lock lock(npcLock[3], true); mType = type; }

	const bool GetAwake() { tbb::queuing_rw_mutex::scoped_lock lock(npcLock[0], false); return awake; }
	const unsigned int GetTarget() { tbb::queuing_rw_mutex::scoped_lock lock(npcLock[1], false); return Target; }
	const EV_TYPE GetEType() { tbb::queuing_rw_mutex::scoped_lock lock(npcLock[2], false); return eType; }
	const BYTE  GetMType() { tbb::queuing_rw_mutex::scoped_lock lock(npcLock[3], false); return mType; }

	void Move();

private:
	bool awake;
	BYTE mType;
	unsigned int Target;
	EV_TYPE eType;
	tbb::queuing_rw_mutex  npcLock[NUM_OF_LOCK];
};

