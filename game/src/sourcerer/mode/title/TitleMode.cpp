/***********************************************************************//**
 *	TitleMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/10.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "TitleMode.h"

// Friends
#include "TitleRole.h"
#include "../../body/Game.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// TitleMode メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
TitleMode::TitleMode()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool TitleMode::create()
{
	if(!GameModeBase::create(0, 0, new TitleRole()))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
bool TitleMode::notifyBegin(void* objParamRef)
{
	FTRACE("== {TitleMode::notifyBegin} 000.\n");
	// パラメータ取得
	GameModeParam* gmparam = (GameModeParam*)objParamRef;
	bool isResumeStart = TFW_IS_FLAG(gmparam->getFlags(), GameModeParam::F_RESUME_START);
	bool isPasteSavesBoot = TFW_IS_FLAG(gmparam->getFlags(), GameModeParam::F_PASTE_SAVES_BOOT);
	if(gmparam != 0L)	{	gmparam->clearFlags();	}
	if(isResumeStart)	{	((TitleRole*)getSelfRole())->setResumeStart();	}
	
	FTRACE("== {TitleMode::notifyBegin} 011. flags=0x%08x, resume=%d, paste=%d\n", gmparam->getFlags(), isResumeStart, isPasteSavesBoot);
	// 背ペーストボードの状態で判定する
	// 実質、起動 URL の "?PasteSaves" は意味をなしていないが機構自体は残しておくことにした
	#if defined(_ENLIGHT_V01) && !defined(_ENLIGHT_V01_LITE)
		if(TitleRole::isTransferSavesPasterboardValid())
		{
			isPasteSavesBoot = true;
		}
	#endif
	
	FTRACE("== {TitleMode::notifyBegin} 021. flags=0x%08x, resume=%d, paste=%d\n", gmparam->getFlags(), isResumeStart, isPasteSavesBoot);
	// ロールコマンド設定
	getSelfRole()->reset();
	if(!isResumeStart && !isPasteSavesBoot)
	{
		FTRACE("== {TitleMode::notifyBegin} 022.\n");
		getSelfRole()->stackCommand(TitleRole::LOGO, 0L);
	}
	if(isPasteSavesBoot)
	{
		FTRACE("== {TitleMode::notifyBegin} 023.\n");
		getSelfRole()->stackCommand(TitleRole::PASTE_SAVES, 0L);
	}
	getSelfRole()->stackCommand(TitleRole::STABLE, 0L);
	getSelfRole()->begin();

	FTRACE("== {TitleMode::notifyBegin} 999.\n");
	return true;
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void TitleMode::notifyEnd(void* objParamRef)
{
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
