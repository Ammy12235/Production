#pragma once

template <typename _T>

//Singletonクラス：スタック領域に保持するシングルトンクラス。
class Singleton {

protected:
	Singleton() = default;
	virtual ~Singleton() = default;
	Singleton(const Singleton& r) = default;
	Singleton& operator=(const Singleton& r) = default;

public:
	static _T* GetInstance() {
		static _T inst;
		return &inst;
	};

};