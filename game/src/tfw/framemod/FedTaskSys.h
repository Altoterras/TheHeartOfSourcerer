/***********************************************************************//**
 *  FedTaskSys.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2010/03/11.
 *  Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_FRAMEMOD_FED_TASK_SYS_H_
#define _TFW_FRAMEMOD_FED_TASK_SYS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../tfw/framemod/FrameExecDraw.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class List;
template<class TYPE> class ListIterator;

/*---------------------------------------------------------------------*//**
 *	FrameExecDraw モジュールの簡易タスクシステム
 *	
**//*---------------------------------------------------------------------*/
class FedTaskSys : public FrameExecDraw
{
	//======================================================================
	// 定数
private:
	// タスクフラグ
	static const u16	TF_RENDER_2D	= 0x0001;	// 2D 描画呼び出し
	static const u16	TF_RENDER_3D	= 0x0002;	// 3D 描画呼び出し

	//======================================================================
	// クラス
public:
	class Task
	{
	public:
		s32				_idHint;	// Exec と Draw のヒント ID
		s32				_order;		// 処理順（小さい方が早い）
		u16				_tflags;	// タスクフラグ
		u16				_gflags;	// 外部モジュール用の汎用フラグ

		Task() : _idHint(0), _order(0), _tflags(0), _gflags(0) {}
	};

	class ExecTask : public Task
	{
	public:
		FrameExec*		_mod;		// モジュール

		ExecTask() : _mod(0L) {}
	};

	class DrawTask : public Task
	{
	public:
		FrameDraw*		_mod;		// モジュール

		DrawTask() : _mod(0L) {}
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	ListIterator<ExecTask*> getExecTaskIterator();
	ListIterator<DrawTask*> getDrawTaskIterator();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual bool	registerExecModule(FrameExec* modExec, s32 order, s32 idHint, u16 gflags);
	virtual bool	registerDrawModule(FrameDraw* modDraw, s32 order, bool is3dRender, s32 idHint, u16 gflags);
	virtual bool	removeExecModule(FrameExec* modExec, bool isSingle);
	virtual bool	removeDrawModule(FrameDraw* modExec, bool isSingle);
	virtual bool	removeModule(s32 idHint);
	virtual bool	isExistExecModule(s32 idHint);
	virtual bool	isExistDrawModule(s32 idHint);
	virtual bool	isExistModule(s32 idHint);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	FedTaskSys();
	virtual ~FedTaskSys();
	virtual bool	create();
	virtual void	destroy();
	virtual void	exec(ExecRes* res, const ExecCtx* ec);
	virtual void	render(const RenderCtx* rc);
	virtual void	draw(const RenderCtx* rc);
	virtual void	renderAndDraw(const RenderCtx* rc);

	//======================================================================
	// 変数
protected:
	List<ExecTask*>*	_listExec;		// フレーム処理リスト
	List<DrawTask*>*	_listDraw;		// 描画リスト
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_GUI_PANEL_H_
