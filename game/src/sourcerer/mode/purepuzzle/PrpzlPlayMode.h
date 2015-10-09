/***********************************************************************//**
 *	PrpzlPlayMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/02/13.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MODE_PUREPUZZLE_PRPZL_PLAY_MODE_H_
#define _SRCR_MODE_PUREPUZZLE_PRPZL_PLAY_MODE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../GameMode.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Color;
typedef Color<u8> ColorU8;
class Camera;

TFW_END_NS

SRCR_BEGIN_NS

class PrpzlOperationData;
class PrpzlPerfBase;
class PurePuzzle;

/*---------------------------------------------------------------------*//**
 *	パズル ビュー モード
 *
**//*---------------------------------------------------------------------*/
class PrpzlPlayMode : public GameModeBase
{
	//======================================================================
	// 定数
public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	PrpzlPlayMode(PrpzlOperationData* opdat);
	~PrpzlPlayMode();
	void exec(ExecRes* res, const ExecCtx* ec);
	void render(const RenderCtx* rc);
	void draw(const RenderCtx* rc);

protected:
	bool notifyBegin(void* objParamRef);
	void notifyEnd(void* objParamRef);

	//======================================================================
	// 変数
private:
	PrpzlOperationData* _opdatRef;
	PurePuzzle* _pzl;
	PrpzlPerfBase* _perf;
	Camera* _camRef;						// 専用カメラへの参照
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MODE_PUREPUZZLE_PRPZL_PLAY_MODE_H_
