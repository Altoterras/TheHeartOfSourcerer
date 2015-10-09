/***********************************************************************//**
 *	ModelViewer.h
 *	Enlight BasicModel Application
 *
 *	Created by Ryoutarou Kishi on 2012/11/13.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _BASICMODEL_BODY_MODEL_VIEWER_H_
#define _BASICMODEL_BODY_MODEL_VIEWER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../tfw/framemod/FrameExecDraw.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Camera;
class ShapeModel;

TFW_END_NS

BASICMODEL_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	モデル ビューワー
 *
**//*---------------------------------------------------------------------*/
class ModelViewer : public FrameExecDraw
{
	//======================================================================
	// 定数
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
	ModelViewer();
	~ModelViewer();
	bool create();
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	void render(const RenderCtx* rc);			// 3D 描画
	void draw(const RenderCtx* rc);				// 2D 描画

	//======================================================================
	// 変数
private:
	Camera* _camMvRef;
	ShapeModel** _mdlarrRef;		// モデル配列
	s32 _numMdl;					// モデル数
	u16 _animid;					// 現在のアニメーション ID
};

BASICMODEL_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _BASICMODEL_BODY_MODEL_VIEWER_H_
