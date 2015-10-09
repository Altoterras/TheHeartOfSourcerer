/***********************************************************************//**
 *	PsnsWin.h
 *	Enlight Windows Application
 *	
 *	Created by Ryoutarou Kishi on 2012/04/14.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _WINDOWS_PSNS_WIN_H_
#define _WINDOWS_PSNS_WIN_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base
#include "../tfw/psns/PsnsBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

namespace app
{

/*---------------------------------------------------------------------*//**
 *	Windows プラットフォーム ソーシャル ネットワーク システム
 *	
**//*---------------------------------------------------------------------*/
class PsnsWin : public tfw::PsnsBase
{
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
	bool getCurrentPlayerId(tfw::VcString* idstr) const;
	bool updateScore(u32 score);
	bool updateTrophy(s32 itp, u8 rate);
	bool resetTrophies();
	bool isValidUserAccount();
	bool showPsnsScreen(ScreenKind sk);

#if defined(_DEBUG)
	void debug_changeValidationUserAccountTest(bool validUser);
#endif

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	PsnsWin();

	//======================================================================
	// メンバ変数
private:
#if defined(_DEBUG)
	bool _validUserTest;
#endif
};

}	// namespace app

////////////////////////////////////////////////////////////////////////////

#endif	// _WINDOWS_PSNS_WIN_H_