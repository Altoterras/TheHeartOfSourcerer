/***********************************************************************//**
 *	TitleScene.h
 *	Enlight AtAr Application
 *
 *	Created by Ryoutarou Kishi on 2013/06/07.
 *	Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ATAR_SCENE_TITLE_SCENE_H_
#define _ATAR_SCENE_TITLE_SCENE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../tfw/framemod/FrameScene.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Rect;
typedef Rect<f32> RectF;
typedef Rect<s32> RectS32;
class Panel;
class Texture;

TFW_END_NS

ATAR_BEGIN_NS

class TitleSceneInner;

/*---------------------------------------------------------------------*//**
 *	タイトル画面
 *
**//*---------------------------------------------------------------------*/
class TitleScene : public FrameScene
{
	//======================================================================
	// 定数
private:
	// 処理フェイズ
	enum Phase
	{
		PHASE_NULL,
		PHASE_STABLE,
		PHASE_START_DOWNLOAD_XML,
		PHASE_WAIT_DOWNLOAD_XML,
		PHASE_WAIT_RETRY,
		PHASE_NEXT,
	};
	
	// UI 識別タグ
	enum UiTag
	{
		UITAG_NULL,
		UITAG_START,
		UITAG_HELP,
		UITAG_GETMARKER,
		UITAG_GOSUPPORT,
	};
	
	//======================================================================
	// クラス
private:

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
	TitleScene();
	~TitleScene();
	bool create(Texture* texGuiRef);
	void destroy();
	void beginScene(void* param);
	void endScene(void* param);
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	void render(const RenderCtx* rc);			// 3D 描画
	void draw(const RenderCtx* rc);				// 2D 描画
	
public:
	void onClickAlertButton(int index);
	void onClickButton(int uitag);

private:
	Panel* createButton(Texture* texGuiRef, const RectS32* rectPanel, const RectF* uvPanelNormal, const RectF* uvPanelHover);

	//======================================================================
	// 変数
private:
	TitleSceneInner* _inner;	// 内包クラス
	Phase _phase;				// 処理フェイズ
	Texture* _texTitle;			// タイトルテクスチャ
	f32 _fcntFade;				// フェード演出用フレームカウンタ
};

ATAR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ATAR_SCENE_TITLE_SCENE_H_
