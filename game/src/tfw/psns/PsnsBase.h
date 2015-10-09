/***********************************************************************//**
 *	PsnsBase.h
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2012/08/14.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_PSNS_PSNS_BASE_H_
#define _TFW_PSNS_PSNS_BASE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class VcString;

/*---------------------------------------------------------------------*//**
 *	プラットフォーム ソーシャル ネットワーク システム基底クラス
 *	
**//*---------------------------------------------------------------------*/
class PsnsBase
{
	//======================================================================
	// 定数
public:
	enum ScreenKind
	{
		SK_SCORE,
		SK_TROPHY,
		NUM_SK
	};

	//======================================================================
	// クラス
public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual bool getCurrentPlayerId(VcString* idstr) const = 0;
	virtual bool getCurrentPlayerNickname(VcString* name) const { return getCurrentPlayerId(name); };

	virtual bool updateScore(u32 score) = 0;
	virtual bool updateTrophy(s32 itp, u8 rate) = 0;
	virtual bool resetTrophies() = 0;

	virtual bool isValidUserAccount() = 0;
	virtual bool showPsnsScreen(ScreenKind sk) = 0;

#if defined(_DEBUG)
	virtual void debug_changeValidationUserAccountTest(bool validUser) {}
#endif

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
//	virtual ~PsnsBase();

	//======================================================================
	// 変数
private:
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_PSNS_PSNS_BASE_H_