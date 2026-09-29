#include "PlayerFactory.h"
#include "Player/Player.h"
#include "Core/EntityManager.h"

PlayerFactory::PlayerFactory()
{

}
bool PlayerFactory::Load(const LevelData& data)
{
	/*
	std::ifstream ifs;
	ifs.open(fileName, std::ios::binary);
	if (!ifs.is_open())
	{
		MSG(L"外部ファイルが読み込めません。:PlayerFactory");
		return false;
	}

	std::string line;

	int xCount = 0;
	int yCount = 0;
	int num = 0;
	
	while (ifs.good()) {
		// csvの分解処理（split）
		std::stringstream ss(line);
		std::string buff;

		while (std::getline(ss, buff, ','))
		{
			int mapId = std::atoi(buff.c_str());
			mapInfo t_Info(mapId, xCount, yCount);
			_mapList.push_back(t_Info);
			switch (num)//ステータスを入力
			{
			case 0:moveSpeed = atof(inputc); break;
			case 1:atacck = atof(inputc); break;
			case 2:diffence = atof(inputc); break;
			default:
				break;

			}
		}


		yCount++;

	}
	xCount = 0;
	*/
	return true;
}

bool PlayerFactory::Create()
{
	//IdInfo map = { 0 ,"Player",XMFLOAT2(2.0f * Define::ChipSize,10.0f * Define::ChipSize)};
	IdInfo map = { 0 ,"Player",XMFLOAT2(0,0) };
	EntityManager::GetInstance().AddEntity(std::make_shared<Player>(map));
	return true;
}


