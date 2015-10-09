/***********************************************************************//**
 *	FriendPcUnit.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/20.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_UNIT_CHAR_PC_FRIEND_PC_UNIT_H_
#define _SRCR_UNIT_CHAR_PC_FRIEND_PC_UNIT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "PcUnit.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	味方キャラユニット（現在未使用）
 *
**//*---------------------------------------------------------------------*/
class FriendPcUnit : public PcUnit
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual void notifyChangedMap();
	virtual void notifyChangedConfront(bool isConfront, Confront* confront, Unit* unitTrg, Unit* unitEntrant, bool isAddition);
	void calcTargetPos(const Vector3F* posPc1Diff);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	FriendPcUnit(UnitType unittype);
	virtual ~FriendPcUnit();
	virtual bool create(Model* mdlRef, Spirit* sprtRef, Party* party, u16 chrdid, CharClass charcls);
	virtual void destroy();
	virtual void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御

protected:
	virtual bool moveMapInitPos();

	//======================================================================
	// 変数
protected:
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_UNIT_CHAR_PC_FRIEND_PC_UNIT_H_
