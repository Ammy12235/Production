#pragma once
#include <Iroha.h>

//使い道はマネージャクラスへの伝達。マネージャークラスをシングルトンにしない場合、マネージャークラスに通知する機構として用いる
template <class _T>
class Observer
{
public:
	Observer() {}
	virtual ~Observer() {};
	virtual bool update(_T* subject) = 0;

private:
protected:
};

template <class _T>
class Subject
{
protected:
public:
	Subject() {}
	virtual ~Subject() {}
	void attach(Observer<_T>* observer)
	{
		_observers.push_back(observer);
	}
	void  detach(Observer<_T>* observer) {
		auto it = std::find(_observers.begin(), _observers.end(), observer);
		if (it != _observers.end())
			_observers.erase(it);
	}

	// notify() メソッドの修正
	void notify()
	{
		for (const auto& o : _observers) {
			o->update(dynamic_cast<_T*>(this));
		}
	}
private:
	std::vector<Observer<_T>*> _observers;

};
