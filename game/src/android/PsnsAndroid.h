/***********************************************************************//**
 *	PsnsAndroid.h
 *	Enlight Android Application
 *	
 *	Created by Ryoutarou Kishi on 2012/12/10.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ANDROID_PSNS_ANDROID_H_
#define _ANDROID_PSNS_ANDROID_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../tfw/psns/PsnsBase.h"
#include "CompilerSwitch.h"

////////////////////////////////////////////////////////////////////////////
// クラス

namespace app
{

/*---------------------------------------------------------------------*//**
 *	Windows プラットフォーム ソーシャル ネットワーク システム
 *	
**//*---------------------------------------------------------------------*/
class PsnsAndroid : public tfw::PsnsBase
{
	//======================================================================
	// クラス
public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// プロパティ
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
	PsnsAndroid();

	//======================================================================
	// メンバ変数
private:
#if defined(_DEBUG)
	bool _validUserTest;
#endif
};

}	// namespace app

////////////////////////////////////////////////////////////////////////////

#endif	// _ANDROID_PSNS_ANDROID_H_
