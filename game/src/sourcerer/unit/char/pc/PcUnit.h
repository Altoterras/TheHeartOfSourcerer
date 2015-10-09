/***********************************************************************//**
 *	PcUnit.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/06/16.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_UNIT_CHAR_PC_PC_UNIT_H_
#define _SRCR_UNIT_CHAR_PC_PC_UNIT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../CharUnit.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;

TFW_END_NS

SRCR_BEGIN_NS

class Item;
class ItemEquip;
class Party;

/*---------------------------------------------------------------------*//**
 *	NPC キャラユニット
 *
**//*---------------------------------------------------------------------*/
class PcUnit : public CharUnit
{
	//======================================================================
	// 定数
public:
	// PC 動作フラグ
	const static u16 PCAF_LEADER	= 0x0001;	// リーダーである

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// リーダーかどうかを得る
	inline bool isLeader() const { return TFW_IS_FLAG(_pcaflags, PCAF_LEADER); }
	// アイテム装備機能を得る（無い場合は NULL が返る）
	virtual inline const ItemEquip* getItemEquip() const { return 0L; }
	virtual inline ItemEquip* itemEquip() { return 0L; }
	// パーティへの参照を得る
	inline Party* getPartyRef() const { return _partyRef; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual void setLeader(bool isLeader);
	virtual bool isValidLeader();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	PcUnit(UnitType unittype);
	virtual ~PcUnit();
	virtual bool create(Model* mdlRef, Spirit* sprtRef, Party* party, u16 chrdid, CharClass charcls);
	virtual void destroy();
	virtual void exec(ExecRes* res, const ExecCtx* ec);
	virtual void execWalking(const ExecCtx* ec, bool isForceAuto);
	virtual void notifyChangedMap();
	virtual void notifyChangedConfront(bool isConfront, Confront* confront, Unit* unitTrg, Unit* unitEntrant, bool isAddition);

	void setVelocityRange(f32 velRangeMin, f32 velRangeMax);
	virtual void calcTargetPos() = 0;

protected:
	virtual bool isOperatedForMoving();
	virtual bool moveMapInitPos();
	void calcTargetPosRelLeader(const Vector3F* posLeaderDiff);
	virtual void checkAfterMove(const Vector3F* pos, s32 idxPlaneCur, s32 idxPlanePrev);

private:
	void execWalking(const ExecCtx* ec) { execWalking(ec, false); }

	//======================================================================
	// 変数
protected:
	Party* _partyRef;
	u16 _pcaflags;			// PC 動作フラグ（※ PcSpirit 側の方がふさわしいかも？）
	f32 _velRangeMin;		// 最小速さ
	f32 _velRangeMinPw;		// 最小速度の二乗
	f32 _velRangeMax;		// 最大速さ
	f32 _velRangeMaxPw;		// 最大速度の二乗
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_UNIT_CHAR_PC_PC_UNIT_H_
