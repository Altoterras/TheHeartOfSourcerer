/***********************************************************************//**
 *	WalkingMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/12.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MODE_INGAME_WALKING_WALKING_MODE_H_
#define _SRCR_MODE_INGAME_WALKING_WALKING_MODE_H_

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
class WalkingMode : public GameModeBase
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	WalkingMode();
	bool create();
	void exec(ExecRes* res, const ExecCtx* ec);
	void render(const RenderCtx* rc);
	void notifyChangedMap(bool isDeserializeCase);

protected:
	bool notifyBegin(void* objParamRef);
	void notifyEnd(void* objParamRef);
	short getDefaultChildMode();

private:
	void setSafeLeader();

	//======================================================================
	// 変数
private:
	f32 _frameSafeSave;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MODE_INGAME_WALKING_WALKING_MODE_H_
