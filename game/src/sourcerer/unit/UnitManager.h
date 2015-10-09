/***********************************************************************//**
 *	UnitManager.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/07/15.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_UNIT_UNIT_MANAGER_H_
#define _SRCR_UNIT_UNIT_MANAGER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../tfw/framemod/FrameExecDraw.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Array;
class Serializer;

TFW_END_NS

SRCR_BEGIN_NS

class CharUnit;
class Game;
class Unit;

/*---------------------------------------------------------------------*//**
 *	ユニット マネージャ クラス
 *
**//*---------------------------------------------------------------------*/
class UnitManager : public FrameExecDraw
{
	//======================================================================
	// 定数
public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// ユニット数を得る
	s32 getUnitNum() const;
	// ユニットを得る
	Unit* getUnitFromId(u16 unitid) const;
	// ユニットをインデックスから得る
	Unit* getUnitFromIndex(s32 index) const;
	// 自キャラユニットを得る
	Unit* getMyCharUnit() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool addUnit(Unit* unitEntr);
	bool addMyCharUnit(Unit* unitEntr);
	void removeUnit(Unit* unit);
	void removeAllUnit();

	Unit* findUnitFromCtrlEvid(u16 evidCtrl) const;
	CharUnit* findCharUnitFromCharId(u16 chrdid) const;
	CharUnit* findCharUnitFromCtrlEvidOrCharId(u16 evidCtrl, u16 chrdid) const;

	bool checkEnableEnemy() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	UnitManager();
	~UnitManager();
	bool create();
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	void render(const RenderCtx* rc);	// 描画
	void notifyChangeMapBefore();
	void notifyChangedMap();
	void notifyChangedEventMode(bool isEventMode);

	bool serialize(Serializer* ser) const;
	bool deserialize(Serializer* ser);

	//======================================================================
	// メンバ変数

private:
	Array<Unit*>* _arrUnit;
	int _idxMyChar;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_UNIT_UNIT_MANAGER_H_
