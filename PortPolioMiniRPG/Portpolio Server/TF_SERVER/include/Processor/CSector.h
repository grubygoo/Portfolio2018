#pragma once
#include "../Include.h"
class CSector
{
public:
	CSector();
	~CSector();
public:
	void AddList(unsigned int id);
	void DeleteList(unsigned int id);
	void TraverseList(const Vec2i& curpos, std::unordered_set<unsigned int>& set);
private:
	std::unordered_set<unsigned int> sectorSet;
	std::mutex sectorLock;
};

