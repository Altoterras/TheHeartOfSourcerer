/***********************************************************************//**
 *	QsEmpFactory.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/12/06.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_QUEST_QSEMP_QS_EMP_FACTORY_H_
#define _SRCR_QUEST_QSEMP_QS_EMP_FACTORY_H_

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
class QsEmpFactory
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static EventMatter* newEventMatter(const CStringBase* nameMatter);

private:
	static EventMatter* newQuestEventMatter(const VcString* name);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_QUEST_QSEMP_QS_EMP_FACTORY_H_
