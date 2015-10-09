/***********************************************************************//**
 *  FedTaskSys.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/11/11.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "FedTaskSys.h"

// Friends
#include "../collection/List.h"

// External

// Library

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// FedTaskSys メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	フレーム処理タスクイテレータを得る

	@return	フレーム処理タスクイテレータ
**//*---------------------------------------------------------------------*/
ListIterator<FedTaskSys::ExecTask*> FedTaskSys::getExecTaskIterator()
{
	return _listExec->iterator();
}

/*---------------------------------------------------------------------*//**
	フレーム描画タスクイテレータを得る

	@return	フレーム描画タスクイテレータ
**//*---------------------------------------------------------------------*/
ListIterator<FedTaskSys::DrawTask*> FedTaskSys::getDrawTaskIterator()
{
	return _listDraw->iterator();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	フレーム処理モジュールを登録
	
	@param	modExec		モジュールへの参照
	@param	order		処理順
	@param	idHint		ヒント ID（後に必要ない場合は適当に 0 など指定も可）
	@retval	true		成功
	@retval	false		失敗
**//*---------------------------------------------------------------------*/
bool FedTaskSys::registerExecModule(FrameExec* modExec, s32 order, s32 idHint, u16 gflags)
{
	if(modExec == 0)	{	return false;	}

	ExecTask* task = new ExecTask();
	task->_idHint = idHint;
	task->_mod = modExec;
	task->_order = order;
	task->_gflags = gflags;

	s32 orderCur = S32_MIN;
	s32 orderNext = S32_MIN;
	ListIterator<ExecTask*> itCur = _listExec->iterator();
	ListIterator<ExecTask*> itNext;
	while(itCur.has())
	{
		orderCur = itCur.object()->_order;

		itNext = itCur;
		itNext.next();
		orderNext = itNext.has() ? itNext.object()->_order : S32_MAX;	// S32_MAX は必ず範囲内に入るように…

		if((orderCur <= order) && (order <= orderNext))
		{
			_listExec->insertAfter(itCur.node(), task);
			return true;
		}
		else if(order <= orderCur)
		{
			_listExec->insertBefore(itCur.node(), task);
			return true;
		}

		itCur = itNext;
	}

	_listExec->addTail(task);
	return true;
}

/*---------------------------------------------------------------------*//**
	描画モジュールを登録
	
	@param	modExec		モジュールへの参照
	@param	order		処理順
	@param	is3dRender	true の場合は 3D 描画を呼び出す
	@param	idHint		ヒント ID （後に不要な場合は適当に 0 など指定も可）
	@retval	true		成功
	@retval	false		失敗
**//*---------------------------------------------------------------------*/
bool FedTaskSys::registerDrawModule(FrameDraw* modDraw, s32 order, bool is3dRender, s32 idHint, u16 gflags)
{
	if(modDraw == 0)	{	return false;	}

	DrawTask* task = new DrawTask();
	task->_idHint = idHint;
	task->_mod = modDraw;
	task->_order = order;
	task->_tflags |= is3dRender ? TF_RENDER_3D : TF_RENDER_2D;
	task->_gflags = gflags;

	s32 orderCur = S32_MIN;
	s32 orderNext = S32_MIN;
	ListIterator<DrawTask*> itCur = _listDraw->iterator();
	ListIterator<DrawTask*> itNext;
	while(itCur.has())
	{
		orderCur = itCur.object()->_order;

		itNext = itCur;
		itNext.next();
		orderNext = itNext.has() ? itNext.object()->_order : S32_MAX;	// S32_MAX は必ず範囲内に入るように…

		if((orderCur <= order) && (order <= orderNext))
		{
			_listDraw->insertAfter(itCur.node(), task);
			return true;
		}
		else if(order <= orderCur)
		{
			_listDraw->insertBefore(itCur.node(), task);
			return true;
		}

		itCur = itNext;
	}

	_listDraw->addTail(task);
	return true;
}

/*---------------------------------------------------------------------*//**
	制御モジュールを登録削除
	
	@param	modExec		モジュールへの参照
	@param	isSingle	一つ削除したら抜けるかどうか
**//*---------------------------------------------------------------------*/
bool FedTaskSys::removeExecModule(FrameExec* modExec, bool isSingle)
{
	bool isRemoved = false;

	ListIterator<ExecTask*> itCur = _listExec->iterator();
	ListIterator<ExecTask*> itNext;
	while(itCur.has())
	{
		itNext = itCur;
		itNext.next();

		if(itCur.object()->_mod == modExec)
		{
			_listExec->removeNode(itCur.node());
			isRemoved = true;
			if(isSingle)	{	break;	}
		}

		itCur = itNext;
	}

	return isRemoved;
}

/*---------------------------------------------------------------------*//**
	描画モジュールを登録削除
	
	@param	modDraw		モジュールへの参照
	@param	isSingle	一つ削除したら抜けるかどうか
	@retval	true		削除成功
	@retval	false		削除されなかった
**//*---------------------------------------------------------------------*/
bool FedTaskSys::removeDrawModule(FrameDraw* modDraw, bool isSingle)
{
	bool isRemoved = false;

	ListIterator<DrawTask*> itCur = _listDraw->iterator();
	ListIterator<DrawTask*> itNext;
	while(itCur.has())
	{
		itNext = itCur;
		itNext.next();

		if(itCur.object()->_mod == modDraw)
		{
			_listDraw->removeNode(itCur.node());
			isRemoved = true;
			if(isSingle)	{	break;	}
		}

		itCur = itNext;
	}

	return isRemoved;
}

/*---------------------------------------------------------------------*//**
	ヒント ID によるモジュールを登録削除
		※同一ヒントは全て削除する	
	
	@param	idHint		ヒント ID
**//*---------------------------------------------------------------------*/
bool FedTaskSys::removeModule(s32 idHint)
{
	bool isRemoved = false;

	{
		ListIterator<ExecTask*> itCur = _listExec->iterator();
		ListIterator<ExecTask*> itNext;
		while(itCur.has())
		{
			itNext = itCur;
			itNext.next();
			if(itCur.object()->_idHint == idHint)
			{
				_listExec->removeNode(itCur.node());
				isRemoved = true;
			}
			itCur = itNext;
		}
	}
	{
		ListIterator<DrawTask*> itCur = _listDraw->iterator();
		ListIterator<DrawTask*> itNext;
		while(itCur.has())
		{
			itNext = itCur;
			itNext.next();
			if(itCur.object()->_idHint == idHint)
			{
				_listDraw->removeNode(itCur.node());
				isRemoved = true;
			}
			itCur = itNext;
		}
	}

	return isRemoved;
}

/*---------------------------------------------------------------------*//**
	制御モジュールの存在確認
	
	@param	idHint		ヒント ID
**//*---------------------------------------------------------------------*/
bool FedTaskSys::isExistExecModule(s32 idHint)
{
	ListIterator<ExecTask*> itCur = _listExec->iterator();
	while(itCur.has())
	{
		if(itCur.object()->_idHint == idHint)
		{
			return true;
		}
		itCur.next();
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	制御モジュールの存在確認
	
	@param	idHint		ヒント ID
**//*---------------------------------------------------------------------*/
bool FedTaskSys::isExistDrawModule(s32 idHint)
{
	ListIterator<DrawTask*> itCur = _listDraw->iterator();
	while(itCur.has())
	{
		if(itCur.object()->_idHint == idHint)
		{
			return true;
		}
		itCur.next();
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	モジュールの存在確認
	
	@param	idHint		ヒント ID
**//*---------------------------------------------------------------------*/
bool FedTaskSys::isExistModule(s32 idHint)
{
	if(isExistExecModule(idHint))	{	return true;	}
	if(isExistDrawModule(idHint))	{	return true;	}
	return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
FedTaskSys::FedTaskSys()
	: _listExec(0L)
	, _listDraw(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
FedTaskSys::~FedTaskSys()
{
	ASSERT(_listExec == 0L);
	ASSERT(_listDraw == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool FedTaskSys::create()
{
	// リスト作成
	_listExec = new List<ExecTask*>(true);
	_listDraw = new List<DrawTask*>(true);

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void FedTaskSys::destroy()
{
	// リスト削除
	if(_listDraw != 0L)
	{
		delete _listDraw;
		_listDraw = 0L;
	}
	if(_listExec != 0L)
	{
		delete _listExec;
		_listExec = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void FedTaskSys::exec(ExecRes* res, const ExecCtx* ec)
{
	for(ListIterator<ExecTask*> it = _listExec->iterator(); it.has(); it.next())
	{
		it.object()->_mod->exec(res, ec);
	}
}

/*---------------------------------------------------------------------*//**
	3D 描画
**//*---------------------------------------------------------------------*/
void FedTaskSys::render(const RenderCtx* rc)
{
	for(ListIterator<DrawTask*> it = _listDraw->iterator(); it.has(); it.next())
	{
		if (TFW_IS_FLAG(it.object()->_tflags, TF_RENDER_3D))
		{
			it.object()->_mod->render(rc);
		}
	}
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void FedTaskSys::draw(const RenderCtx* rc)
{
	for(ListIterator<DrawTask*> it = _listDraw->iterator(); it.has(); it.next())
	{
		if (TFW_IS_FLAG(it.object()->_tflags, TF_RENDER_2D))
		{
			it.object()->_mod->draw(rc);
		}
	}
}

/*---------------------------------------------------------------------*//**
	3D と 2D 混合描画
**//*---------------------------------------------------------------------*/
void FedTaskSys::renderAndDraw(const RenderCtx* rc)
{
	for(ListIterator<DrawTask*> it = _listDraw->iterator(); it.has(); it.next())
	{
		if (TFW_IS_FLAG(it.object()->_tflags, TF_RENDER_3D))
		{
			it.object()->_mod->render(rc);
		}
		else if (TFW_IS_FLAG(it.object()->_tflags, TF_RENDER_2D))
		{
			it.object()->_mod->draw(rc);
		}
	}
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
