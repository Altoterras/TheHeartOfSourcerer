/***********************************************************************//**
 *	MovieScene.h
 *	Enlight AtAr Application
 *
 *	Created by Ryoutarou Kishi on 2013/06/17.
 *	Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ATAR_SCENE_MOVIE_SCENE_H_
#define _ATAR_SCENE_MOVIE_SCENE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../tfw/framemod/FrameScene.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Rect;
typedef Rect<f32> RectF;

TFW_END_NS

ATAR_BEGIN_NS

class MovieSceneInner;

/*---------------------------------------------------------------------*//**
 *	ムービー画面
 *
**//*---------------------------------------------------------------------*/
class MovieScene : public FrameScene
{
	//======================================================================
	// 定数
private:
	// 処理フェイズ
	enum Phase
	{
		PHASE_NULL,
		PHASE_WAIT_MOVIEEND,
		PHASE_WAIT_ANSWER,
	};
	
	// UI 識別タグ
	enum UiTag
	{
		UITAG_NULL,
		UITAG_MORE_VIEW,
		UITAG_BACK,
	};
	
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// プロパティ
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	MovieScene();
	~MovieScene();
	bool create();
	void destroy();
	void beginScene(void* param);
	void endScene(void* param);
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	void render(const RenderCtx* rc);			// 3D 描画
	void draw(const RenderCtx* rc);				// 2D 描画
	
public:
	void onClickButton(int tag);
	
private:
	void destroyScene();
	RectF* makeButtonRect(RectF* rect, int tag);

	//======================================================================
	// 変数
private:
	MovieSceneInner* _inner;
	Phase _phase;
	s32 _idxMatchedMaterial;
};

ATAR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ATAR_SCENE_MOVIE_SCENE_H_
