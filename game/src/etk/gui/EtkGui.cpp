/***********************************************************************//**
 *	EtkGui.cpp
 *	Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2009/10/01.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EtkGui.h"

// Friends
#include "window/implement/ActionMsgWindow.h"
#include "window/implement/SysMsgWindow.h"
#include "../body/EtkTaskModDef.h"

// External
#include "../../tfw/framemod/FedTaskSys.h"
#include "../../tfw/gcmn/Texture.h"

// Library

////////////////////////////////////////////////////////////////////////////

ETK_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EtkGui メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	何か操作をロックする GUI を開いているかどうか
**//*---------------------------------------------------------------------*/
bool EtkGui::isOpenedLockGui() const
{
	if(_wndSysMsg->isShowWindow())		{	return true;	}
	return false;
}

/*---------------------------------------------------------------------*//**
	実行制御フラグを設定する
**//*---------------------------------------------------------------------*/
void EtkGui::setExecFlags(u32 eflags, bool isValid)
{
	TFW_SET_FLAG(_eflags, eflags, isValid);

	// 一時隠しの制御
	bool isTmpHide = TFW_IS_FLAG(_eflags, EF_HIDE_TEMPORARY);
	_wndActMsg->setActFlags(Window::AWNDF_TMP_HIDE, isTmpHide);
	_wndSysMsg->setActFlags(Window::AWNDF_TMP_HIDE, isTmpHide);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御
	
/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EtkGui::EtkGui()
	: _eflags(0)
	, _wndSysMsg(0L)
	, _wndActMsg(0L)
	, _fontsetRef(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EtkGui::~EtkGui()
{
	ASSERT(_wndSysMsg == 0L);
	ASSERT(_wndActMsg == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool EtkGui::create(FontSet* fontsetRef, FedTaskSys* tasksys)
{
	// フォントセット参照を保存
	_fontsetRef = fontsetRef;

	// システムメッセージウインドウを作成する
	_wndSysMsg = makeSysMsgWindow();

	// アクションメッセージウインドウを作成する
	_wndActMsg = makeActionMsgWindow();

	// タスクシステムに各モジュールを登録する（描画順をそのままヒント ID に流用する）
	if (!tasksys->registerExecModule(_wndActMsg, EtkTaskModDef::calcExecOrder(EtkTaskModDef::MOD_ACTMSG_WND), EtkTaskModDef::MOD_ACTMSG_WND, EtkTaskModDef::MGF_WINDOW)) { ASSERT(false); }
	if (!tasksys->registerExecModule(_wndSysMsg, EtkTaskModDef::calcExecOrder(EtkTaskModDef::MOD_SYS_MSG_WND), EtkTaskModDef::MOD_SYS_MSG_WND, EtkTaskModDef::MGF_WINDOW)) { ASSERT(false); }
	if (!tasksys->registerDrawModule(_wndActMsg, EtkTaskModDef::calcDrawOrder(EtkTaskModDef::MOD_ACTMSG_WND), false, EtkTaskModDef::MOD_ACTMSG_WND, EtkTaskModDef::MGF_WINDOW)) { ASSERT(false); }
	if (!tasksys->registerDrawModule(_wndSysMsg, EtkTaskModDef::calcDrawOrder(EtkTaskModDef::MOD_SYS_MSG_WND), false, EtkTaskModDef::MOD_SYS_MSG_WND, EtkTaskModDef::MGF_WINDOW)) { ASSERT(false); }

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void EtkGui::destroy()
{
	// フォントセット参照を NULL 化
	_fontsetRef = 0L;

	// アクションメッセージウインドウを削除する
	if(_wndActMsg != 0L)
	{
		_wndActMsg->destroy();
		delete _wndActMsg;
		_wndActMsg = 0L;
	}

	// システムメッセージウインドウを作成する
	if(_wndSysMsg != 0L)
	{
		_wndSysMsg->destroy();
		delete _wndSysMsg;
		_wndSysMsg = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	スクリーンリサイズ通知
**//*---------------------------------------------------------------------*/
void EtkGui::notifyResizeView()
{
}

////////////////////////////////////////////////////////////////////////////

ETK_END_NS
