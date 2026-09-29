#include "StageUtility.h"

void Clamp(int* value, int min, int max)
{
	if (*value < min)
	{
		*value = min;
	}
	else if (*value > max)
	{
		*value = max;
	}
}

bool StageUtility::openStgFile(std::string stgFileName, int stageId, std::vector<int>& stgData, stgInfo& info)
{
	if (strcmp(stgFileName.c_str(), "NULL") == 0)return false;
	std::ifstream ifs;
	ifs.open(stgFileName);
	if (!ifs.is_open())
	{
		MSG(L"外部ファイルが読み込めません。:StageFactory");
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
			if (yCount == 0)xCount++;
			stgData.push_back(mapId);
		}
		yCount++;

	}
	info = { stageId,xCount,yCount - 1 };

	return true;
}

void StageUtility::SetEdgeHitInfo(hitInfo& hitInfo_edge, EdgeType edge_type)
{
	switch (edge_type)
	{
	case EdgeTypeLeft:
		hitInfo_edge.isHitLeft = true;
		break;
	case EdgeTypeRight:
		hitInfo_edge.isHitRight = true;
		break;
	case EdgeTypeTop:
		hitInfo_edge.isHitTop = true;
		break;
	case EdgeTypeBottom:
		hitInfo_edge.isHitBottom = true;
		break;
	}
}

bool StageUtility::collide(Stage& stg, Entity& entity, hitInfo& hitInfo, bool* isFloating, bool* isHitSlope, float width, float height)
{
	_stgInfo = stg.getStageInfo();

	XMFLOAT2 pos = entity.GetPosition();
	XMFLOAT2 prePos = entity.GetPrePosition();
	XMFLOAT2 vel = entity.GetVelocity();

	//落下中かそうでないかで斜面の判定方法が変わる
	if (*isHitSlope = AdjustToSlope(stg, entity, isFloating, width, height))//斜面に合わせる
	{
		XMFLOAT2 vel = entity.GetVelocity();
		vel.y = 0;
		entity.SetVelocity(vel);
	}

	int rectHitCountMid = 0;
	int isHitBottom = 0;

	int ty = floor((pos.y - 10) / Define::ChipSize);
	int tx = floor(pos.x - width / 2 / Define::ChipSize);//左
	if (checkHit(stg, tx, ty))rectHitCountMid++;

	tx = floor((pos.x + width / 2) / Define::ChipSize);//右
	if (checkHit(stg, tx, ty))rectHitCountMid++;

	ty = floor((pos.y + height / 2 + 10) / Define::ChipSize);//真下の点が当たっているか
	tx = floor((pos.x + 10) / Define::ChipSize);
	if (checkHit(stg, tx, ty))isHitBottom++;

	tx = floor((pos.x - 10) / Define::ChipSize);
	if (checkHit(stg, tx, ty))isHitBottom++;

	float contact_pos = 0.0f;

	_rect =
	{
		pos.y - vel.y - height / 2,
		pos.y - vel.y + height / 2,
		pos.x - vel.x - width / 2,
		pos.x - vel.x + width / 2,
	};

	//X軸の矩形の判定
	if (CollisionRectAndMapchipEdgeVersion(stg, _rect, Vec2(vel.x, 0.0f), edge_type, contact_pos))
	{
		int hitSlopeCount = 0;
		ty = floor((pos.y + height / 2) / Define::ChipSize);
		tx = floor((pos.x) / Define::ChipSize);
		if (checkHitSlope(stg, tx, ty))hitSlopeCount++;
		ty = floor((pos.y + height / 2 + Define::ChipSize / 2) / Define::ChipSize);
		if (checkHitSlope(stg, tx, ty))hitSlopeCount++;
		ty = floor((pos.y + height / 2 + Define::ChipSize) / Define::ChipSize);
		if (checkHitSlope(stg, tx, ty))hitSlopeCount++;

		if (hitSlopeCount > 0)//斜面に接触しているとき
		{
			if (rectHitCountMid > 0)
			{
				AdjustToEdge(entity, edge_type, contact_pos, isFloating, width, height);//地面に付いたら接地フラグがtrueになる
				SetEdgeHitInfo(hitInfo, edge_type);
			}
		}
		else//斜面に接触していないとき
		{
			if (*isFloating)//空中にいるなら
			{
				AdjustToEdge(entity, edge_type, contact_pos, isFloating, width, height);
				SetEdgeHitInfo(hitInfo, edge_type);
			}
			else
			{
				if (isHitBottom == 2)//両足でしっかり着地しているのなら
				{
					AdjustToEdge(entity, edge_type, contact_pos, isFloating, width, height);
					SetEdgeHitInfo(hitInfo, edge_type);
				}
			}
		}
	}

	//Y軸の矩形の判定
	if (CollisionRectAndMapchipEdgeVersion(stg, _rect, Vec2(0.0f, vel.y), edge_type, contact_pos))
	{
		if (!*isHitSlope)//斜面に接触していないとき
		{
			AdjustToEdge(entity, edge_type, contact_pos, isFloating, width, height);
			SetEdgeHitInfo(hitInfo, edge_type);
		}

	}

	return true;
}

void StageUtility::GetContactParameter(EdgeType rect_edge, int chip_id_x, int chip_id_y, EdgeType& contact_edge, float& contact_position)
{

	Vec2 chip_pos = Vec2((float)chip_id_x * Define::ChipSize, (float)chip_id_y * Define::ChipSize);

	switch (rect_edge)
	{
	case EdgeTypeLeft:
		contact_edge = EdgeType::EdgeTypeRight;
		contact_position = chip_pos.X + Define::ChipSize;
		break;
	case EdgeTypeRight:
		contact_edge = EdgeType::EdgeTypeLeft;
		contact_position = chip_pos.X;
		break;
	case EdgeTypeTop:
		contact_edge = EdgeType::EdgeTypeBottom;
		contact_position = chip_pos.Y + Define::ChipSize;
		break;
	case EdgeTypeBottom:
		contact_edge = EdgeType::EdgeTypeTop;
		contact_position = chip_pos.Y;
		break;
	}
}

bool StageUtility::CollisionRectAndMapchipEdgeVersion(Stage& stg, RectF src_rect, Vec2 vector, EdgeType& edge_type, float& contact_edge_position)
{

	int ChipSize = Define::ChipSize;
	RectF rect = src_rect;

	// 矩形にベクトルを加算する
	rect.Bottom += vector.Y;
	rect.Top += vector.Y;
	rect.Right += vector.X;
	rect.Left += vector.X;

	// サイズ調整
	rect.Right -= 1.0f;
	rect.Bottom -= 1.0f;

	// 矩形のX軸範囲
	int width_range_ids[]
	{
		(int)(rect.Left / ChipSize),
		(int)(rect.Right / ChipSize)
	};

	// 矩形のY軸範囲
	int height_range_ids[]
	{
		(int)(rect.Top / ChipSize),
		(int)(rect.Bottom / ChipSize)
	};

	// 範囲最大数
	const int max_range_ids[]
	{
		_stgInfo.xSize - 1,
		_stgInfo.ySize - 1,
	};

	for (int i = 0; i < 2; i++)
	{
		Clamp(&height_range_ids[i], 0, max_range_ids[0]);
		Clamp(&width_range_ids[i], 0, max_range_ids[1]);
	}

	const int start = 0;
	const int end = 1;

	// 各辺の検索範囲
	Vec2I edge_list[(int)EdgeType::EdgeTypeMax][2]
	{
		// 左辺
		{
			Vec2I(width_range_ids[start], height_range_ids[start]),
			Vec2I(width_range_ids[start], height_range_ids[end])
		},

		// 右辺
		{
			Vec2I(width_range_ids[end], height_range_ids[start]),
			Vec2I(width_range_ids[end], height_range_ids[end])
		},
		// 上辺
		{
			Vec2I(width_range_ids[start], height_range_ids[start]),
			Vec2I(width_range_ids[end], height_range_ids[start])
		},

		// 下辺
		{
			Vec2I(width_range_ids[start], height_range_ids[end]),
			Vec2I(width_range_ids[end], height_range_ids[end])
		},
	};

	// 判定で使用するベクトルを決める
	bool is_use_edge_list[(int)EdgeType::EdgeTypeMax]
	{
		vector.X < 0.0f ? true : false,
		vector.X > 0.0f ? true : false,
		vector.Y < 0.0f ? true : false,
		vector.Y > 0.0f ? true : false,
	};

	for (int i = 0; i < (int)EdgeType::EdgeTypeMax; i++)
	{
		// 使わない辺は判定しない
		if (is_use_edge_list[i] == false)
		{
			continue;
		}

		try
		{

			for (int y = edge_list[i][start].Y; y <= edge_list[i][end].Y; y++)
			{
				for (int x = edge_list[i][start].X; x <= edge_list[i][end].X; x++)
				{
					if (stg.mapHitData[y * _stgInfo.xSize + x] == 1)
					{
						// 接触した辺の座標を返す(座標はX or Yのどちらか)
						GetContactParameter((EdgeType)i, x, y, edge_type, contact_edge_position);

						// 当たり
						return true;
					}
				}
			}
		}
		catch (const std::exception&)
		{
			OutputDebugString(L"Out of Range!!\n");
		}
	}

	return false;
}

bool StageUtility::checkHit(Stage& stg, int tx, int ty)
{
	if (tx<0 || tx>_stgInfo.xSize || ty<0 || ty>_stgInfo.ySize - 1)return false;
	try
	{

		if (stg.mapHitData[ty * _stgInfo.xSize + tx] == 1)
			return true; else return false;
	}
	catch (const std::exception&)
	{
		OutputDebugString(L"Out of Range!!(isHit)\n");
		return false;
	}

};

bool StageUtility::checkHitSlope(Stage& stg, int tx, int ty)
{
	if (tx<0 || tx>_stgInfo.xSize || ty<0 || ty>_stgInfo.ySize - 1)return false;
	try
	{

		if (stg.mapHitData[ty * _stgInfo.xSize + tx] > 1)
			return true; else return false;
	}
	catch (const std::exception&)
	{
		OutputDebugString(L"Out of Range!!(isHitSlope)\n");
		return false;
	}
}

void StageUtility::AdjustToEdge(Entity& entity, EdgeType edge_type, float contact_pos, bool* isFloating, float width, float height)
{
	Vec2 offset = Vec2(width, height);

	XMFLOAT2 pos = entity.GetPosition();
	XMFLOAT2 vel = entity.GetVelocity();

	switch (edge_type)
	{
	case EdgeTypeLeft:
		pos.x = contact_pos - offset.X/2;
		vel.x = 0.0f;
		break;
	case EdgeTypeRight:
		pos.x = contact_pos + offset.X / 2;
		vel.x = 0.0f;
		break;
	case EdgeTypeTop:
		pos.y = contact_pos - offset.Y/2;
		vel.y = 0.0f;
		*isFloating = false;
		break;
	case EdgeTypeBottom:
		pos.y = contact_pos+offset.Y/2;
		vel.y = 0.0f;
		break;
	}

	entity.SetPosition(pos);
	entity.SetVelocity(vel);
}

int  StageUtility::AdjustToSlope(Stage& stg, Entity& entity, bool* isFloating, float width, float height)
{
	int ChipSize = Define::ChipSize;

	XMFLOAT2 pos = entity.GetPosition();
	XMFLOAT2 prePos = entity.GetPrePosition();
	XMFLOAT2 vel = entity.GetVelocity();

	int rx = (int)(pos.x) % (int)ChipSize;
	int tx = floor(pos.x  / ChipSize);
	int ty = floor((pos.y + height/2 - 1.1) / ChipSize);//計算誤差を吸収

	if (tx<0 || tx>_stgInfo.xSize || ty<0 || ty>_stgInfo.ySize - 1)return 0;

	bool isFloat = *isFloating;
	if (_stgInfo.ySize > ty + 1)
	{
		if (stg.mapHitData[(ty + 1) * _stgInfo.xSize + tx] == 0)isFloat = true;
	}
	*isFloating = isFloat;

	int hitChipData = stg.mapHitData[ty * _stgInfo.xSize + tx];

	float Y = 0;

	for (int i = 0; i < 3; i++)//かなり強引な実装なので様子をみて、やり直してもいい
	{
		if (i == 1)
		{
			ty = floor((pos.y + height/2 - ChipSize / 2) / ChipSize);//坂道補助
			if (tx<0 || tx>_stgInfo.xSize || ty<0 || ty>_stgInfo.ySize - 1)return 0;
			hitChipData = stg.mapHitData[ty * _stgInfo.xSize + tx];
		}
		if (i == 2)
		{
			ty = floor((pos.y + height/2 + ChipSize / 2) / ChipSize);//坂道補助
			if (tx<0 || tx>_stgInfo.xSize || ty<0 || ty>_stgInfo.ySize - 1)return 0;
			hitChipData = stg.mapHitData[ty * _stgInfo.xSize + tx];
		}

		switch (hitChipData)//地面
		{
		case SlopeUp_Ground://地面右上がり45度

			if (isFloat)//浮いている場合は正確な計算で衝突判定をする
			{
				{
					float Y = 0;
					Y = -(pos.x + width / 2) + tx * ChipSize + (ty + 1) * ChipSize;
					if (Y < pos.y + height/2)
					{
						pos.y = (ty + 1) * ChipSize - height/2 - rx;
						entity.SetPosition(pos);
						*isFloating = false;
						return true;
					}
					else return false;
				}
			}
			else//そうでない場合はマップチップで衝突判定をする（下り坂で浮かずにスナップさせるための措置）
			{
				float y = (ty + 1) * ChipSize - height/2 - rx;
				pos.y = (4 * prePos.y + 6 * y) / 10;
				pos.x = (4 * prePos.x + 6 * (pos.x)) / 10;
				entity.SetPosition(pos);
				*isFloating = false;
				return true;
			}
		case SlopeDown_Ground://地面右下がり45度

			if (isFloat)
			{
				{
					float Y = 0;
					Y = (pos.x + width / 2) - (tx * ChipSize) + (ty * ChipSize);
					if (Y < pos.y + height/2)
					{
						pos.y = (ty + 1) * ChipSize - height/2 - (ChipSize - 1 - rx);
						entity.SetPosition(pos);
						*isFloating = false;
						return true;
					}
					else return false;
				}
			}
			else
			{
				float y = (ty + 1) * ChipSize - height/2 - (ChipSize - 1 - rx);
				pos.y = (4 * prePos.y + 6 * y) / 10;
				pos.x = (4 * prePos.x + 6 * (pos.x)) / 10;
				entity.SetPosition(pos);
				*isFloating = false;
				return true;
			}
		case SlopeUp_GroundHalf_01://地面右上がり22.5度下半分

			if (isFloat)
			{
				{
					float Y = 0;
					Y = -(pos.x + width / 2) / 2 + (tx * ChipSize) / 2 + (ty + 1) * ChipSize;
					if (Y < pos.y + height/2)
					{
						pos.y = (ty + 1) * ChipSize - height/2 - (float)rx / 2;
						entity.SetPosition(pos);
						*isFloating = false;
						return true;
					}
					else return false;
				}
			}
			else
			{
				float y = (ty + 1) * ChipSize - height/2 - (float)rx / 2;
				pos.y = (2 * prePos.y + 8 * y) / 10;
				pos.x = (2 * prePos.x + 8 * (pos.x)) / 10;
				entity.SetPosition(pos);
				*isFloating = false;
				return true;
			}
		case SlopeUp_GroundHalf_02://地面右上がり22.5度上半分

			if (isFloat)
			{
				{
					float Y = 0;
					Y = -(pos.x + width / 2) / 2 + ((tx + 1) * ChipSize) / 2 + ty * ChipSize;
					if (Y < pos.y + height/2)
					{
						pos.y = (ty + 1) * ChipSize - height/2 - (float)rx / 2 - ChipSize / 2;
						entity.SetPosition(pos);
						*isFloating = false;
						return true;
					}
					else return false;
				}
			}
			else
			{
				float y = (ty + 1) * ChipSize - height/2 - (float)rx / 2 - ChipSize / 2;
				pos.y = (2 * prePos.y + 8 * y) / 10;
				pos.x = (2 * prePos.x + 8 * (pos.x)) / 10;
				entity.SetPosition(pos);
				*isFloating = false;
				return true;
			}
		case SlopeDown_GroundHalf_01://地面右下がり22.5度上半分

			if (isFloat)
			{
				{
					float Y = 0;
					Y = (pos.x + width / 2) / 2 - (tx * ChipSize) / 2 + (ty * ChipSize);
					if (Y < pos.y + height/2)
					{
						pos.y = (ty + 1) * ChipSize - height/2 - (ChipSize - 1 - (float)rx) / 2 - ChipSize / 2;
						entity.SetPosition(pos);
						*isFloating = false;
						return true;
					}
					else return false;
				}
			}
			else
			{
				float y = (ty + 1) * ChipSize - height/2 - (ChipSize - 1 - (float)rx) / 2 - ChipSize / 2;
				pos.y = (2 * prePos.y + 8 * y) / 10;
				pos.x = (2 * prePos.x + 8 * (pos.x)) / 10;
				entity.SetPosition(pos);
				*isFloating = false;
				return true;
			}
		case SlopeDown_GroundHalf_02://地面右下がり22.5度下半分

			if (isFloat)
			{
				{
					float Y = 0;
					Y = (pos.x + width / 2) / 2 - ((tx + 1) * ChipSize) / 2 + (ty + 1) * ChipSize;
					if (Y < pos.y + height/2)
					{
						pos.y = (ty + 1) * ChipSize - height/2 - (ChipSize - 1 - (float)rx) / 2;
						entity.SetPosition(pos);
						*isFloating = false;
						return true;
					}
					else return false;
				}
			}
			else
			{
				float y = (ty + 1) * ChipSize - height/2 - (ChipSize - 1 - (float)rx) / 2;
				pos.y = (2 * prePos.y + 8 * y) / 10;
				pos.x = (2 * prePos.x + 8 * (pos.x)) / 10;
				entity.SetPosition(pos);
				*isFloating = false;
				return true;
			}
		default:
			break;

		}
	}

	ty = floor(pos.y-width/2 - 1.1) / ChipSize;

	if (tx<0 || tx>_stgInfo.xSize || ty<0 || ty>_stgInfo.ySize - 1)return 0;
	int hitChipData2 = stg.mapHitData[ty * _stgInfo.xSize + tx];//天井

	switch (hitChipData2)
	{
	case SlopeUp_Ceiling://天井右上がり45度

		Y = 0;
		Y = -(pos.x + width / 2) + tx * ChipSize + (ty + 1) * ChipSize;
		if (Y > pos.y+height/2)
		{
			pos.y = ty * ChipSize + height/2 - rx + 3;
			entity.SetPosition(pos);
			*isFloating = true;
			return true;
		}
		else return false;

	case SlopeDown_Ceiling://天井右下がり45度

		Y = 0;
		Y = (pos.x + width / 2) - (tx * ChipSize) + (ty * ChipSize);
		if (Y > pos.y + height / 2)
		{
			pos.y = ty * ChipSize + height/2 - (ChipSize - 1 - rx) + 4;
			entity.SetPosition(pos);
			*isFloating = true;
			return true;
		}
		else return false;
	case SlopeUp_CeilingHalf_01://天井右上がり22.5度下半分

		Y = 0;
		Y = -(pos.x + width / 2) / 2 + (tx * ChipSize) / 2 + (ty + 1) * ChipSize;
		if (Y > pos.y)
		{
			pos.y = (ty + 1) * ChipSize - (float)rx / 2 + 4;
			entity.SetPosition(pos);
			return true;
		}
		else return false;


	case SlopeUp_CeilingHalf_02://天井右上がり22.5度上半分


		Y = 0;
		Y = -(pos.x + width / 2) / 2 + ((tx + 1) * ChipSize) / 2 + ty * ChipSize;
		if (Y > pos.y)
		{
			pos.y = (ty + 1) * ChipSize - (float)rx / 2 - ChipSize / 2 + 4;
			entity.SetPosition(pos);
			return true;
		}
		else return false;

	case SlopeDown_CeilingHalf_01://天井右下がり22.5度上半分


		Y = 0;
		Y = (pos.x + width / 2) / 2 - (tx * ChipSize) / 2 + (ty * ChipSize);
		if (Y > pos.y)
		{
			pos.y = (ty + 1) * ChipSize - (ChipSize - 1 - (float)rx) / 2 - ChipSize / 2 + 4;
			entity.SetPosition(pos);
			return true;
		}
		else return false;

	case SlopeDown_CeilingHalf_02://天井右下がり22.5度下半分

		Y = 0;
		Y = (pos.x + width / 2) / 2 - ((tx + 1) * ChipSize) / 2 + (ty + 1) * ChipSize;
		if (Y > pos.y)
		{
			pos.y = (ty + 1) * ChipSize - (ChipSize - 1 - (float)rx) / 2 + 4;
			entity.SetPosition(pos);
			return true;
		}
		else return false;

	default:
		return false;
	}
	return false;

}
