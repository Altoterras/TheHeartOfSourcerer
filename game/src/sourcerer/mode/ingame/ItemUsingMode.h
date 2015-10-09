/***********************************************************************//**
 *	ItemUsingMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/09/08.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MODE_INGAME_WALKING_ITEM_USING_MODE_H_
#define _SRCR_MODE_INGAME_WALKING_ITEM_USING_MODE_H_

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
class ItemUsingMode : public GameModeBase
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ItemUsingMode();
	bool create();
	void exec(ExecRes* res, const ExecCtx* ec);

protected:
	bool notifyBegin(void* objParamRef);
	void notifyEnd(void* objParamRef);

	//======================================================================
	// 変数
private:
	Unit* _unitRef;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MODE_INGAME_WALKING_ITEM_USING_MODE_H_
