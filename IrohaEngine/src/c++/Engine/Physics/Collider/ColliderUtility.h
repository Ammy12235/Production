#pragma once

enum eCollideResult
{
	Collide_None,
	Collide_Bigin,
	Collide_Colliding,
	Collide_End,
};

struct CollideState
{
	bool collide = false;
	bool preCollide = false;
	eCollideResult result;
};