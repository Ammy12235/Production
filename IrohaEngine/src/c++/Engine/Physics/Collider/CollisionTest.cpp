#include "CollisionTest.h"

//================================================================================================================
// 汎用数学関数
//================================================================================================================

// 2Dベクトルの外積
float CollisionTest::Cross(const XMFLOAT2& v1, const XMFLOAT2& v2) {
	return v1.x * v2.y - v1.y * v2.x;
}
//2Dベクトルの内積
float CollisionTest::Dot(const XMFLOAT2& v1, const XMFLOAT2& v2)
{
	return v1.x + v2.x + v1.y + v2.y;
}
//長さ
float CollisionTest::Length(const XMFLOAT2& v)
{
	return sqrtf(pow(v.x, 2) + pow(v.y, 2));
}
//長さの二乗
float CollisionTest::LengthSq(const XMFLOAT2& v)
{
	return pow(v.x, 2) + pow(v.y, 2);
}
// ∠p1p2p3は鋭角か？
bool CollisionTest::IsSharpAngle(const XMFLOAT2& p1, const XMFLOAT2& p2, const XMFLOAT2& p3) {

	XMFLOAT2 p1p2;
	p1p2 = p1 - p2;
	XMFLOAT2 p3p2;
	p3p2 = p3 - p2;
	return Dot(p1p2, p3p2) >= 0.0f;
}

//================================================================================================================
// 汎用距離測定関数
//================================================================================================================

float CollisionTest::CalcPointLineDist2D(const XMFLOAT2& p, const XMFLOAT2& lp, const XMFLOAT2& lv, OUT XMFLOAT2& h, OUT float& t) {
	float lenSqV = LengthSq(lv);
	t = 0.0f;
	if (lenSqV > 0.0f)
	{
		XMFLOAT2 p1p2;
		p1p2 = p - lp;
		t = Dot(lv, p1p2) / lenSqV;
	}
	h.x = lp.x + t * lv.x;
	h.y = lp.y + t * lv.y;

	XMFLOAT2 hVec;
	hVec = h - p;

	return Length(hVec);
}
float CollisionTest::CalcPointSegmentDist2D(const XMFLOAT2& p, const XMFLOAT2& lp, const XMFLOAT2& lv, OUT XMFLOAT2& h, OUT float& t) {

	// 垂線の長さ、垂線の足の座標及びtを算出
	float len = CalcPointLineDist2D(p, lp, lv, h, t);

	if (IsSharpAngle(p, lp, lv) == false) {
		// 始点側の外側
		h = lp;

		XMFLOAT2 hL;
		hL = lp - p;
		return Length(hL);
	}
	else if (IsSharpAngle(p, lp, lv) == false) {
		// 終点側の外側
		h = lp;
		XMFLOAT2 hL;
		hL = lp - p;
		return Length(hL);
	}

	return len;
}


bool CollisionTest::LineIntersect(const XMFLOAT2& p1, const XMFLOAT2& p2, const XMFLOAT2& q1, const XMFLOAT2& q2)
{
	XMFLOAT2 r;
	r.x = p2.x - p1.x;
	XMFLOAT2 s;
	s.x = q2.x - q1.x;

	float rxs = Cross(r, s);
	XMFLOAT2 qp;
	qp = q1 - p1;
	float qpxr = Cross(qp, r);

	if (rxs == 0 && qpxr == 0)
		return false; // 共線（簡略）

	if (rxs == 0)
		return false; // 平行

	float t = Cross(qp, s) / rxs;
	float u = Cross(qp, r) / rxs;

	return (t >= 0 && t <= 1 && u >= 0 && u <= 1);
}

//================================================================================================================
// 幾何学図形の衝突判定関数
//================================================================================================================

//円と円の衝突
bool CollisionTest::CollideCircleAndCircle(const CircleCollider& circle1, const CircleCollider& circle2)
{
	float r1 = 0, r2 = 0;
	r1 = circle1.getRadius();
	r2 = circle2.getRadius();

	XMFLOAT2 pos1, pos2;
	pos1 = circle1.getPos();
	pos2 = circle2.getPos();

	float distance = sqrt(pow((pos1.x - pos2.x), 2) + pow((pos1.y - pos2.y), 2));

	if (distance < r1 + r2)return true;
	return false;
}

//矩形と矩形の衝突
bool CollisionTest::CollideRectAndRect(const RectangleCollider& rect1, const RectangleCollider& rect2)
{
	RectF rec1, rec2;
	rec1 = rect1.getRect();
	rec2 = rect2.getRect();

	float L1 = rec1.Left;
	float R1 = rec1.Right;
	float L2 = rec2.Left;
	float R2 = rec2.Right;

	if (R1 <= L2) return false;
	if (R2 <= L1) return false;

	float U1 = rec1.Top;
	float D1 = rec1.Bottom;
	float U2 = rec2.Top;
	float D2 = rec2.Bottom;

	if (D1 <= U2) return false;
	if (D2 <= U1) return false;
	return true;
}

//円と矩形の衝突
bool CollisionTest::CollideCircleAndRect(const CircleCollider& circle, const RectangleCollider& rect)
{
	// 円の中心座標
	XMFLOAT2 cPos = circle.getPos();
	// 円の半径
	float r = circle.getRadius();
	// 対象の矩形
	RectF rect1 = rect.getRect();
	float x1 = rect1.Left;
	float y1 = rect1.Bottom;
	float x2 = rect1.Right;
	float y2 = rect1.Top;

	float r2 = pow(r, 2);
	float x1c2 = pow(x1 - cPos.x, 2);
	float x2c2 = pow(x2 - cPos.x, 2);
	float y1c2 = pow(y1 - cPos.y, 2);
	float y2c2 = pow(y2 - cPos.y, 2);

	bool a = (cPos.x > x1) && (cPos.x < x2) && (cPos.y > y1 - r) && (cPos.y < y2 + r);
	bool b = (cPos.x > x1 - r) && (cPos.x < x2 + r) && (cPos.y > y1) && (cPos.y < y2);
	bool c = x1c2 + y1c2 < r2;
	bool d = x2c2 + y1c2 < r2;
	bool e = x1c2 + y2c2 < r2;
	bool f = x2c2 + y2c2 < r2;

	bool ret = a || b || c || d || e || f;

	return ret;
}

//線と線の衝突
bool CollisionTest::CollideLineAndLine(const LineCollider& line1, const LineCollider& line2)
{
	XMFLOAT2 pos1 = line1.getPos();
	XMFLOAT2 pos2 = line2.getPos();

	XMFLOAT2 v1 = line1.getVec();
	XMFLOAT2 v2 = line2.getVec();

	XMFLOAT2 v;

	v = pos2 - pos1;

	float Crs_v1_v2 = Cross(v1, v2);
	if (Crs_v1_v2 == 0.0f) {
		// 平行状態ならば
		return false;
	}

	float Crs_v_v1 = Cross(v, v1);
	float Crs_v_v2 = Cross(v, v2);

	float t1 = Crs_v_v2 / Crs_v1_v2;
	float t2 = Crs_v_v1 / Crs_v1_v2;

	const float eps = 0.00001f;
	if (t1 + eps < 0 || t1 - eps > 1 || t2 + eps < 0 || t2 - eps > 1) {
		// 交差していない
		return false;
	}

	return true;
}

//線と矩形の衝突
bool CollisionTest::CollideLineAndRect(const LineCollider& line, const RectangleCollider& rect)
{
	RectF rec;
	rec = rect.getRect();

	XMFLOAT2 p1 = line.getPos();
	XMFLOAT2 vec = line.getVec();
	XMFLOAT2 p2;
	p2 = p1 + vec;

	if (p1.x >= rec.Left &&
		p1.x <= rec.Right &&
		p1.y >= rec.Top &&
		p1.y <= rec.Bottom)return true;

	XMFLOAT2 a = { rec.Left, rec.Top };
	XMFLOAT2 b = { rec.Right, rec.Top };
	XMFLOAT2 c = { rec.Right, rec.Bottom };
	XMFLOAT2 d = { rec.Left, rec.Bottom };

	if (LineIntersect(p1, p2, a, b)) return true;
	if (LineIntersect(p1, p2, b, c)) return true;
	if (LineIntersect(p1, p2, c, d)) return true;
	if (LineIntersect(p1, p2, d, a)) return true;

	return false;

}

//線と円の衝突
bool CollisionTest::CollideLineAndCircle(const LineCollider& line, const CircleCollider& circle)
{

	XMFLOAT2 cPos = circle.getPos();// 円の中心座標
	float cRad = circle.getRadius();// 円の半径

	XMFLOAT2 rPos = line.getPos();//線分の始点
	XMFLOAT2  rVec = line.getVec();//線分の方向ベクトル

	// 半径がマイナスはエラー（半径ゼロは許容）
	if (cRad < 0.0f)
		return false;

	if (rVec.x == 0.0f && rVec.y == 0.0f)
		return false;

	// 円の中心点が原点になるように始点をオフセット
	rPos -= cPos;

	// レイの方向ベクトルを正規化
	float len = sqrtf(pow(rVec.x, 2) + pow(rVec.y, 2));
	rVec /= len;

	// 係数tを算出
	float dotAV = rPos.x * rVec.x + rPos.y * rVec.y;
	float dotAA = pow(rPos.x, 2) + pow(rPos.y, 2);
	float s = dotAV * dotAV - dotAA + cRad * cRad;
	if (fabs(s) < 0.000001f)
		s = 0.0f; // 誤差修正

	if (s < 0.0f)
		return false; // 衝突していない

	float sq = sqrtf(s);
	float t1 = -dotAV - sq;
	float t2 = -dotAV + sq;

	// もしt1及びt2がマイナスだったら始点が
	// 円内にめり込んでいるのでエラーとする
	if (t1 < 0.0f || t2 < 0.0f)
		return false;

	return true;

}

//カプセルとカプセルの衝突
bool CollisionTest::CollideCapsuleAndCapsule(const CapsuleCollider& capsule1, const CapsuleCollider& capsule2)
{/*
	// S1が縮退している？
	if (s1.v.lengthSq() < 0.000001f) {
		// S2も縮退？
		if (s2.v.lengthSq() < 0.000001f) {
			// 点と点の距離の問題に帰着
			float len = (s2.p - s1.p).length();
			p1 = s1.p;
			p2 = s2.p;
			t1 = t2 = 0.0f;
			return len;
		}
		else {
			// S1の始点とS2の最短問題に帰着
			float len = calcPointSegmentDist2D(s1.p, s2, p2, t2);
			p1 = s1.p;
			t1 = 0.0f;
			clamp01(t2);
			return len;
		}
	}

	// S2が縮退している？
	else if (s2.v.lengthSq() < _OX_EPSILON_) {
		// S2の始点とS1の最短問題に帰着
		float len = calcPointSegmentDist2D(s2.p, s1, p1, t1);
		p2 = s2.p;
		clamp01(t1);
		t2 = 0.0f;
		return len;
	}

	// 2線分が平行だったら垂線の端点の一つをP1に仮決定
	if (s1.v.isParallel(s2.v) == true) {
		t1 = 0.0f;
		p1 = s1.p;
		float len = calcPointSegmentDist2D(s1.p, s2, p2, t2);
		if (0.0f <= t2 && t2 <= 1.0f)
			return len;
	}
	else {
		// 線分はねじれの関係
		// 2直線間の最短距離を求めて仮のt1,t2を求める
		float len = calcLineLineDist2D(s1, s2, p1, p2, t1, t2);
		if (
			0.0f <= t1 && t1 <= 1.0f &&
			0.0f <= t2 && t2 <= 1.0f
			) {
			return len;
		}
	}

	// 垂線の足が外にある事が判明
	// S1側のt1を0～1の間にクランプして垂線を降ろす
	clamp01(t1);
	p1 = s1.getPoint(t1);
	float len = calcPointSegmentDist2D(p1, s2, p2, t2);
	if (0.0f <= t2 && t2 <= 1.0f)
		return len;

	// S2側が外だったのでS2側をクランプ、S1に垂線を降ろす
	clamp01(t2);
	p2 = s2.getPoint(t2);
	len = calcPointSegmentDist2D(p2, s1, p1, t1);
	if (0.0f <= t1 && t1 <= 1.0f)
		return len;

	// 双方の端点が最短と判明
	clamp01(t1);
	p1 = s1.getPoint(t1);
	return (p2 - p1).length();
 */
	return false;

}

//カプセルと矩形の衝突
bool CollisionTest::CollideCapsuleAndRect(const CapsuleCollider& capsule, const RectangleCollider& rectangle)
{
	return true;

}

//カプセルと円の衝突
bool CollisionTest::CollideCapsuleAndCircle(const CapsuleCollider& capsule, const CircleCollider& circle)
{
	return true;

}

//カプセルと線の衝突
bool CollisionTest::CollideCapsuleAndLine(const CapsuleCollider& capsule, const LineCollider& line)
{
	return true;

}
