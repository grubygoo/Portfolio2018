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
	void  SetMType(BYTE type) { tbb::queuing_rw_mutex::scoped_lock lock(NPCLock[3], true); MType = type; }
	void  SetSpeed(BYTE speed) { tbb::queuing_rw_mutex::scoped_lock lock(NPCLock[4], true); Speed = speed; }
	void  SetPrevAttackTime(const chrono::steady_clock::time_point& t) {tbb::queuing_rw_mutex::scoped_lock lock(NPCLock[5], true); AttackTime= t;}
	void  AddMoveTime(float time) { MoveTime += time; }
	void  ResetMoveTime() { MoveTime = 0.f; }

	const bool          GetAwake() { tbb::queuing_rw_mutex::scoped_lock lock(NPCLock[0], false); return Awake; }
	const WORD          GetTarget() { tbb::queuing_rw_mutex::scoped_lock lock(NPCLock[1], false); return Target; }
	const EV_TYPE       GetEType() { tbb::queuing_rw_mutex::scoped_lock lock(NPCLock[2], false); return EType; }
	const BYTE  GetMType() { tbb::queuing_rw_mutex::scoped_lock lock(NPCLock[3], false); return MType; }
	const BYTE GetSpeed() { tbb::queuing_rw_mutex::scoped_lock lock(NPCLock[4], false); return Speed; }
	const BYTE GetMoveTime() { return MoveTime; }
	const chrono::steady_clock::time_point& GetPrevAttackTime() { tbb::queuing_rw_mutex::scoped_lock lock(NPCLock[5], false); return AttackTime; }
	void LookAtTarget(const XMFLOAT3& otherpos);
	void MapCollisionCheck(float fDelatTime);
	void CollisionMove(BYTE DIR, float fDeltaTime);
	void Move(const float& fDeltaTime);

private:
	bool              Awake;
	chrono::steady_clock::time_point AttackTime;
	BYTE              MType;
	EV_TYPE           EType;
	WORD              Target;
	BYTE              Speed;
	float             MoveTime;
	tbb::queuing_rw_mutex NPCLock[NPCLockNum];
};

