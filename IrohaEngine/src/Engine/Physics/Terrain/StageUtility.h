#pragma once
#include "Core/Base.h"
#include "Core/DEFINE.h"
#include "Core/Entity.h"
#include "Stage.h"
#include "eStage.h"

class Entity;
class Stage;

//ステージユーティリティクラス：ステージの衝突判定処理などの汎用クラス。
class StageUtility
{
public:
	bool openStgFile(std::string stgFileName,int stageId, std::vector<int>& stgData, stgInfo& info);//ステージ情報をファイルから読み込む
	bool collide(Stage& stg, Entity& entity, hitInfo& hitInfo_edge, bool* isFloating, bool* isHitSlope, float width, float height);//衝突判定と押し戻し処理を行う
	bool checkHit(Stage& stg, int tx, int ty);                        //入力した格子座標が衝突判定か
	bool checkHitSlope(Stage& stg, int tx, int ty);                   //入力した格子座標が衝突判定か
protected:
private:

	void SetEdgeHitInfo(hitInfo& hitInfo_edge, EdgeType edge_type);//衝突した地形の辺を地形衝突情報として保存


	bool CollisionRectAndMapchipEdgeVersion(Stage& stg, RectF src_rect, Vec2 vector, EdgeType& edge_type, float& contact_edge_position); //矩形の衝突処理
	void GetContactParameter(EdgeType rect_edge, int chip_id_x, int chip_id_y, EdgeType& contact_edge, float& contact_position);//衝突した地形の辺を取得
	void AdjustToEdge(Entity& entity, EdgeType edge_type, float contact_pos, bool* isFloating,float width, float height);//矩形の押し戻し
	int AdjustToSlope(Stage& stg, Entity& entity, bool* isFloating,float width, float height);//坂道の衝突、押し戻し


	stgInfo _stgInfo; //ステージ情報
	EdgeType edge_type;//地形側の衝突した辺の種類
	RectF _rect;       //矩形

	int ChipSize = 0;//地形サイズ

};
