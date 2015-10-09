/***********************************************************************//**
 *	QsEmp_CallFinish.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/12/03.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_QUEST_QSEMP_IMPLEMENT_QS_EMP_CONFIRM_FINISH_H_
#define _SRCR_QUEST_QSEMP_IMPLEMENT_QS_EMP_CONFIRM_FINISH_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../QsEmp.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class SimpleSprite;

/*---------------------------------------------------------------------*//**
 *	クエスト終了演出
 *
**//*---------------------------------------------------------------------*/
class QsEmp_CallFinish : public QsEmp
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void begin(u32 param);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	QsEmp_CallFinish();

private:
	void reserveFinishQuest();
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_QUEST_QSEMP_IMPLEMENT_QS_EMP_CONFIRM_FINISH_H_
