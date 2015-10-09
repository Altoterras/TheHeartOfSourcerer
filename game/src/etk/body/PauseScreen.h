/***********************************************************************//**
 *  PauseScreen.h
 *  Envelopment Terras Kit
 *
 *  Created by Ryoutarou Kishi on 2012/08/30.
 *  Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ETK_BODY_PAUSE_SCREEN_H_
#define _ETK_BODY_PAUSE_SCREEN_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../tfw/framemod/FrameExecDraw.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Texture;

TFW_END_NS

ETK_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	ポーズ スクリーン
 *
**//*---------------------------------------------------------------------*/
class PauseScreen : public FrameExecDraw
{
	//======================================================================
	// 定数
protected:
	enum Step
	{
		STEP_NULL,
		STEP_FADEIN,
		STEP_STABLE,
		STEP_FADEOUT,
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// ポーズ中かどうか
	inline bool isPausing() const { return _step != STEP_NULL; }

protected:
	// ステップを得る
	inline s32 getStep() const { return _step; }
	// ステップ内フレームカウントを得る
	inline f32 getStepFrameCount() const { return _fcntStep; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void start();
	void setTexture(Texture* texDelg) { _tex = texDelg; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	PauseScreen();
	virtual ~PauseScreen();
	virtual bool create();
	virtual void destroy();
	virtual void exec(ExecRes* res, const ExecCtx* ec);
	virtual void draw(const RenderCtx* rc) {}

protected:
	virtual bool loadTexture() { return false; }
	virtual void deleteTexture();

	//======================================================================
	// 変数
protected:
	s32 _step;
	f32 _fcntStep;
	Texture* _tex;
};

ETK_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ETK_BODY_PAUSE_SCREEN_H_