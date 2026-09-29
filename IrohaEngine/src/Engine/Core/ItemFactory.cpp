#include "ItemFactory.h"
#include "eItem.h"
#include "EntityManager.h"
#include "Item/MedicalKit.h"
#include "Item/Sugar.h"

ItemFactory::ItemFactory()
{

}
bool ItemFactory::Load(const LevelData& data)
{
	std::ifstream ifs;
	ifs.open(data.itemFileName, std::ios::binary);
	if (!ifs.is_open())
	{
		MSG(L"外部ファイルが読み込めません。:ItemFactory");
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
			if (mapId > Item_None && mapId < Item_Max)
			{
				mapInfo t_Info(mapId, xCount, yCount);
				_mapList.push_back(t_Info);
			}
			xCount++;
		}
		yCount++;
		xCount = 0;
	}
	return true;
}

bool ItemFactory::Create()
{
	for (auto it = _mapList.begin(); it != _mapList.end();) {
		if ((*it).id<Item_None + 1 || (*it).id >Item_Max) {
			continue;
		}
		IdInfo map = { it->id ,"Item",XMFLOAT2((float)it->px * Define::ChipSize,(float)it->py * Define::ChipSize)};
		switch (map._id)
		{
		case Item_MedicalKit:
			EntityManager::GetInstance().AddEntity(std::make_shared<MedicalKit>(map));
			break;
		case Item_Sugar:
			EntityManager::GetInstance().AddEntity(std::make_shared<Sugar>(map));
			break;
		default:
			break;
		}
		it++;
	}

	return true;
}
