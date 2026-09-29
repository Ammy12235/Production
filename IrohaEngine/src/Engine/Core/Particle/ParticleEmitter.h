#pragma once

#include "Core/Base.h"
#include "Core/Component.h"

class ParticleEmitter :public IComponent
{
private:

	//パーティクルごとのパラメーター
	struct ParticleParameter
	{
		float lifeTime;//寿命
		float speed;//速度
		float angle;//角度
		XMFLOAT2 size;//サイズ
		float alpha;//アルファ値
		float rotationSpeed;//回転速度
		
	};

	struct EmitterParameter
	{
		float spawnRate;//発生率
		float spawnRadius;//発生半径
		float spawnAngle;//発生角度
		float attenuationVelocity;//減衰率
		float attenuationAngle;//減衰率
		int texId;//テクスチャID
		bool isLoop = false;//ループするかどうか
		bool isPlaying = false;//再生中かどうか
		float gravity;//重力
		bool isFollowDirection;//発生方向に追従するかどうか
		bool isFollowEmitter;//エミッターに追従するかどうか

	};
public:
	ParticleEmitter() = default;
	~ParticleEmitter() = default;

	void Start()const;
	void Stop()const;

	std::vector<ParticleParameter> _particles;//パーティクルのパラメーターの配列

};
