/***********************************************************************//**
 *	BookViewMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/01/19.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MODE_BOOKVIEW_BOOK_VIEW_MODE_H_
#define _SRCR_MODE_BOOKVIEW_BOOK_VIEW_MODE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../GameMode.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Camera;

TFW_END_NS

SRCR_BEGIN_NS

class BookViewer;

/*---------------------------------------------------------------------*//**
 *	ブック ビュー モード
 *
**//*---------------------------------------------------------------------*/
class BookViewMode : public GameModeBase
{
	//======================================================================
	// 定数
public:
	// 制御モード
	enum ActMode
	{
		AMODE__DUMMY_1,		// 
		AMODE__DUMMY_2,		// 
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
	BookViewMode();
	~BookViewMode();
	void exec(ExecRes* res, const ExecCtx* ec);
	void render(const RenderCtx* rc);
	void draw(const RenderCtx* rc);

protected:
	bool notifyBegin(void* objParamRef);
	void notifyEnd(void* objParamRef);

private:
	bool beginActMode(ActMode amode, bool isFirst);
	void endActMode();

	//======================================================================
	// 変数
private:
	BookViewer*		_bookvw;		// ブックビューワー
	Camera*			_camRef;		// 専用カメラへの参照
	ActMode			_amode;			// 制御モード
	s32				_dispCnt;		// 表示カウンタ
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MODE_BOOKVIEW_BOOK_VIEW_MODE_H_
