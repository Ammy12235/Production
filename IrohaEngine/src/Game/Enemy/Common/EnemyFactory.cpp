#include "EnemyFactory.h"
#include "Enemy/eEnemy.h"
#include "Enemy/NormalEnemy/Walker.h"

EnemyFactory::EnemyFactory()
{

}
bool EnemyFactory::Load(const LevelData& data)
{
	std::ifstream ifs;
	ifs.open(data.enemyFileName, std::ios::binary);
	if (!ifs.is_open())
	{
		MSG(L"外部ファイルが読み込めません。:EnemyFactory");
		return false;
	}

	std::string line;

	int xCount = 0;
	int yCount = 0;
	while (ifs.good()) {
		std::getline(ifs, line);

		// csvの分解処理（split）
		std::stringstream ss(line);
		std::string buff;
		while (std::getline(ss, buff, ','))
		{
			int mapId = std::atoi(buff.c_str());

			mapInfo t_Info(mapId, xCount, yCount);
			_mapList.push_back(t_Info);

			xCount++;
		}
		yCount++;
		xCount = 0;
	}
	return true;
}

bool EnemyFactory::Create()
{


	for (auto it = _mapList.begin(); it != _mapList.end();) {
	
		IdInfo map = { it->id ,"Enemy",XMFLOAT2((float)it->px * Define::ChipSize,(float)it->py * Define::ChipSize) };
		switch (map._id)
		{
		case 0:
			EntityManager::GetInstance().AddEntity(std::make_shared<Walker>(map));
			break;
		default:
			break;
		}

		it++;
	}

	return true;
}

