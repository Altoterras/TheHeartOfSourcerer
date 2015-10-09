/***********************************************************************//**
 *	QsEmp_ConfirmContract.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/11/29.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_QSEMP_IMPLEMENT_QS_EMP_CONFIRM_CONTRACT_H_
#define _SRCR_EPISODE_QSEMP_IMPLEMENT_QS_EMP_CONFIRM_CONTRACT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../QsEmp.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS
	
class SimpleSprite;

/*---------------------------------------------------------------------*//**
 *	クエスト受託確認演出
 *
**//*---------------------------------------------------------------------*/
class QsEmp_ConfirmContract : public QsEmp
{
	//======================================================================
	// 定数
private:
	enum Step
	{
		STEP_NULL,
		STEP_WAIT_BEGIN,
		STEP_WAIT_YESNO,
		STEP_WAIT_EFFECT,
		STEP_WAIT_FOUT,
		STEP_WAIT_FIN,
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void begin(u32 param);
	void end();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	QsEmp_ConfirmContract();
	~QsEmp_ConfirmContract();
	void execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun);
	void draw(const RenderCtx* rc);

private:
	void contractQuest();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 変数
private:
	SimpleSprite* _sprite;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_QSEMP_IMPLEMENT_QS_EMP_CONFIRM_CONTRACT_H_
