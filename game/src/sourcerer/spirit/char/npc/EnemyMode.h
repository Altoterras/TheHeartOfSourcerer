/***********************************************************************//**
 *	EnemyMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/02/05.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_SPIRIT_CHAR_NPC_ENEMY_MODE_H_
#define _SRCR_SPIRIT_CHAR_NPC_ENEMY_MODE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../SpiritMode.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class Confront;
class EnemySpirit;
class EnemyRole;

/*---------------------------------------------------------------------*//**
 *	敵のロール管理
 *
**//*---------------------------------------------------------------------*/
class EnemyMode : public SpiritMode
{
	//======================================================================
	// 定数
public:
	enum Action
	{
		ACT_NULL,
		ACT_STANDBY,
		ACT_WALKING,
		ACT_ATTACK,
		ACT_DEFENSE,
		ACT_SKILL,
		ACT_MOVETOTRG,
		NUM_ACT
	};

	//======================================================================
	// クラス
public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// ロールを得る
	inline EnemyRole*		getRole()	{	return _role;	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void toWalking();
	void toConfront(Confront* confrontRef);
	void toStanby();

	bool action();
	bool isActing();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	explicit EnemyMode();
	bool create(EnemySpirit* sprtRef, EnemyRole* roleEntr);
	void destroy();

protected:
	bool notifyBegin(void* objParamRef);
	void notifyEnd(void* objParamRef);
	void notifyChildModeBegin(short modeid, void* objParamRef);
	void notifyChildModeEnd(short modeid, void* objParamRef);

	//======================================================================
	// メンバ変数

private:
	EnemyRole*		_role;
	EnemySpirit*	_sprtRef;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SPIRIT_CHAR_NPC_ENEMY_MODE_H_
