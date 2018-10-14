#pragma once
#include "../Include.h"
class CSector;
class CObject
{
	static const int ObjectLockNum = 10;
public:
	CObject();
	virtual ~CObject();
	void  SetID(WORD id) { tbb::queuing_rw_mutex::scoped_lock lock(ObjectLock[0], true); Data.ID = id; }
	void  SetHp(float  hp) { tbb::queuing_rw_mutex::scoped_lock lock(ObjectLock[1], true); Data.HP = hp; }
	void  SetMp(float  mp) { tbb::queuing_rw_mutex::scoped_lock lock(ObjectLock[2], true); Data.MP = mp; }
	void  SetAtk(float atk) { tbb::queuing_rw_mutex::scoped_lock lock(ObjectLock[3], true); Data.ATK = atk; }
	void  SetPos(const XMFLOAT3& pos) { tbb::queuing_rw_mutex::scoped_lock lock(ObjectLock[4], true); Data.POS = pos; }
	void  SetDir(WORD look) { tbb::queuing_rw_mutex::scoped_lock lock(ObjectLock[5], true); Data.DIR = look; }
	void  SetState(BYTE state) { tbb::queuing_rw_mutex::scoped_lock lock(ObjectLock[6], true); Data.STATE = state; }
	void  SetAlive(bool check) { tbb::queuing_rw_mutex::scoped_lock lock(ObjectLock[7], true); Data.ALIVE = check; }
	void  SetStage(BYTE stage) { tbb::queuing_rw_mutex::scoped_lock lock(ObjectLock[8], true); Data.STAGE = stage; }
	void  SetOOBB(const XMFLOAT3& pos) { tbb::queuing_rw_mutex::scoped_lock lock(ObjectLock[9], true); OOBB->Center = pos; }

	const WORD      GetID() { tbb::queuing_rw_mutex::scoped_lock lock(ObjectLock[0], false); return Data.ID; }
	const float     GetHp() { tbb::queuing_rw_mutex::scoped_lock lock(ObjectLock[1], false); return Data.HP; }
	const float     GetMp() { tbb::queuing_rw_mutex::scoped_lock lock(ObjectLock[2], false); return Data.MP; }
	const float     GetAtk() { tbb::queuing_rw_mutex::scoped_lock lock(ObjectLock[3], false); return Data.ATK; }
	const XMFLOAT3& GetPos() { tbb::queuing_rw_mutex::scoped_lock lock(ObjectLock[4], false); return Data.POS; }
	const WORD      GetDir() { tbb::queuing_rw_mutex::scoped_lock lock(ObjectLock[5], false); return Data.DIR; }
	const BYTE      GetState() { tbb::queuing_rw_mutex::scoped_lock lock(ObjectLock[6], false); return Data.STATE; }
	const bool      GetAlive() { tbb::queuing_rw_mutex::scoped_lock lock(ObjectLock[7], false); return Data.ALIVE; }
	const BYTE      GetStage() { tbb::queuing_rw_mutex::scoped_lock lock(ObjectLock[8], false); return Data.STAGE; }
	const BoundingOrientedBox* GetOOBB() { tbb::queuing_rw_mutex::scoped_lock lock(ObjectLock[9], false); return OOBB; }
	bool   CheckCollision(BoundingOrientedBox& other);
private:
	OBJECT_DATA          Data;
	BoundingOrientedBox* OOBB;
	high_resolution_clock::time_point  ExecuteTime;
	tbb::queuing_rw_mutex ObjectLock[ObjectLockNum];
};

