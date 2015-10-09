/***********************************************************************//**
 *	QsModFactory.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/03/20.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_QUEST_QSMOD_QS_MOD_FACTORY_H_
#define _SRCR_QUEST_QSMOD_QS_MOD_FACTORY_H_

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
class QsModFactory
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static QsMod* makeQsMod(s32 questid, QuestArticle* qsdatRef);
};

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS

#endif	// _SRCR_QUEST_QSMOD_QS_MOD_FACTORY_H_
