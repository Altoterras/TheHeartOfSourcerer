/***********************************************************************//**
 *	IngameMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/07/15.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MODE_FIELD_FIELD_MODE_H_
#define _SRCR_MODE_FIELD_FIELD_MODE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../GameMode.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class WalkingCam;

/*---------------------------------------------------------------------*//**
 *	インゲーム モード
 *
**//*---------------------------------------------------------------------*/
class IngameMode : public GameModeBase
{
	//======================================================================
	// 定数

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	inline WalkingCam*	getWalkingCam()	{	return _wlkcam;	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	IngameMode();
	~IngameMode();
	bool create();
	void destroy();

protected:
	bool notifyBegin(void* objParamRef);
	void notifyEnd(void* objParamRef);
	short getDefaultChildMode();

	//======================================================================
	// メンバ変数

private:
	WalkingCam*	_wlkcam;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MODE_FIELD_FIELD_MODE_H_
