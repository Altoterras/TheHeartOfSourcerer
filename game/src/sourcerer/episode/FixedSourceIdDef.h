/***********************************************************************//**
 *	FixedSourceIdDef.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/08/09.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_FIXED_SOURCE_ID_DEF_H_
#define _SRCR_EPISODE_FIXED_SOURCE_ID_DEF_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	固定ソース ID の定義
 *
**//*---------------------------------------------------------------------*/
class FixedSourceIdDef
{
	//======================================================================
	// 定数
public:
	static const s32	FSRCID_EP01_AIER			= 1001;		// トランス
	static const s32	FSRCID_EP01_NERUROS			= 1002;		// 水飛沫
	static const s32	FSRCID_EP01_NERUHAMAD		= 1003;		// 水のベール
	static const s32	FSRCID_EP01_DANERUROS		= 1004;		// 水飛沫の攻撃
	static const s32	FSRCID_EP01_NERUTRAOS		= 1005;		// 回復
	static const s32	FSRCID_EP01_DEBIDULO		= 1006;		// 倦怠化
	static const s32	FSRCID_EP01_NESTAGLAM		= 1007;		// 錯乱回復
	static const s32	FSRCID_EP01_LUCSEGREFOC		= 1008;		// 発光
	static const s32	FSRCID_EP01_ASCENERNERU		= 1009;		// 水単元素上昇シフト
	static const s32	FSRCID_EP01_FAURAC			= 1010;		// 暖粒
	static const s32	FSRCID_EP01_DANERUMOL		= 1011;		// 水球の攻撃
	static const s32	FSRCID_EP01_FOGOHAMAD		= 1012;		// 火のベール
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_FIXED_SOURCE_ID_DEF_H_
