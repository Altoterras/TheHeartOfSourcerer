/***********************************************************************//**
 *	TemplateMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/12.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MODE_INGAME_WALKING_TEMPLATE_MODE_H_
#define _SRCR_MODE_INGAME_WALKING_TEMPLATE_MODE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../GameMode.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	インゲーム モード
 *
**//*---------------------------------------------------------------------*/
class TemplateMode : public GameModeBase
{
	//======================================================================
	// 定数

	//======================================================================
	// メソッド
public:
	TemplateMode();
	bool create();
	void exec(ExecRes* res, const ExecCtx* ec);
	void render(const RenderCtx* rc);

protected:
	bool notifyBegin(void* objParamRef);
	void notifyEnd(void* objParamRef);
	short getDefaultChildMode();

	//======================================================================
	// メンバ変数
private:
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MODE_INGAME_WALKING_TEMPLATE_MODE_H_
