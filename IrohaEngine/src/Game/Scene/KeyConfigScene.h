#pragma once

#include <Iroha.h>

#define BOX_NUM 200
//キーコンフィグクラス：キーコンフィグ画面を実行、描画するクラス。適用ボタンで設定ファイルへ書き込みを行う。
class KeyConfigScene : public Scene
{
public:
    KeyConfigScene(SceneListener* sceneListener, SceneEffectListener* sceneEffectListener, const Parameter& parameter);
    virtual ~KeyConfigScene() = default;

    bool update() override;
    bool draw() const override;

private:
    int textHandle[2] = { 0 };
    int selectNum = 0;
    float alpha = 0;
	int c = 0;
	int rx[BOX_NUM];
	int ry[BOX_NUM];
	int rcg[BOX_NUM];
	int rcb[BOX_NUM];

	typedef struct
	{
		wchar_t name[128];
		int color;
		int type;

	}KeyConfig_Element_t;

	KeyConfig_Element_t KeyConfig_Element[9] =
	{
		{L"攻撃・決定",255,0},
		{L"ジャンプ",255,0},
		{L"ボム",255,0},
		{L"ガード",255,0},
		{L"ポーズ",255,0},
		{L"チャージ",255,0},
		{L"他",255,0},
		{L"他",255,0},
		{L"他",255,0},

	};

};
