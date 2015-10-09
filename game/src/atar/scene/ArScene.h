/***********************************************************************//**
 *	ArScene.h
 *	Enlight AtAr Application
 *
 *	Created by Ryoutarou Kishi on 2013/06/05.
 *	Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ATAR_SCENE_AR_SCENE_H_
#define _ATAR_SCENE_AR_SCENE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../tfw/framemod/FrameScene.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Rect;
template<class TYPE> class Vector2;
template<class TYPE> class Vector3;
typedef Rect<f32> RectF;
typedef Rect<s32> RectS32;
typedef Vector2<f32> Vector2F;
typedef Vector3<f32> Vector3F;
class Camera;
class ShapeModel;
class Panel;
class Texture;

TFW_END_NS

ATAR_BEGIN_NS

class ArRender;
class ArSceneInner;
class ArScreen;
class ArSys;

/*---------------------------------------------------------------------*//**
 *	AR 画面
 *
**//*---------------------------------------------------------------------*/
class ArScene : public FrameScene
{
	//======================================================================
	// 定数
private:
	// 処理フェイズ
	enum Phase
	{
		PHASE_NULL,
		PHASE_AR,
		PHASE_MOVIE_DOWNLOADING,
		PHASE_MOVIE_ASK_RETRY,
		PHASE_MOVIE_START,
	};

	// ボタン種別定義
	enum ButtonKind
	{
		BTN_BACK,
		NUM_BTN
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
	ArScene();
	~ArScene();
	bool create(Texture* texGuiRef);
	void destroy();
	void beginScene(void* param);
	void endScene(void* param);
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	void renderBySg(const RenderCtx* rc);		// シーングラフによる 3D 描画
	void draw(const RenderCtx* rc);				// 2D 描画
	
public:
	void onClickAlertButton(int index);

private:
	bool createScene();
	void destroyScene();
	void appearLockScopeParticle();
	bool startMatchedMovieDownload();
	void prepareMatchedMoviePlay();
	Panel* createButton(Texture* texGuiRef, const RectS32* rectPanel, const RectF* uvPanelNormal, const RectF* uvPanelHover);

	//======================================================================
	// 変数
private:
	ArSceneInner* _inner;			// 内包クラス
	ArSys* _arsys;					// AR システム
	ArScreen* _screen;				// AR スクリーン
	ArRender* _rnode;				// 描画システム
	Panel* _pnlButtons[NUM_BTN];	// ボタンパネル
	bool _hasMotionDevice;			// モーションデバイスが有効である

	Phase _phase;					// フェイズ
	s32	_ecntPhase;					// フェイズ実行カウンタ
	f32 _fcntPerfFade;				// フェード演出用フレームカウンタ
	s32 _cntDestruction;			// スクリーン破壊カウンタ
	f32 _fcntPrefDestruction;		// スクリーン破壊演出用フレームカウンタ
	s32 _idxMatchedTrainResult;		// 最終的な認識画像

	Camera* _camMvRef;
	ShapeModel** _mdlarrRef;		// モデル配列
	s32 _numMdl;					// モデル数
	u16 _animid;					// 現在のアニメーション ID
};

ATAR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ATAR_SCENE_AR_SCENE_H_
