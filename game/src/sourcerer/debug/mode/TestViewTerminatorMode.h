/***********************************************************************//**
 *	TestViewTerminatorMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/11/26.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_DEBUG_MODE_TEST_VIEW_TERMINATOR_MODE_H_
#define _SRCR_DEBUG_MODE_TEST_VIEW_TERMINATOR_MODE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../mode/GameMode.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	テスト ビュー終端モード
 *
**//*---------------------------------------------------------------------*/
class TestViewTerminatorMode : public GameModeBase
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
protected:
	bool notifyBegin(void* objParamRef);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_DEBUG_MODE_TEST_VIEW_TERMINATOR_MODE_H_
