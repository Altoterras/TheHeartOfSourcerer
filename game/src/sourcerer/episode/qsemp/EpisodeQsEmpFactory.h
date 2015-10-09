/***********************************************************************//**
 *	EpisodeQsEmpFactory.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/21.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_QSEMP_EPISODE_QS_EMP_FACTORY_H_
#define _SRCR_EPISODE_QSEMP_EPISODE_QS_EMP_FACTORY_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class EventMatter;

/*---------------------------------------------------------------------*//**
 *	クエスト用イベント マター プログラム ファクトリー
 *
**//*---------------------------------------------------------------------*/
class EpisodeQsEmpFactory
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static EventMatter* newEventMatter(s32 questid, u32 caseid);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_QSEMP_EPISODE_QS_EMP_FACTORY_H_
