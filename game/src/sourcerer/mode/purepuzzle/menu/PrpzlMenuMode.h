/***********************************************************************//**
 *	PrpzlMenuMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/02/23.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MODE_PUREPUZZLE_MENU_PRPZL_MENU_MODE_H_
#define _SRCR_MODE_PUREPUZZLE_MENU_PRPZL_MENU_MODE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../GameMode.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class PrpzlOperationData;

/*---------------------------------------------------------------------*//**
 *	パズル メニュー モード
 *
**//*---------------------------------------------------------------------*/
class PrpzlMenuMode : public GameModeBase
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	PrpzlMenuMode(PrpzlOperationData* opd);
	bool create();

protected:
	bool notifyBegin(void* objParamRef);
	void notifyEnd(void* objParamRef);

	//======================================================================
	// 変数
private:
	PrpzlOperationData* _opdatRef;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MODE_PUREPUZZLE_MENU_PRPZL_MENU_MODE_H_
