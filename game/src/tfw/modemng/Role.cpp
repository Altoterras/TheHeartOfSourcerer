/***********************************************************************//**
 *	Role.cpp
 *  Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/06/23.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Role.h"

// Friends
#include "../collection/List.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Role メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	コマンドを積む
**//*---------------------------------------------------------------------*/
void Role::stackCommand(s32 cid, void* param)
{
	_listCmdStck->addTail(new Command(cid, param));
}

/*---------------------------------------------------------------------*//**
	スタック実行の強制終了フラグ設定
	※　現在実行中のコマンドは終わりまで実行する
**//*---------------------------------------------------------------------*/
void Role::setForceEndStack(bool isForceEndStack)
{
	_isForceEndStack = isForceEndStack;
}

/*---------------------------------------------------------------------*//**
	コマンドの強制送りフラグ設定
**//*---------------------------------------------------------------------*/
void Role::setForceNextCommand(bool isForceNextCommand)
{
	_isForceNextCommand = isForceNextCommand;
}

/*---------------------------------------------------------------------*//**
	コマンドの強制ジャンプ設定
**//*---------------------------------------------------------------------*/
bool Role::setForceJumpCommand(s32 cid)
{
	s32 index = -1;
	for(ListIterator<Command*> it = _listCmdStck->iterator(); it.has(); it.next())
	{
		if(it.object()->_cid == cid)
		{
			_idxForceJumpCmdReserved = (u32)index;
			return true;
		}
		index++;
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	ロール開始
**//*---------------------------------------------------------------------*/
void Role::begin()
{
	_isForceEndStack = false;
	_isForceNextCommand = false;
	_idxCmd = 0;
	_idxForceJumpCmdReserved = 0xffffffff;
	_ecntBegun = 0;
	_frmcntBegun = 0.0f;

	// 最初のコマンドを設定
	nextCommand(true);

	// 開始通知
	notifyBegin();
}

/*---------------------------------------------------------------------*//**
	ロール終了
**//*---------------------------------------------------------------------*/
void Role::end()
{
	// 終了通知
	notifyEnd();

	// コマンドなし
	_idxCmd = 0xffffffff;
	_idxForceJumpCmdReserved = 0xffffffff;
	_cmdCur = 0L;

	// 全てのコマンドを終了する
	_listCmdStck->removeAll();		// コマンドクリア

	_ecntBegun = -1;
}

/*---------------------------------------------------------------------*//**
	ロールリセット
**//*---------------------------------------------------------------------*/
void Role::reset()
{
	if(_cmdCur != 0L)
	{
		// 終了通知
		notifyEnd();

		// コマンドなし
		_idxCmd = 0xffffffff;
		_cmdCur = 0L;
	}

	// 全てのコマンドを終了する
	_listCmdStck->removeAll();		// コマンドクリア

	_ecntBegun = -1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Role::Role()
	: _listCmdStck(0L)
	, _idxCmd(0)
	, _idxForceJumpCmdReserved(0xffffffff)
	, _cmdCur(0L)
	, _isForceEndStack(false)
	, _isForceNextCommand(false)
	, _stepCmd(0)
	, _ecntStep(0)
	, _ecntCmd(0)
	, _ecntBegun(-1)
	, _frmcntStep(0.0f)
	, _frmcntCmd(0.0f)
	, _frmcntBegun(0.0f)
	, _modeOwnerRef(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Role::~Role()
{
	ASSERT(_listCmdStck == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
	※	モードを使わずに単体でこのモジュールを使う場合などは、
		modeOwnerRef を NULL 指定可（内部ではとくに使用しない）．
**//*---------------------------------------------------------------------*/
bool Role::create(Mode* modeOwnerRef)
{
	// スタックリストの作成
	_listCmdStck = new List<Command*>(true);

	// オーナーの保存
	_modeOwnerRef = modeOwnerRef;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void Role::destroy()
{
	if(_ecntBegun >= 0)
	{
		end();	// 終了処理
	}

	// スタックリストの削除
	delete _listCmdStck;
	_listCmdStck = 0L;
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void Role::exec(ExecRes* res, const ExecCtx* ec)
{
	// コマンドなしは抜ける
	if(_cmdCur == 0L)	{	return;	}
	if(_ecntBegun < 0)	{	return;	}

	// 強制ジャンプ処理
	if(_idxForceJumpCmdReserved != 0xffffffff)
	{
		goCommand(_idxForceJumpCmdReserved);
		_idxForceJumpCmdReserved = 0xffffffff;
	}

	// カウンタを進める
	_ecntBegun++;
	_ecntCmd++;
	_ecntStep++;
	_frmcntBegun += ec->getDeltaFrame();
	_frmcntCmd += ec->getDeltaFrame();
	_frmcntStep += ec->getDeltaFrame();

	ExecRes resSub;
	// コマンド処理を呼び出す
	CmdParam cprm(_cmdCur->_cid, _cmdCur->_param, _stepCmd, _ecntStep, _ecntCmd, _ecntBegun, _frmcntStep, _frmcntCmd, _frmcntBegun);
	s32 stepRes = execCommand(&resSub, ec, &cprm);
	if(_stepCmd != stepRes)
	{
		_stepCmd = stepRes;
		_ecntStep = 0;
		_frmcntStep = 0.0f;
	}

	if(_idxForceJumpCmdReserved != 0xffffffff)
	{
		// 強制ジャンプ処理
		goCommand(_idxForceJumpCmdReserved);
		_idxForceJumpCmdReserved = 0xffffffff;
	}
	else if(_isForceNextCommand || resSub.isDone())
	{
		// １コマンド終了
		_isForceNextCommand = false;

		// 次のコマンド
		if(_isForceEndStack || nextCommand(false))	// 全てのコマンドが終了した
		{
			// 終了処理
			end();

			if(res != 0L)	{	res->setDone();	}
		}
	}
}

/*---------------------------------------------------------------------*//**
	3D 描画
**//*---------------------------------------------------------------------*/
void Role::render(const RenderCtx* rc)
{
	// コマンドなしは抜ける
	if(_cmdCur == 0L)	{	return;	}

	// コマンド 3D 描画
	CmdParam cprm(_cmdCur->_cid, _cmdCur->_param, _stepCmd, _ecntStep, _ecntCmd, _ecntBegun, _frmcntStep, _frmcntCmd, _frmcntBegun);
	renderCommand(rc, &cprm);
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void Role::draw(const RenderCtx* rc)
{
	// コマンドなしは抜ける
	if(_cmdCur == 0L)	{	return;	}

	// コマンド 2D 描画
	CmdParam cprm(_cmdCur->_cid, _cmdCur->_param, _stepCmd, _ecntStep, _ecntCmd, _ecntBegun, _frmcntStep, _frmcntCmd, _frmcntBegun);
	drawCommand(rc, &cprm);
}

/*---------------------------------------------------------------------*//**
 * 次のコマンドへ送る
 * 
 * @retval	true				コマンド終了
 * @retval	false				コマンド継続
**//*---------------------------------------------------------------------*/
bool Role::nextCommand(bool isFirst)
{
	if(isFirst)
	{
		return goCommand(0);
	}
	else
	{
		return goCommand(_idxCmd + 1);
	}
}

/*---------------------------------------------------------------------*//**
 * 指定コマンドへ送る
 * 
 * @retval	true				コマンド終了
 * @retval	false				コマンド継続
**//*---------------------------------------------------------------------*/
bool Role::goCommand(u32 idxCmd)
{
	_idxCmd = idxCmd;
	if(_idxCmd >= _listCmdStck->getCount())
	{
		_cmdCur = 0L;
		return true;
	}
	
	// 新しいコマンドの設定
	_cmdCur = _listCmdStck->getObject(_idxCmd);
	_ecntCmd = 0;
	_frmcntCmd = 0.0f;

	_stepCmd = 1;
	_ecntStep = 0;
	_frmcntStep = 0.0f;

	_isForceNextCommand = false;
	
	return false;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
