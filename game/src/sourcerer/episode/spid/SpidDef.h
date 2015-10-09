/***********************************************************************//**
 *	SpidDef.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/16.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_SPID_SPID_DEF_H_
#define _SRCR_EPISODE_SPID_SPID_DEF_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class SorSgm;
class Unit;

/*---------------------------------------------------------------------*//**
 *	SPID （指定制御 ID）定義
 *
**//*---------------------------------------------------------------------*/
class SpidDef
{
	//======================================================================
	// 定数
public:
	const static u16	SPID_PC2_WEP_ACTIVE		= 1007;
	const static u16	SPID_PC2_WEP_INACTIVE	= 1009;
	const static u16	SPID_PC2_SLD_ACTIVE		= 1008;
	const static u16	SPID_PC2_SLD_INACTIVE	= 1010;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_SPID_SPID_DEF_H_
