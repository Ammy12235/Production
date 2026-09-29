#pragma once
#include "RectangleCollider.h"
#include "CircleCollider.h"
#include "LineCollider.h"
#include "CapsuleCollider.h"

class RectangleCollider;
class CircleCollider;
class LineCollider;
class CapsuleCollider;

//コリジョンテストクラス：各コライダーが衝突する時の計算方法を記載したクラス。
class CollisionTest
{
public:
	static bool CollideRectAndRect(const RectangleCollider& rect1, const RectangleCollider& rect2);//矩形と矩形の衝突
	static bool CollideCircleAndCircle(const CircleCollider& circle1, const CircleCollider& circle2);//円と円の衝突
	static bool CollideCircleAndRect(const CircleCollider& circle,const RectangleCollider& rect);//円と矩形の衝突
	static bool CollideLineAndLine(const LineCollider& line1,const LineCollider& line2);//線と線の衝突
	static bool CollideLineAndRect(const LineCollider& line,const RectangleCollider& rect);//線と矩形の衝突
	static bool CollideLineAndCircle(const LineCollider& line,const CircleCollider& circle);//線と円の衝突
	static bool CollideCapsuleAndCapsule(const CapsuleCollider& capsule1,const CapsuleCollider& capsule2);//カプセルとカプセルの衝突
	static bool CollideCapsuleAndRect(const CapsuleCollider& capsule, const RectangleCollider& rect);//カプセルと矩形の衝突
	static bool CollideCapsuleAndCircle(const CapsuleCollider& capsule, const CircleCollider& circle);//カプセルと円の衝突
	static bool CollideCapsuleAndLine(const CapsuleCollider& capsule, const LineCollider& line);//カプセルと線の衝突
private:
	static float Length(const XMFLOAT2& v);
	static float LengthSq(const XMFLOAT2& v);
	static float Dot(const XMFLOAT2& v1,const XMFLOAT2& v2);
	static float Cross(const XMFLOAT2& v1,const XMFLOAT2& v2);
	static bool IsSharpAngle(const XMFLOAT2& p1, const XMFLOAT2& p2,const XMFLOAT2& p3);
	static bool LineIntersect(const XMFLOAT2& p1,const XMFLOAT2& p2,const XMFLOAT2& q1,const XMFLOAT2& q2);
	static float CalcPointLineDist2D(const XMFLOAT2& p, const XMFLOAT2& lp, const XMFLOAT2& lv,OUT XMFLOAT2& h, OUT float& t);
	static float CalcPointSegmentDist2D(const XMFLOAT2& p, const XMFLOAT2& seg1, const XMFLOAT2& seg2, OUT XMFLOAT2& h, OUT float& t);
	
};