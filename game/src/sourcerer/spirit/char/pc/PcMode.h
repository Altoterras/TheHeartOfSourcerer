/***********************************************************************//**
 *	PcMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/01/08.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_SPIRIT_CHAR_PC_PC_MODE_H_
#define _SRCR_SPIRIT_CHAR_PC_PC_MODE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../SpiritMode.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class Confront;
class PcSpirit;
class PcRole;

/*---------------------------------------------------------------------*//**
 *	プレイヤー キャララクター ロール管理
 *
**//*---------------------------------------------------------------------*/
class PcMode : public SpiritMode
{
	//======================================================================
	// 定数
public:
	enum Action
	{
		ACT_NULL,
		ACT_WALKING,
		ACT_EVENT,
		ACT_ATTACK,
		ACT_CHANTING,
		ACT_USE_ITEM,
		ACT_PICKUP_ITEM,
		ACT_GUARD_CHARGE,
		ACT_ATTRACT,
		ACT_CURE,
		ACT_MEDITATION,
		ACT_ENTER_CONFRONT,
		ACT_LEAVE_CONFRONT,
		NUM_ACT
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// ロールを得る
	inline const PcRole* getPcRole() const { return _role; }
	inline PcRole* pcRole() { return _role; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	// 直接モード変更
	void toWalking(bool isAfterConfront);
	void toConfront(Confront* confrontRef);
	void toEvent();
	void toNull();

	// アクションによるモード変更
	virtual bool action() = 0;
	void resetAction();
	bool isActing();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	PcMode();
	virtual bool create(PcSpirit* sprtRef, PcRole* roleEntr);
	virtual void destroy();

protected:
	virtual bool notifyBegin(void* objParamRef) { return true; }
	virtual void notifyEnd(void* objParamRef) {}
	virtual void notifyChildModeBegin(short modeid, void* objParamRef) = 0;
	virtual void notifyChildModeEnd(short modeid, void* objParamRef);

protected:
	virtual bool judgeConfrontAutoBehavior() = 0;

	//======================================================================
	// 変数
protected:
	PcRole*	_role;
	PcSpirit* _sprtRef;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SPIRIT_CHAR_PC_PC_MODE_H_
