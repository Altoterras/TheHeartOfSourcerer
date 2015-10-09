/***********************************************************************//**
 *	TitleMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/10.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MODE_TITLE_TITLE_MODE_H_
#define _SRCR_MODE_TITLE_TITLE_MODE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../GameMode.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	タイトル モード
 *
**//*---------------------------------------------------------------------*/
class TitleMode : public GameModeBase
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	TitleMode();
	bool create();

protected:
	bool notifyBegin(void* objParamRef);
	void notifyEnd(void* objParamRef);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MODE_TITLE_TITLE_MODE_H_
