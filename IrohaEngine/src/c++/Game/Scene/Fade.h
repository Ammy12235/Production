#pragma once
#include <Iroha.h>


class Fade:public SceneEffect
{
public:
	Fade() = default;
	~Fade() = default;

	void update() override;
	void draw() const override;

	bool isFade = false;
	bool isFadeEnd = false;
	float fadeTimer = 0.0f;
	const float fadeInterval = 0.8f;//second
private:
	float alpha = 0;
protected:

};
