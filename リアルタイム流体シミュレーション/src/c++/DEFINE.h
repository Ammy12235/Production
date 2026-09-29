#pragma once

//Defineクラス:プログラムで使用する定数を格納している
class Define final {
public:
	const static wchar_t APP_NAME[100];//このアプリケーション名

	const static int WIN_W;	//ウィンドウサイズ横
	const static int WIN_H;	//ウィンドウサイズ縦

	const static float PI;	//円周率

    enum eStage {
        Stage1,
        Stage2,
        Stage3,
        Stage4,
        Stage5,
        StageEX,
        StageNum,
    };

    enum eLevel {
        Easy,
        Normal,
        Hard,
        LevelNum
    };

    const static float Gravity;
};