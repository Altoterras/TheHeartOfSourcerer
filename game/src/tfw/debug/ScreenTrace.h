/***********************************************************************//**
 *	ScreenTrace.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2010/01/15.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_DEBUG_SCREEN_TRACE_H_
#define _TFW_DEBUG_SCREEN_TRACE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../framemod/FrameExecDraw.h"

#include "../string/VcString.h"
#include "../string/CcString.h"

////////////////////////////////////////////////////////////////////////////
// クラス

namespace tfw
{

class Font;

/*---------------------------------------------------------------------*//**
 *	スクリーン トレース
 *	
**//*---------------------------------------------------------------------*/
class ScreenTrace
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void trace(const CStringBase* str);
	void trace(const char* str);
	void traceFormat(const char* str, ...);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ScreenTrace();
	~ScreenTrace();
	bool create(Font* fontRef, s32 xBase, s32 yBase, s32 sizeFont);
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	void draw(const RenderCtx* rc);				// 2D 描画


	//======================================================================
	// 変数
public:
	VcString _str[2];
	u8 _dside;

	s32 _xBase;
	s32 _yBase;
	s32 _sizeFont;
	Font* _fontRef;
};

}	// namespace tfw

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_DEBUG_SCREEN_TRACE_H_

