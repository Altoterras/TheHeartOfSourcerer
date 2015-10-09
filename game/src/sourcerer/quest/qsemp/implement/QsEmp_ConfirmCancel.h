/***********************************************************************//**
 *	QsEmp_ConfirmCancel.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/12/03.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_QSEMP_IMPLEMENT_QS_EMP_CONFIRM_CANCEL_H_
#define _SRCR_EPISODE_QSEMP_IMPLEMENT_QS_EMP_CONFIRM_CANCEL_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../QsEmp.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	クエスト中断確認演出
 *
**//*---------------------------------------------------------------------*/
class QsEmp_ConfirmCancel : public QsEmp
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
	QsEmp_ConfirmCancel();
	void execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun);

private:
	void reserveCancelQuest();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 変数
private:
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_QSEMP_IMPLEMENT_QS_EMP_CONFIRM_CANCEL_H_
