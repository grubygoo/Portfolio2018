#pragma once
#include "../Include.h"
#include "../Processor/CSector.h"
class CObject
{

	static const int NUM_OF_LOCK = 8;
public:
	CObject();
	virtual ~CObject();
	void SetID(unsigned int id)   { tbb::queuing_rw_mutex::scoped_lock lock(objectLock[0], true); data.id = id; }
	void SetAlive(bool alive)     { tbb::queuing_rw_mutex::scoped_lock lock(objectLock[1], true); data.alive = alive; }
	void SetPos(int x, int y)     { tbb::queuing_rw_mutex::scoped_lock lock(objectLock[2], true); data.pos.x = x; data.pos.y = y; }
	void SetHP(unsigned int hp)   { tbb::queuing_rw_mutex::scoped_lock lock(objectLock[3], true); data.hp = hp; }
	void SetMP(unsigned int mp)   { tbb::queuing_rw_mutex::scoped_lock lock(objectLock[4], true); data.mp = mp; }
	void SetAtk(unsigned int atk) { tbb::queuing_rw_mutex::scoped_lock lock(objectLock[5], true); data.atk = atk; }
	void SetExp(unsigned int exp) { tbb::queuing_rw_mutex::scoped_lock lock(objectLock[6], true); data.exp = exp; }
	void SetLev(unsigned int lv)  { tbb::queuing_rw_mutex::scoped_lock lock(objectLock[7], true); data.lv = lv; }

	const int  GetID()    { tbb::queuing_rw_mutex::scoped_lock lock(objectLock[0], false); return data.id; }
	const bool GetAlive() { tbb::queuing_rw_mutex::scoped_lock lock(objectLock[1], false); return data.alive; }
	 Vec2i& GetPos()      { tbb::queuing_rw_mutex::scoped_lock lock(objectLock[2], false); return data.pos; }
	const unsigned int GetHP() { tbb::queuing_rw_mutex::scoped_lock lock(objectLock[3], false); return data.hp; }
	const unsigned int GetMP() { tbb::queuing_rw_mutex::scoped_lock lock(objectLock[4], false); return data.mp; }
	const unsigned int GetAtk() { tbb::queuing_rw_mutex::scoped_lock lock(objectLock[5], false); return data.atk; }
	const unsigned int GetExp() { tbb::queuing_rw_mutex::scoped_lock lock(objectLock[6], false); return data.exp; }
	const unsigned int GetLev() { tbb::queuing_rw_mutex::scoped_lock lock(objectLock[7], false); return data.lv; }

private:
	tbb::queuing_rw_mutex  objectLock[NUM_OF_LOCK];
	OBJECT_DATA            data;
};

