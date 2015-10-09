/***********************************************************************//**
 *	NpcCharUnit.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/20.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_UNIT_CHAR_NPC_NPC_CHAR_UNIT_H_
#define _SRCR_UNIT_CHAR_NPC_NPC_CHAR_UNIT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../CharUnit.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector2;
typedef Vector2<f32> Vector2F;
template<class TYPE> class Point;
typedef Point<f32> PointF32;

TFW_END_NS

SRCR_BEGIN_NS

class CharStat;
class PlacementObj;

/*---------------------------------------------------------------------*//**
 *	NPC キャラユニット
 *
**//*---------------------------------------------------------------------*/
class NpcCharUnit : public CharUnit
{
	//======================================================================
	// 定数
private:
	// 歩行状態
	enum WalkState
	{
		WSTAT_NULL,
		WSTAT_WALK,
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 配置オブジェクト情報を得る
	inline PlacementObj* getPlacementObj() const { return _pobjRef; }
	// 制御用イベント ID を得る
	virtual u16 getCtrlEvid() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void setPlacementObj(PlacementObj* pobj);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	NpcCharUnit(UnitType unittype);
	virtual ~NpcCharUnit();
	virtual bool create(Model* mdlRef, Spirit* sprtRef, u16 chrdid, u16 unitflag, CharClass charcls);
	virtual void execWalkingRandom(const ExecCtx* ec);

	//======================================================================
	// メンバ変数
protected:
	PlacementObj* _pobjRef;		// 配置オブジェクト情報

	f32 _fcntRandWalk;			// ランダム歩行フレームカウンタ
	WalkState _walkstat;		// 歩行状態
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_UNIT_CHAR_NPC_NPC_CHAR_UNIT_H_
