/***********************************************************************//**
 *	QsEmp_ConfirmHandOver.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/12/10.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_QSEMP_IMPLEMENT_QS_EMP_CONFIRM_HAND_OVER_H_
#define _SRCR_EPISODE_QSEMP_IMPLEMENT_QS_EMP_CONFIRM_HAND_OVER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../QsEmp.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	クエストのクリア条件（アイテム等）を依頼者に渡す
 *
**//*---------------------------------------------------------------------*/
class QsEmp_ConfirmHandOver : public QsEmp
{
	//======================================================================
	// 定数
public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void begin(u32 param);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	QsEmp_ConfirmHandOver();
	void execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun);

private:
	bool handoverThing();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 変数
private:
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_QSEMP_IMPLEMENT_QS_EMP_CONFIRM_HAND_OVER_H_
