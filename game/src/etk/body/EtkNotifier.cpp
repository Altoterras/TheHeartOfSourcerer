/***********************************************************************//**
 *	EtkNotifier.cpp
 *  Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2014/04/14.
 *	Copyright 2014 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// EtkNotifier

// Self
#include "EtkNotifier.h"

// Friends
#include "EtkBody.h"
#include "EtkTaskModDef.h"
#include "../gui/EtkGui.h"
#include "../gui/window/Window.h"
#include "../sg/SorSgm.h"

// External
#include "../../tfw/collection/List.h"
#include "../../tfw/framemod/FedTaskSys.h"

// Library

////////////////////////////////////////////////////////////////////////////

ETK_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EtkNotifier メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	言語変更通知
	※システムが完全に構成されていないうちに呼び出される可能性がある点に注意
**//*---------------------------------------------------------------------*/
void EtkNotifier::notifyChangeLanguage()
{
	// GUI に伝達
	EtkGui* gui = EtkBody::getBody()->getGuiManager();
	if (gui != 0L)
	{
		gui->notifyChangeLanguage();
	}

	// タスクシステムに登録済みの各ウインドウに伝達
	FedTaskSys* tasksys = EtkBody::getBody()->getTaskSys();
	for (ListIterator<FedTaskSys::ExecTask*> it = tasksys->getExecTaskIterator(); it.has(); it.next())
	{
		FedTaskSys::ExecTask* et = it.object();
		if (TFW_IS_FLAG(et->_gflags, EtkTaskModDef::MGF_WINDOW))
		{
			((Window*)et->_mod)->notifyChangeLanguage();
		}
	}
}

/*---------------------------------------------------------------------*//**
	論理スクリーンサイズ変更通知
**//*---------------------------------------------------------------------*/
void EtkNotifier::notifyChangeLogicalScreenSize()
{
	// GUI に伝達
	EtkGui* gui = EtkBody::getBody()->getGuiManager();
	if (gui != 0L)
	{
		gui->notifyChangeLogicalScreenSize();
	}

	// タスクシステムに登録済みの各ウインドウに伝達
	FedTaskSys* tasksys = EtkBody::getBody()->getTaskSys();
	for (ListIterator<FedTaskSys::ExecTask*> it = tasksys->getExecTaskIterator(); it.has(); it.next())
	{
		FedTaskSys::ExecTask* et = it.object();
		if (TFW_IS_FLAG(et->_gflags, EtkTaskModDef::MGF_WINDOW))
		{
			((Window*)et->_mod)->notifyChangeLogicalScreenSize();
		}
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EtkNotifier::EtkNotifier()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool EtkNotifier::create()
{
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void EtkNotifier::destroy()
{
}

////////////////////////////////////////////////////////////////////////////

ETK_END_NS
