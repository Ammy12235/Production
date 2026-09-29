#pragma once
#include "Core/Base.h"

class EngineLoop final
{
public:
	bool InitializeEngine(HINSTANCE hInstance); // エンジンの初期化
	bool ExecuteEngineLoop()const;// エンジンのループ処理
	void FinalizeEngine();// エンジンの終了処理

	void CleanupAllSystem()const;//全てのシステムの管理オブジェクトを削除

	EngineLoop()=default;
	virtual ~EngineLoop()=default;
private:

protected:
	EngineLoop(const  EngineLoop& r) = default;
	EngineLoop& operator=(const  EngineLoop& r) = default;

	static inline EngineLoop* s_instance;

public:


	static void CreateInstance() {
		if (!s_instance) {
			s_instance = new EngineLoop;
		}
	}

	static void DeleteInstance() {
		delete s_instance;
		s_instance = nullptr;
	}
	static EngineLoop& GetInstance() {
		return *s_instance;
	}

};
