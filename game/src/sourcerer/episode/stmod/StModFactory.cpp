/***********************************************************************//**
 *	StModFactory.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/16.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

#include "StModFactory.h"
// Self

// Friends
#include "StMod.h"
#include "implement/StMod10101.h"
#include "implement/StMod10204.h"
#include "implement/StMod10301.h"
#include "implement/StMod10303.h"
#include "implement/StMod10305.h"
#include "implement/StMod10306.h"
#include "implement/StMod10401.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// StModFactory メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	ストーリー作成
**//*---------------------------------------------------------------------*/
StMod* StModFactory::makeStMod(s32 storyid, StoryManager* strymngRef)
{
	StMod* stmod = 0L;

	switch(storyid)
	{
	case 10101:	stmod = new StMod10101();	break;
	case 10204:	stmod = new StMod10204();	break;
	case 10301:	stmod = new StMod10301();	break;
	case 10303:	stmod = new StMod10303();	break;
	case 10305:	stmod = new StMod10305();	break;
	case 10306:	stmod = new StMod10306();	break;
	case 10401:	stmod = new StMod10401();	break;
	default:	stmod = new StMod10000();	break;	// Episode01 基底
	}

	if(stmod == 0L)
	{
		return 0L;
	}

	if(!stmod->create(strymngRef))
	{
		delete stmod;
		return 0L;
	}

	return stmod;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
