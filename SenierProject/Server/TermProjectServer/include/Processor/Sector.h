#pragma once
//SECTOR ���͸� ������ �������� ������ 20 20 
#include "../Include.h"
class CSector 
{
public:
	CSector();
	~CSector();
	void AddList(WORD id);
	void DeleteList(WORD id);
	void TraverseList(const XMFLOAT3& pos, std::unordered_set<WORD>& set);

private:
	std::unordered_set<WORD> SectorSet;
	tbb::queuing_rw_mutex SectorLock;
};

