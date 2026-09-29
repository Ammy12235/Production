#pragma once
# include <DirectXMath.h>
# include <initializer_list>
# include <array>
# include <cmath>
# include <cassert>
# include <limits>
# include <algorithm>
# include <numeric>

//-------------------------------------------------------------------------------------------------------------
//. XMFLOAT2系
//-------------------------------------------------------------------------------------------------------------
	static inline void operator+= (DirectX::XMFLOAT2& v1, const std::initializer_list<float>& v2)
	{
		assert(v2.size() == 2u && "不正なイニシャライザーリスト");

		auto itr{ v2.begin() };
		v1.x += *itr;
		itr++;
		v1.y += *itr;
	}

	static inline void operator-= (DirectX::XMFLOAT2& v1, const std::initializer_list<float>& v2)
	{
		assert(v2.size() == 2u && "不正なイニシャライザーリスト");

		auto itr{ v2.begin() };
		v1.x -= *itr;
		itr++;
		v1.y -= *itr;
	}

	static inline void operator/= (DirectX::XMFLOAT2& v1, const std::initializer_list<float>& v2)
	{
		assert(v2.size() == 2u && "不正なイニシャライザーリスト");

		auto itr{ v2.begin() };
		v1.x /= *itr;
		itr++;
		v1.y /= *itr;
	}

	static inline void operator*= (DirectX::XMFLOAT2& v1, const std::initializer_list<float>& v2)
	{
		assert(v2.size() == 2u && "不正なイニシャライザーリスト");

		auto itr{ v2.begin() };
		v1.x *= *itr;
		itr++;
		v1.y *= *itr;
	}

	static inline void operator%= (DirectX::XMFLOAT2& v1, const std::initializer_list<float>& v2)
	{
		assert(v2.size() == 2u && "不正なイニシャライザーリスト");

		auto itr{ v2.begin() };
		v1.x = ::fmodf(v1.x, *itr);
		itr++;
		v1.y = ::fmodf(v1.y, *itr);
	}

	_NODISCARD static inline auto operator+ (const DirectX::XMFLOAT2& v1, const std::initializer_list<float>& v2)
	{
		assert(v2.size() == 2u && "不正なイニシャライザーリスト");

		DirectX::XMFLOAT2 temp{ v1 };
		auto itr{ v2.begin() };

		temp.x += *itr;
		itr++;
		temp.y += *itr;

		return temp;
	}

	_NODISCARD static inline auto operator- (const DirectX::XMFLOAT2& v1, const std::initializer_list<float>& v2)
	{
		assert(v2.size() == 2u && "不正なイニシャライザーリスト");

		DirectX::XMFLOAT2 temp{ v1 };
		auto itr{ v2.begin() };

		temp.x -= *itr;
		itr++;
		temp.y -= *itr;

		return temp;
	}

	_NODISCARD static inline auto operator* (const DirectX::XMFLOAT2& v1, const std::initializer_list<float>& v2)
	{
		assert(v2.size() == 2u && "不正なイニシャライザーリスト");

		DirectX::XMFLOAT2 temp{ v1 };
		auto itr{ v2.begin() };

		temp.x *= *itr;
		itr++;
		temp.y *= *itr;

		return temp;
	}

	_NODISCARD static inline auto operator/ (const DirectX::XMFLOAT2& v1, const std::initializer_list<float>& v2)
	{
		assert(v2.size() == 2u && "不正なイニシャライザーリスト");

		DirectX::XMFLOAT2 temp{ v1 };
		auto itr{ v2.begin() };

		temp.x /= *itr;
		itr++;
		temp.y /= *itr;

		return temp;
	}

	_NODISCARD static inline auto operator% (const DirectX::XMFLOAT2& v1, const std::initializer_list<float>& v2)
	{
		assert(v2.size() == 2u && "不正なイニシャライザーリスト");

		DirectX::XMFLOAT2 temp{ v1 };
		auto itr{ v2.begin() };

		temp.x = ::fmodf(v1.x, *itr);
		itr++;
		temp.y = ::fmodf(v1.y, *itr);

		return temp;
	}

	static inline void operator+= (DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2)
	{
		v1.x += v2.x;
		v1.y += v2.y;
	}

	static inline void operator-= (DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2)
	{
		v1.x -= v2.x;
		v1.y -= v2.y;
	}

	static inline void operator/= (DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2)
	{
		v1.x /= v2.x;
		v1.y /= v2.y;
	}

	static inline void operator*= (DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2)
	{
		v1.x *= v2.x;
		v1.y *= v2.y;
	}

	static inline void operator%= (DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2)
	{
		v1.x = ::fmodf(v1.x, v2.x);
		v1.y = ::fmodf(v1.y, v2.y);
	}

	_NODISCARD static inline constexpr auto operator+ (const DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2)
	{
		return DirectX::XMFLOAT2{ v1.x + v2.x, v1.y + v2.y };
	}

	_NODISCARD static inline constexpr auto operator- (const DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2)
	{
		return DirectX::XMFLOAT2{ v1.x - v2.x, v1.y - v2.y };
	}

	_NODISCARD static inline constexpr auto operator* (const DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2)
	{
		return DirectX::XMFLOAT2{ v1.x * v2.x, v1.y * v2.y };
	}

	_NODISCARD static inline constexpr auto operator/ (const DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2)
	{
		return DirectX::XMFLOAT2{ v1.x / v2.x, v1.y / v2.y };
	}

	_NODISCARD static inline constexpr auto operator% (const DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2)
	{
		return DirectX::XMFLOAT2{ ::fmodf(v1.x, v2.x), ::fmodf(v1.y, v2.y) };
	}

	static inline void operator+= (DirectX::XMFLOAT2& v1, const float num)
	{
		v1.x += num;
		v1.y += num;
	}

	static inline void operator-= (DirectX::XMFLOAT2& v1, const float num)
	{
		v1.x -= num;
		v1.y -= num;
	}

	static inline void operator/= (DirectX::XMFLOAT2& v1, const float num)
	{
		v1.x /= num;
		v1.y /= num;
	}

	static inline void operator*= (DirectX::XMFLOAT2& v1, const float num)
	{
		v1.x *= num;
		v1.y *= num;
	}

	static inline void operator%= (DirectX::XMFLOAT2& v1, const float num)
	{
		v1.x = ::fmodf(v1.x, num);
		v1.y = ::fmodf(v1.y, num);
	}

	_NODISCARD static inline constexpr auto operator+ (const DirectX::XMFLOAT2& v1, const float num)
	{
		return DirectX::XMFLOAT2{ v1.x + num, v1.y + num };
	}

	_NODISCARD static inline constexpr auto operator- (const DirectX::XMFLOAT2& v1, const float num)
	{
		return DirectX::XMFLOAT2{ v1.x - num, v1.y - num };
	}

	_NODISCARD static inline constexpr auto operator* (const DirectX::XMFLOAT2& v1, const float num)
	{
		return DirectX::XMFLOAT2{ v1.x * num, v1.y * num };
	}

	_NODISCARD static inline constexpr auto operator/ (const DirectX::XMFLOAT2& v1, const float num)
	{
		return DirectX::XMFLOAT2{ v1.x / num, v1.y / num };
	}

	_NODISCARD static inline constexpr auto operator% (const DirectX::XMFLOAT2& v1, const float num)
	{
		return DirectX::XMFLOAT2{ ::fmodf(v1.x, num), ::fmodf(v1.y, num) };
	}

	_NODISCARD static inline constexpr auto operator+ (const float num, DirectX::XMFLOAT2& v1)
	{
		return DirectX::XMFLOAT2{ num + v1.x, num + v1.y };
	}

	_NODISCARD static inline constexpr auto operator- (const float num, DirectX::XMFLOAT2& v1)
	{
		return DirectX::XMFLOAT2{ num - v1.x, num - v1.y };
	}

	_NODISCARD static inline constexpr auto operator* (const float num, DirectX::XMFLOAT2& v1)
	{
		return DirectX::XMFLOAT2{ num * v1.x, num * v1.y };
	}

	_NODISCARD static inline constexpr auto operator/ (const float num, DirectX::XMFLOAT2& v1)
	{
		return DirectX::XMFLOAT2{ num / v1.x, num / v1.y };
	}

	_NODISCARD static inline constexpr auto operator% (const float num, DirectX::XMFLOAT2& v1)
	{
		return DirectX::XMFLOAT2{ fmodf(num, v1.x), fmodf(num, v1.y) };
	}

	_NODISCARD static inline constexpr bool operator< (const DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2)
	{
		return ((v1.x < v2.x) && (v1.y < v2.y));
	}

	_NODISCARD static inline constexpr bool operator> (const DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2)
	{
		return ((v1.x > v2.x) && (v1.y > v2.y));
	}


	_NODISCARD static inline constexpr auto operator- (const DirectX::XMFLOAT2& v1)
	{
		return DirectX::XMFLOAT2{ -v1.x, -v1.y };
	}

inline namespace XMM
{
	// クランプ---------------------------------------------------------------------------------------------------
	_NODISCARD static inline auto Clamp(const DirectX::XMFLOAT2& vf2, const DirectX::XMFLOAT2& low, const DirectX::XMFLOAT2& high)
	{
		DirectX::XMFLOAT2 temp{};

		temp.x = std::clamp(vf2.x, low.x, high.x);
		temp.y = std::clamp(vf2.y, low.y, high.y);

		return temp;
	}

	// マックス---------------------------------------------------------------------------------------------------
	_NODISCARD static inline auto Max(const DirectX::XMFLOAT2& left, const DirectX::XMFLOAT2& right)
	{
		DirectX::XMFLOAT2 temp{};

		temp.x = (std::max)(left.x, right.x);
		temp.y = (std::max)(left.y, right.y);

		return temp;
	}

	_NODISCARD static inline auto Max(const DirectX::XMFLOAT2& left, const float right)
	{
		DirectX::XMFLOAT2 temp{};

		temp.x = (std::max)(left.x, right);
		temp.y = (std::max)(left.y, right);

		return temp;
	}

	// ミニ---------------------------------------------------------------------------------------------------
	_NODISCARD static inline auto Min(const DirectX::XMFLOAT2& left, const DirectX::XMFLOAT2& right)
	{
		DirectX::XMFLOAT2 temp{};

		temp.x = (std::min)(left.x, right.x);
		temp.y = (std::min)(left.y, right.y);

		return temp;
	}

	_NODISCARD static inline auto Min(const DirectX::XMFLOAT2& left, const float right)
	{
		DirectX::XMFLOAT2 temp{};

		temp.x = (std::min)(left.x, right);
		temp.y = (std::min)(left.y, right);

		return temp;
	}

	// 2 を底とする指数関数---------------------------------------------------------------------------------------------------
		// 2 の 引数 乗を返す。

	_NODISCARD static inline auto Exp2(const DirectX::XMFLOAT2& vf2)
	{
		DirectX::XMFLOAT2 temp{};

		temp.x = exp2f(vf2.x);
		temp.y = exp2f(vf2.y);

		return temp;
	}

	// 2 を底とする二進対数---------------------------------------------------------------------------------------------------
		// 引数 の 2 を底とする二進対数を返す。（引数が２の何乗かを返す）

	_NODISCARD static inline auto Log2(const DirectX::XMFLOAT2& vf2)
	{
		DirectX::XMFLOAT2 temp{};

		temp.x = log2f(vf2.x);
		temp.y = log2f(vf2.y);

		return temp;
	}

	// 2 を底とする二進対数---------------------------------------------------------------------------------------------------
		// 引数 x の 10 を底とする常用対数を返す。(引数が10の何乗かを返す)

	_NODISCARD static inline auto Log10(const DirectX::XMFLOAT2& vf2)
	{
		DirectX::XMFLOAT2 temp{};

		temp.x = log10f(vf2.x);
		temp.y = log10f(vf2.y);

		return temp;
	}

	// 切り上げ---------------------------------------------------------------------------------------------------

	_NODISCARD static inline auto Ceil(const DirectX::XMFLOAT2& vf2)
	{
		DirectX::XMFLOAT2 temp{};

		temp.x = ceilf(vf2.x);
		temp.y = ceilf(vf2.y);

		return temp;
	}

	// 切り捨て---------------------------------------------------------------------------------------------------

	_NODISCARD static inline auto Floor(const DirectX::XMFLOAT2& vf2)
	{
		DirectX::XMFLOAT2 temp{};

		temp.x = floorf(vf2.x);
		temp.y = floorf(vf2.y);

		return temp;
	}

	// ゼロ方向への丸め---------------------------------------------------------------------------------------------------

	_NODISCARD static inline auto Trunc(const DirectX::XMFLOAT2& vf2)
	{
		DirectX::XMFLOAT2 temp{};

		temp.x = truncf(vf2.x);
		temp.y = truncf(vf2.y);

		return temp;
	}

	// 四捨五入---------------------------------------------------------------------------------------------------

	_NODISCARD static inline auto Round(const DirectX::XMFLOAT2& vf2)
	{
		DirectX::XMFLOAT2 temp{};

		temp.x = roundf(vf2.x);
		temp.y = roundf(vf2.y);

		return temp;
	}

	//. XMFFLOATの全ての変数に同じ値を代入-------------------------------------------------------------------------

	_NODISCARD static constexpr inline auto Fill2(const float num)
	{
		DirectX::XMFLOAT2 temp{ num, num };

		return temp;
	}

	// 累乗---------------------------------------------------------------------------------------------------

	_NODISCARD static inline auto Pow(const DirectX::XMFLOAT2& vf2, const float pow_num)
	{
		DirectX::XMFLOAT2 temp{};

		temp.x = powf(vf2.x, pow_num);
		temp.y = powf(vf2.y, pow_num);

		return temp;
	}

	// 平方根---------------------------------------------------------------------------------------------------

	_NODISCARD static inline auto Sqrt(const DirectX::XMFLOAT2& vf2)
	{
		DirectX::XMFLOAT2 temp{};

		temp.x = sqrtf(vf2.x);
		temp.y = sqrtf(vf2.y);

		return temp;
	}

	// 立方根---------------------------------------------------------------------------------------------------

	_NODISCARD static inline auto Cbrt(const DirectX::XMFLOAT2& vf2)
	{
		DirectX::XMFLOAT2 temp{};

		temp.x = cbrtf(vf2.x);
		temp.y = cbrtf(vf2.y);

		return temp;
	}

	// 絶対値---------------------------------------------------------------------------------------------------

	_NODISCARD static inline auto FAbs(const DirectX::XMFLOAT2& vf2)
	{
		DirectX::XMFLOAT2 temp{};

		temp.x = fabs(vf2.x);
		temp.y = fabs(vf2.y);

		return temp;
	}

	// ０クリアー--------------------------------------------------------------------------------------------------

	static inline void Clear(DirectX::XMFLOAT2& vf2)
	{
		vf2 = { 0.f, 0.f };
	}

	// 正規化-----------------------------------------------------------------------------------------------------

	_NODISCARD static inline auto Normalize(const DirectX::XMFLOAT2& vf2)
	{
		DirectX::XMFLOAT2 rv{ vf2 };

		auto&& vec{ DirectX::XMLoadFloat2(&rv) };

		DirectX::XMStoreFloat2(&rv, DirectX::XMVector2Normalize(vec));

		return rv;
	}

	// ベクトルの長さを取得----------------------------------------------------------------------------------------------------

	_NODISCARD static inline float Length(const DirectX::XMFLOAT2& vf2)
	{
		float len{};

		auto&& vec{ DirectX::XMLoadFloat2(&vf2) };

		DirectX::XMStoreFloat(&len, DirectX::XMVector2Length(vec));

		return len;
	}

	// ベクトルの長さの二乗を取得----------------------------------------------------------------------------------------------------

	_NODISCARD static inline float LengthSq(const DirectX::XMFLOAT2& vf2)
	{
		float len{};

		auto&& vec{ DirectX::XMLoadFloat2(&vf2) };

		DirectX::XMStoreFloat(&len, DirectX::XMVector2LengthSq(vec));

		return len;
	}

	// ベクトルの直行するベクトルを計算----------------------------------------------------------------------------------------------------

	_NODISCARD static inline auto Orthogonal(const DirectX::XMFLOAT2& vf2)
	{
		DirectX::XMFLOAT2 rv{};

		auto&& vec{ DirectX::XMLoadFloat2(&vf2) };

		DirectX::XMStoreFloat2(&rv, DirectX::XMVector2Orthogonal(vec));

		return rv;
	}

	// ベクトルの内積を計算----------------------------------------------------------------------------------------------------

	_NODISCARD static inline auto Dot(const DirectX::XMFLOAT2& vec1, const DirectX::XMFLOAT2& vec2)
	{
		float rv{};

		const auto&& v1{ DirectX::XMLoadFloat2(&vec1) }, && v2{ DirectX::XMLoadFloat2(&vec2) };

		DirectX::XMStoreFloat(&rv, DirectX::XMVector2Dot(v1, v2));

		return rv;
	}

	// ベクトルの外積を計算----------------------------------------------------------------------------------------------------

	_NODISCARD static inline auto Cross(const DirectX::XMFLOAT2& vec1, const DirectX::XMFLOAT2& vec2)
	{
		float rv{};

		const auto&& v1{ DirectX::XMLoadFloat2(&vec1) }, && v2{ DirectX::XMLoadFloat2(&vec2) };

		DirectX::XMStoreFloat(&rv, DirectX::XMVector2Cross(v1, v2));

		return rv;
	}

	// ベクトル間のラジアン角度を計算-----------------------------------------------------------------------------------------

	_NODISCARD static inline float RadDiff(const DirectX::XMFLOAT2& vf2_left, const DirectX::XMFLOAT2& vf2_right)
	{
		float rad{};

		auto&& l_vec{ DirectX::XMLoadFloat2(&vf2_left) }, && r_vec{ DirectX::XMLoadFloat2(&vf2_right) };

		DirectX::XMStoreFloat(&rad, DirectX::XMVector2AngleBetweenVectors(l_vec, r_vec));

		return rad;
	}


	_NODISCARD static inline constexpr auto DegToRad(const DirectX::XMFLOAT2& degree)
	{
		DirectX::XMFLOAT2 rv{};

		rv.x = DirectX::XMConvertToRadians(degree.x);
		rv.y = DirectX::XMConvertToRadians(degree.y);

		return rv;
	}

	_NODISCARD static inline constexpr auto RadToDeg(const DirectX::XMFLOAT2& radian)
	{
		DirectX::XMFLOAT2 rv{};

		rv.x = DirectX::XMConvertToDegrees(radian.x);
		rv.y = DirectX::XMConvertToDegrees(radian.y);

		return rv;
	}

	// 基準座標から他の座標への角度を求める--------------------------------------------------------------

	_NODISCARD static inline float Atan2(const DirectX::XMFLOAT2& base_pos,
		const DirectX::XMFLOAT2& another_pos)
	{
		return ::atan2f(another_pos.x - base_pos.x, another_pos.y - base_pos.y);
	}

	// 距離を求める-------------------------------------------------------------------------------

	_NODISCARD static inline float Distance(const DirectX::XMFLOAT2& pos1, const DirectX::XMFLOAT2& pos2)
	{
		return (::sqrtf(::powf(pos2.x - pos1.x, 2.f) + ::powf(pos2.y - pos1.y, 2.f)));
	}

	_NODISCARD static inline float DistanceSq(const DirectX::XMFLOAT2& pos1, const DirectX::XMFLOAT2& pos2)
	{
		return (::powf(pos2.x - pos1.x, 2.f) + ::powf(pos2.y - pos1.y, 2.f));
	}

	// 二点間の中点を求める----------------------------------------------------------------------------------

	_NODISCARD static inline DirectX::XMFLOAT2 Mid(const DirectX::XMFLOAT2& pos1, const DirectX::XMFLOAT2& pos2)
	{
		return { ((pos1.x + pos2.x) / 2.f), ((pos1.y + pos2.y) / 2.f) };
	}

	_NODISCARD static inline float ManhattanDistance(const DirectX::XMFLOAT2& pos1, const DirectX::XMFLOAT2& pos2)
	{
		auto FAbs{ [](const auto& vf3, const auto& vf3b) {
			DirectX::XMFLOAT2 temp{};

			temp.x = fabs(vf3.x - vf3b.x);
			temp.y = fabs(vf3.y - vf3b.y);

			return temp;
		} };

		const auto&& dis_vec3{ FAbs(pos2, pos1) };

		return (dis_vec3.x + dis_vec3.y);
	}
}
