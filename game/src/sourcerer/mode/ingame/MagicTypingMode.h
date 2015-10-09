/***********************************************************************//**
 *	MagicTypingMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/12.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MODE_INGAME_WALKING_MAGIC_TYPING_MODE_H_
#define _SRCR_MODE_INGAME_WALKING_MAGIC_TYPING_MODE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../GameMode.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	ソウマ入力モード
 *
**//*---------------------------------------------------------------------*/
class MagicTypingMode : public GameModeBase
{
	//======================================================================
	// 定数
private:
	enum SmallMode
	{
		SMODE_TYPING,
		SMODE_SELLIST,
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	MagicTypingMode();
	bool create();
	void exec(ExecRes* res, const ExecCtx* ec);

protected:
	bool notifyBegin(void* objParamRef);
	void notifyEnd(void* objParamRef);

private:
	void changePrevMode();

	//======================================================================
	// 変数
private:
	bool			_isDirectListMode;
	u8				_srckind;
	SmallMode		_smode;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MODE_INGAME_WALKING_MAGIC_TYPING_MODE_H_
