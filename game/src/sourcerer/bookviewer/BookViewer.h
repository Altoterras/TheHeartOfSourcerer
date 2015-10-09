/***********************************************************************//**
 *	BookViewer.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/01/21.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_BOOKVIEW_BOOK_VIEWER_H_
#define _SRCR_BOOKVIEW_BOOK_VIEWER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../tfw/lib/Vector.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;
class Texture;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	モデルビューワー
 *
**//*---------------------------------------------------------------------*/
class BookViewer
{
	//======================================================================
	// 定数
private:
	// 操作状態
	static const u8 OPSTAT_STABLE		= 0;	// 通常
	static const u8 OPSTAT_FLIP_L		= 1;	// 左ページめくり中
	static const u8 OPSTAT_FLIP_R		= 2;	// 右ページめくり中

	// ページめくりの頂点分割数
	static const s32 NUM_DIV_HZ			= 10;	// 水平方向
	static const s32 NUM_DIV_VT			= 10;	// 垂直方向

	// ページサイズ
	#define PAGE_W 1000.0f
	#define PAGE_H 1000.0f

	// 表示ページ定義
	enum DispPageDef
	{
		DPD_LEFT,
		DPD_FLIP_LEFT,
		DPD_FLIP_RIGHT,
		DPD_RIGHT,
		NUM_DPD
	};

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	設定
	 *
	**//*-----------------------------------------------------------------*/
	class Config
	{
	public:
		s32 _numPage;		// ページ数
		s32 _fidFirst;		// 最初のファイル ID（要連番）
		bool _isLeftBind;	// 左綴じ

		Config() : _numPage(0), _fidFirst(0), _isLeftBind(false) {}
	};

private:
	/*-----------------------------------------------------------------*//**
	 *	表示ページ
	 *
	**//*-----------------------------------------------------------------*/
	class DispPage
	{
	public:
		Texture* _tex;
		s32 _page;
		s32 _fileid;

		DispPage() : _tex(0L), _page(0), _fileid(0) {}
	};

	/*-----------------------------------------------------------------*//**
	 *	ページめくり頂点
	 *
	**//*-----------------------------------------------------------------*/
	class FlpVtx
	{
	public:
		Vector3F _vtx;

		FlpVtx() {}
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	BookViewer();
	~BookViewer();
	bool create();
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);
	void render(const RenderCtx* rc);

	bool begin(Config* conf, void* objParamRef);
	void end(void* objParamRef);

private:
	void renderPage(const RenderCtx* rc, const Vector3F* v1, const Vector3F* v2, f32 tu, f32 tv, f32 tw, f32 th);
	void renderFlipPage(const RenderCtx* rc, f32 tu, f32 tv, f32 tw, f32 th, bool isFrontSide);
	void setupFlipPageStartVertex(bool isLeftPage);
	void setupDispPageTexture();

	//======================================================================
	// 変数
private:
	Config _conf;
	s32 _pageCur;			// 現在のページ番号（0 が表紙、とする）
	u8 _opstat;				// 操作状態
	u8 _fliprateCur;		// 現在のページめくり率（0 ～ 100）
	u8 _fliprateDrag;		// ドラッグにおけるページめくり率（_fliprate の目標値）
	bool _hold;
	DispPage _dparr[NUM_DPD];
	FlpVtx _fvarr[NUM_DIV_HZ + 1][NUM_DIV_VT + 1];
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_BOOKVIEW_BOOK_VIEWER_H_
