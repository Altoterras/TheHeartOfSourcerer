/***********************************************************************//**
 *	ConfrontMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/21.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MODE_INGAME_CONFRONT_CONFRONT_MODE_H_
#define _SRCR_MODE_INGAME_CONFRONT_CONFRONT_MODE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../GameMode.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class Confront;
class ConfrontCam;

/*---------------------------------------------------------------------*//**
 *	対決モード
 *
**//*---------------------------------------------------------------------*/
class ConfrontMode : public GameModeBase
{
	//======================================================================
	// 定数
private:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ConfrontMode();
	~ConfrontMode();
	bool create();
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);
	void render(const RenderCtx* rc);
	void notifyDischargeSouma(Souma* souma, u16 mcid);
	void notifyUnitInfluenceStat(Unit* unitRecv, const StatusChangeResult* scres, const TransStat* tstat);

protected:
	bool notifyBegin(void* objParamRef);
	void notifyEnd(void* objParamRef);
	short getDefaultChildMode();

	//======================================================================
	// メンバ変数
private:
	Confront*		_confrontRef;	// 対決モジュール
	ConfrontCam*	_camCf;			// 対決モード専用カメラ
	u16				_camidBk;		// 専用カメラ変更前のカメラ ID
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MODE_INGAME_CONFRONT_CONFRONT_MODE_H_
