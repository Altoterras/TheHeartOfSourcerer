/***********************************************************************//**
 *	EpisodeQsModFactory.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/21.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_QSMOD_QS_MOD_FACTORY_H_
#define _SRCR_EPISODE_QSMOD_QS_MOD_FACTORY_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class QsMod;
class QuestArticle;

/*---------------------------------------------------------------------*//**
 *	クエスト専用モジュール ファクトリー
 *
**//*---------------------------------------------------------------------*/
class EpisodeQsModFactory
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static QsMod* newQsMod(s32 questid, QuestArticle* qsdatRef);
};

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS

#endif	// _SRCR_EPISODE_QSMOD_QS_MOD_FACTORY_H_
