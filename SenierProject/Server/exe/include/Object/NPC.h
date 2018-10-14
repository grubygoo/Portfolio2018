#pragma once
#include "Object.h"
class CNPC :public CObject
{
	static const int  NPCLockNum = 6;

public:
	CNPC();
	virtual ~CNPC() {};
	void  SetAwake(bool check) { tbb::queuing_rw_mutex::scoped_lock lock(NPCLock[0], true); Awake = check; }
	void  SetTarget(WORD target) { tbb::queuing_rw_mutex::scoped_lock lock(NPCLock[1], true); Target = target; }
	void  SetEType(EV_TYPE type) { tbb::queuing_rw_mutex::scoped_lock lock(NPCLock[2], true); EType = type; }
	void  SetMType(MONSTER type) { tbb::queuing_rw_mutex::scoped_lock lock(NPCLock[3], true); MType = type; }
	void  SetSpeed(BYTE speed) { tbb::queuing_rw_mutex::scoped_lock lock(NPCLock[4], true); Speed = speed; }
	void  SetAttackTime(chrono::high_resolution_clock::time_point time) { tbb::queuing_rw_mutex::scoped_lock lock(NPCLock[5], true); AttackTime = time; }
	void  AddMoveTime(float time) { MoveTime += time; }
	void  ResetMoveTime() { MoveTime = 0.f; }

	const bool          GetAwake() { tbb::queuing_rw_mutex::scoped_lock lock(NPCLock[0], false); return Awake; }
	const WORD          GetTarget() { tbb::queuing_rw_mutex::scoped_lock lock(NPCLock[1], false); return Target; }
	const EV_TYPE       GetEType() { tbb::queuing_rw_mutex::scoped_lock lock(NPCLock[2], false); return EType; }
	const MONSTER  GetMType() { tbb::queuing_rw_mutex::scoped_lock lock(NPCLock[3], false); return MType; }
	const BYTE GetSpeed() { tbb::queuing_rw_mutex::scoped_lock lock(NPCLock[4], false); return Speed; }
	chrono::high_resolution_clock::time_point&  GetAttackTime() { tbb::queuing_rw_mutex::scoped_lock lock(NPCLock[5], true); return AttackTime; }
	const BYTE GetMoveTime() { return MoveTime; }

	void LookAtTarget(const XMFLOAT3& otherpos);
	void MapCollisionCheck(float fDelatTime, bool& isColl);
	void CollisionMove(BYTE DIR, float fDeltaTime);
	void Move(const float& fDeltaTime);

private:
	bool              Awake;
	MONSTER      MType;
	EV_TYPE           EType;
	WORD              Target;
	BYTE              Speed;
	float             MoveTime;
	chrono::high_resolution_clock::time_point AttackTime;
	tbb::queuing_rw_mutex NPCLock[NPCLockNum];
};

