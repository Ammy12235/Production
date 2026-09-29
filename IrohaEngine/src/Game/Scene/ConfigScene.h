#pragma once

#include <Iroha.h>

#define BOX_NUM 200
//コンフィグシーンクラス：設定画面を実行、描画するクラス。
class ConfigScene : public Scene
{
public:

	ConfigScene(SceneListener* sceneListener, SceneEffectListener* sceneEffectListener, const Parameter& parameter);
	virtual ~ConfigScene() = default;

	bool update() override;
	bool draw() const override;

private:
	int textHandle[2] = { 0 };
	int selectNum = 0;
	float alpha = 0;
	int c = 0;
	int rx[BOX_NUM];
	int ry[BOX_NUM];
	int rcr[BOX_NUM];
	int rcb[BOX_NUM];
	

	bool isFadeOut = false;
	bool isFadeOutEnd = false;
	bool isFadeIn = false;
	bool isFadeInEnd = false;
	static const int selectAllNum = 3;
	static const int selectDetailAllNum = 6;


	typedef struct
	{
		wchar_t name[128];
		int color;
		int type;

	}Config_Element_t;

	Config_Element_t Config_Element[selectAllNum] =
	{
		{L"音",255,0},
		{L"映像",255,0},
		{L"キーコンフィグ",255,0},
		

	};

	typedef struct
	{
		wchar_t name[128];
		int color;
		int type;

	}Config_Detail_Element_t;

	Config_Detail_Element_t Config_Detail_Element[selectDetailAllNum] =
	{
		{L"BGM",255,0},
		{L"SE",255,0},
		{L"解像度",255,0},
		{L"明るさ",255,0},
		{L"詳細",255,0},
		{L"戻る",255,0}

	};
};
