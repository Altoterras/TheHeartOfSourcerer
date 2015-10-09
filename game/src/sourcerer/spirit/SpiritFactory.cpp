/***********************************************************************//**
 *	SpiritFactory.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/23.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SpiritFactory.h"

// Friends
#include "char/npc/EnemySpirit.h"
//#include "char/npc/implement/ExtraSpirit.h"
#include "../episode/char/pc/MySpirit.h"
#include "../episode/char/pc/PartnerSpirit.h"
#include "../episode/char/pc/HelperSpirit.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// SpiritFactory メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	ユニット作成
**//*---------------------------------------------------------------------*/
Spirit* SpiritFactory::makeSpirit(SpiritType sprttype)
{
	Spirit* spirit = 0L;
	switch(sprttype)
	{
	case SPRTTYPE_MY:		spirit = new MySpirit();			break;
	case SPRTTYPE_PARTNER:	spirit = new PartnerSpirit();		break;
	case SPRTTYPE_HELPER:	spirit = new HelperSpirit();		break;
	case SPRTTYPE_ENEMY:	spirit = new EnemySpirit();			break;
//	case SPRTTYPE_EXTRA:	spirit = new ExtraSpirit();			break;
	default:				spirit = new Spirit(SPRTTYPE_NULL);	break;
	}
	ASSERT(spirit != 0L);
	return spirit;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
