/***********************************************************************//**
 *	SpiritManager.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/12/13.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_SPIRIT_SPIRIT_MANAGER_H_
#define _SRCR_SPIRIT_SPIRIT_MANAGER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../tfw/framemod/FrameExecDraw.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Array;

TFW_END_NS

SRCR_BEGIN_NS

class Game;
class Spirit;

/*---------------------------------------------------------------------*//**
 *	動作ロジック マネージャ クラス
 *
**//*---------------------------------------------------------------------*/
class SpiritManager : public FrameExecDraw
{
	//======================================================================
	// 定数
public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// スピリット数を得る
	s32 getSpiritNum() const;
	// スピリットを得る
	Spirit* getSpirit(u16 spiritid) const;
	// スピリットをインデックスから得る
	Spirit* getSpiritFromIndex(s32 index) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	u16 addSpirit(Spirit* spiritEntr);
	void removeSpirit(const Spirit* spirit);
	void removeAllSpirit();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
	SpiritManager();
	~SpiritManager();
	bool create();
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	void render(const RenderCtx* rc);			// 描画
	void notifyTerminateGame();

	//======================================================================
	// メンバ変数

private:
	Array<Spirit*>* _arrSpirit;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SPIRIT_SPIRIT_MANAGER_H_
