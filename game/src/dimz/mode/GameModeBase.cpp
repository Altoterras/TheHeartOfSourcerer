/***********************************************************************//**
 *	GameModeBase.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/06/29.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "GameModeBase.h"

// Friends
#include "../body/DimzBody.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

DIMZ_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// GameModeBase メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// プロパティ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
GameModeBase::GameModeBase()
	: _modeidStart(0)
	, _modeidEnd(0)
{
}

/*---------------------------------------------------------------------*//**
	マップ変更後通知
**//*---------------------------------------------------------------------*/
void GameModeBase::notifyChangedMap(bool isDeserializeCase)
{
	// 子に伝達
	for(s32 i = 0; i < getChildModeNum(); i++)
	{
		GameModeBase* gmodeWk = (GameModeBase*)getChildMode(i);
		if(gmodeWk != 0L)
		{
			gmodeWk->notifyChangedMap(isDeserializeCase);
		}
	}
}

/*---------------------------------------------------------------------*//**
	モード変更通知
**//*---------------------------------------------------------------------*/
void GameModeBase::notifyChildModeChanged(short modeid, void* objParamRef)
{
	Mode* modeParent = this->getParentMode();
	if(modeParent != 0L)
	{
		modeParent->notifyChildModeChanged(modeid, objParamRef);
	}
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool GameModeBase::create(short modeidStart, short modeidEnd, Role* roleSelf)
{
	// 範囲を保存
	_modeidStart = modeidStart;
	_modeidEnd = modeidEnd;

	// 範囲からスタック数を計算して作成する
	s32 numStack = _modeidEnd - _modeidStart - 1;
	if(numStack < 0)	{	numStack = 0;	}
	if(!Mode::create(numStack, roleSelf, true))
	{
		return false;
	}

	// exec で setFromReservedMode を自動呼出しする
	setResolveReseredModeOnExec(true);

	return true;
}

/*---------------------------------------------------------------------*//**
	子モード登録
**//*---------------------------------------------------------------------*/
bool GameModeBase::registerMode(short modeid, Mode* mode)
{
	mode->setParameter(this->getParameter());	// 汎用パラメータを設定

	return Mode::registerMode(modeid, mode);
}

/*---------------------------------------------------------------------*//**
	末端のモードを変更する
	※ 指定モードが自ら保持している子モードであればモードチェンジを行う
**//*---------------------------------------------------------------------*/
bool GameModeBase::changeModeDive(short modeid)
{
	// 自分が担当している範囲のモードの場合は自分で変更して true を返す
	if((_modeidStart <= modeid) && (modeid <= _modeidEnd))
	{
		reserveMode(modeid);	// 予約する
		return true;
	}

	// 子ノードに任せてみる
	for(s32 i = 0; i < getChildModeNum(); i++)
	{
		GameModeBase* gmodeWk = (GameModeBase*)getChildMode(i);
		if(gmodeWk->changeModeDive(modeid))
		{
			// 子ノードが適合する場合、自身のモード指定がその子を指定していない場合は自身のモードも予約する
			if(gmodeWk->getSelfModeId() != getCurrentModeId())	// 現在の自身のモードが適合する子ノードでない場合
			{
				reserveMode(gmodeWk->getSelfModeId());	// 適合子モードに移行するように予約する
			}
			return true;
		}
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	末端の予約モードを得る
**//*---------------------------------------------------------------------*/
short GameModeBase::getLastReservedModeDive()
{
	// 予約モードがある場合は返す
	s16 modeReserved = getLastReservedMode();
	if(modeReserved != 0)
	{
		return modeReserved;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////

DIMZ_END_NS
