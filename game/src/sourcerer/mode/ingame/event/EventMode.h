/***********************************************************************//**
 *	EventMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/02/10.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MODE_INGAME_EVENT_EVENT_MODE_H_
#define _SRCR_MODE_INGAME_EVENT_EVENT_MODE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../GameMode.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	インゲーム メニュー モード
 *
**//*---------------------------------------------------------------------*/
class EventMode : public GameModeBase
{
	//======================================================================
	// 定数

	//======================================================================
	// メソッド
public:
	EventMode();
	~EventMode();
	bool create();
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);
	void draw(const RenderCtx* rc);

protected:
	bool notifyBegin(void* objParamRef);
	void notifyEnd(void* objParamRef);

	//======================================================================
	// メンバ変数
private:
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MODE_INGAME_EVENT_EVENT_MODE_H_
