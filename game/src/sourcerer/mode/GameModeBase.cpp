/***********************************************************************//**
 *	GameModeBase.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/12.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "GameModeBase.h"

// Friends
#include "GameMode.h"
#include "../body/Game.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// GameModeBase メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

#if defined(_DEBUG)
/*---------------------------------------------------------------------*//**
	デバッグ用モード状態出力
**//*---------------------------------------------------------------------*/
void GameModeBase::straceModeStatus()
{
	STRACE("[%d]", getSelfModeId());
	for(int i = 0; i < getReservedModeNum(); i++)
	{
		STRACE((i == 0) ? " (" : ", ");
		STRACE("%d", getReservedMode(i));
		if(i == (getReservedModeNum() - 1)) { STRACE(")"); }
	}
	/*
	for(int i = 0; i < getStackedModeNum(); i++)
	{
		if(i != 0) { STRACE(" <"); }
		STRACE(" %d", getStackedMode(i));
	}
	*/
	STRACE("\n");
}

/*---------------------------------------------------------------------*//**
	デバッグ用モードツリー出力
**//*---------------------------------------------------------------------*/
void GameModeBase::straceModeTreeDive(int dive)
{
	for(int i = 0; i < dive; i++) { STRACE(" "); }
	straceModeStatus();

#if 1
	GameModeBase* gmodeWk = (GameModeBase*)getCurrentMode();
	if(gmodeWk != 0L)
	{
		gmodeWk->straceModeTreeDive(dive + 1);
	}
#else
	for(int i = 0; i < getChildModeNum(); i++)
	{
		GameModeBase* gmodeWk = (GameModeBase*)getChildMode(i);
		gmodeWk->straceModeTreeDive(dive + 1);
	}
#endif
}
#endif

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
		GameModeBase* gmodeChild = (GameModeBase*)getChildMode(i);
		if(gmodeChild != 0L)
		{
			gmodeChild->notifyChangedMap(isDeserializeCase);
		}
	}
}

/*---------------------------------------------------------------------*//**
	ソウマ発射通知
**//*---------------------------------------------------------------------*/
void GameModeBase::notifyDischargeSouma(Souma* souma, u16 mcid)
{
	// 子に伝達
	for(s32 i = 0; i < getChildModeNum(); i++)
	{
		GameModeBase* gmodeChild = (GameModeBase*)getChildMode(i);
		if(gmodeChild != 0L)
		{
			gmodeChild->notifyDischargeSouma(souma, mcid);
		}
	}
}

/*---------------------------------------------------------------------*//**
	ユニットのステータス影響通知
**//*---------------------------------------------------------------------*/
void GameModeBase::notifyUnitInfluenceStat(Unit* unitRecv, const StatusChangeResult* scres, const TransStat* tstat)
{
	// 子に伝達
	for(s32 i = 0; i < getChildModeNum(); i++)
	{
		GameModeBase* gmodeChild = (GameModeBase*)getChildMode(i);
		if(gmodeChild != 0L)
		{
			gmodeChild->notifyUnitInfluenceStat(unitRecv, scres, tstat);
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
short GameModeBase::changeModeDive(short modeid)
{
	// 自分が担当している範囲のモードの場合は自分で変更して true を返す
	if((_modeidStart <= modeid) && (modeid <= _modeidEnd))
	{
		reserveMode(modeid);	// 予約する
		modeid = reserveDefaultChildModeDive(modeid, 0);	// 更に子ノードの初期モードを予約する
		return modeid;
	}

	// 子ノードに任せてみる
	for(s32 i = 0; i < getChildModeNum(); i++)
	{
		GameModeBase* gmodeChild = (GameModeBase*)getChildMode(i);
		short modeidEnd = gmodeChild->changeModeDive(modeid);
		if(modeidEnd != 0)
		{
			// 子ノードが適合する場合、自身のモード指定がその子を指定していない場合は自身のモードも予約する
			short modeidChild = gmodeChild->getSelfModeId();
			if(modeidChild != getCurrentModeId())	// 現在の自身のモードが適合する子ノードでない場合
			{
				reserveMode(modeidChild);
			}
			return modeidEnd;
		}
	}

	return 0;
}

/*---------------------------------------------------------------------*//**
	子のモードを初期モードに変更する
**//*---------------------------------------------------------------------*/
short GameModeBase::reserveDefaultChildModeDive(short modeid, int divecnt)
{
	short modeidChild = modeid;
	if(divecnt > 0)
	{
		modeidChild = getDefaultChildMode();
		if(modeidChild == 0)
		{
			return 0;
		}
		reserveMode(modeidChild);
	}

	for(s32 i = 0; i < getChildModeNum(); i++)
	{
		GameModeBase* gmodeChild = (GameModeBase*)getChildMode(i);

		if(gmodeChild->getSelfModeId() != modeidChild)
		{
			continue;;
		}

		short modeidEnd = gmodeChild->reserveDefaultChildModeDive(modeidChild, divecnt + 1);
		if(modeidEnd != 0)
		{
			return modeidEnd;
		}
	}

	return modeidChild;
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

/*---------------------------------------------------------------------*//**
	子モード変更
**//*---------------------------------------------------------------------*/
void GameModeBase::changeChildMode(short modeid)
{
	Mode::changeMode(modeid, false);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
