#include "Object.h"




CObject::CObject() :OOBB(nullptr)
{
	OOBB = new BoundingOrientedBox(XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(0, 0, 0, 1));
}

CObject::~CObject()
{
	SAFE_DELETE(OOBB);
}

bool CObject::CheckCollision( BoundingOrientedBox& other)
{   
	ContainmentType Type = GetOOBB()->Contains(other);
	if (Type == INTERSECTS  || Type == CONTAINS) return true; 
	 return false;
}

