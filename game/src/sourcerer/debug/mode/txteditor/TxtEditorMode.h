/***********************************************************************//**
 *	TxtEditorMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/11/26.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_DEBUG_MODE_TXTEDITOR_TXT_EDITOR_MODE_H_
#define _SRCR_DEBUG_MODE_TXTEDITOR_TXT_EDITOR_MODE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../mode/GameMode.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class TxtBox;

TFW_END_NS

SRCR_BEGIN_NS

class SourceEditWindow;

/*---------------------------------------------------------------------*//**
 *	テキストエディタ モード
 *
**//*---------------------------------------------------------------------*/
class TxtEditorMode : public GameModeBase
{
	//======================================================================
	// 定数
public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual void callCustomEvent(void* objRes, const VcString* strParam, const void* objParam);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	TxtEditorMode();
	~TxtEditorMode();
	void exec(ExecRes* res, const ExecCtx* ec);
	void render(const RenderCtx* rc);
	void draw(const RenderCtx* rc);

protected:
	bool notifyBegin(void* objParamRef);
	void notifyEnd(void* objParamRef);

	//======================================================================
	// 変数
private:
	TxtBox*		_txtbox;		// テキストボックス
	s32			_dispCnt;		// 表示カウンタ
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_DEBUG_MODE_TXTEDITOR_TXT_EDITOR_MODE_H_
