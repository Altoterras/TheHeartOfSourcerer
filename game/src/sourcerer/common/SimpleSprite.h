/***********************************************************************//**
 *	SimpleSprite.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/09/26.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_COMMON_CALC_UTILS_H_
#define _SRCR_COMMON_CALC_UTILS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class ExecCtx;
class RenderCtx;

TFW_END_NS

SRCR_BEGIN_NS

class SpriteMatter;

/*---------------------------------------------------------------------*//**
 *	簡易スプライト
 *		※	チュートリアル用
 *
**//*---------------------------------------------------------------------*/
class SimpleSprite
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 有効なスプライトかどうかを得る
	bool isValid() const;
	// 終了したかどうかを得る
	bool isEnd() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	SimpleSprite();
	~SimpleSprite();
	bool create(u32 fileid, f32 frameMin, bool fitScreen, u32 colorSide, bool isProhibitOtherUiReact);
	void destroy();
	void exec(const ExecCtx* ec);
	void draw(const RenderCtx* rc);

	//======================================================================
	// 変数
private:
	SpriteMatter* _sprite;
	f32 _frameCur;
	bool _isProhibitOtherUiReact;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_COMMON_CALC_UTILS_H_
