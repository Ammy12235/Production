#include "SoundStorage.h"
#include "Core/DEFINE.h"

void SoundStorage::deleteSound(UINT32 id)
{
	for (auto it = _sounds.begin(); it != _sounds.end();)
	{
		if (it->get()->getId() == id)//一致したらそれを削除する
		{
			it = _sounds.erase(it);
			return;
		}
	}
}

bool SoundStorage::update()
{/*
	//ミキシングなどの更新
	for (auto it = _sounds.begin(); it != _sounds.end();)
	{
		if ((*it)->_isDelete)//falseなら削除する
		{
			it = _sounds.erase(it);
		}
		else
		{
			(*it)->update();
			it++;
		}

	}


	

 */
	return true;
}

bool SoundStorage::draw()const
{
	/*
	if (Define::Debug)
	{
		for (auto it = _colliders.begin(); it != _colliders.end();)
		{
			(*it)->draw();
			it++;
		}
	}
	*/
	return true;
}

//コライダーの総数を表示する
void SoundStorage::drawColliderDebug()const
{
	if (Define::Debug)
	{
		/*
		WCHAR str[128];

		int num = _colliders.size();
		swprintf(str, 128, L"ColliderNum:%d", num);
		DWRITE.DrawFormatText(str, 0, 130, 300, 100, D3D.GetColor(255, 255, 255), 1, 1);

		num = calculateNum;
		swprintf(str, 128, L"CulculateNum:%d", num);
		DWRITE.DrawFormatText(str, 0, 160, 300, 100, D3D.GetColor(255, 255, 255), 1, 1);

		num = collidingNum;
		swprintf(str, 128, L"CollidingNum:%d", num);
		DWRITE.DrawFormatText(str, 0, 190, 300, 100, D3D.GetColor(255, 255, 255), 1, 1);
		*/
	}
}
