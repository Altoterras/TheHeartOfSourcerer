/***********************************************************************//**
 *	ModelViewMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/06/23.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_DEBUG_MODE_MODELVIEW_MODEL_VIEW_MODE_H_
#define _SRCR_DEBUG_MODE_MODELVIEW_MODEL_VIEW_MODE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../mode/GameMode.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Camera;
class ShapeModel;

TFW_END_NS

SRCR_BEGIN_NS

#if defined(_DEBUG)
class CharStat;
#endif
/*---------------------------------------------------------------------*//**
 *	モデルビュー モード
 *
**//*---------------------------------------------------------------------*/
class ModelViewMode : public GameModeBase
{
	//======================================================================
	// 定数
public:
	// 制御モード
	enum ActMode
	{
		AMODE_MODEL,	// モデル表示
		AMODE_RTMV,		// リアルタイムムービー
		AMODE_PTCL,		// パーティクル
		AMODE_FONT,		// フォント
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual void callCustomEvent(void* objRes, const VcString* strParam, const void* objParam);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ModelViewMode();
	~ModelViewMode();
	void exec(ExecRes* res, const ExecCtx* ec);
	void draw(const RenderCtx* rc);

protected:
	bool notifyBegin(void* objParamRef);
	void notifyEnd(void* objParamRef);

private:
	bool beginActMode(ActMode amode, bool isFirst);
	void endActMode();
	void appearCurParticle();
	void appearNextParticle();
	void appearPrevParticle();
	void appearNextFont();

	//======================================================================
	// 変数
private:
	ShapeModel**	_mdlarrRef;		// モデル配列
	s32				_numMdl;		// モデル数
	u16				_animid;		// 現在のアニメーション ID
	u16				_ptclid;		// パーティクル ID
	ActMode			_amode;			// 制御モード
	s32				_dispCnt;		// 表示カウンタ
	s32				_param;			// 汎用パラメータ
	Camera*			_camMvRef;		// モデルビューワー用カメラ
	Camera*			_camRmRef;		// リアルタイムムービー用カメラ
#if defined(_DEBUG)
	CharStat*		_cstatDbg;		// デバッグ用キャラクターステータス
#endif
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_DEBUG_MODE_MODELVIEW_MODEL_VIEW_MODE_H_
