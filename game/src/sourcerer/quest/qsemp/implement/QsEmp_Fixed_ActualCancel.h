/***********************************************************************//**
 *	QsEmp_Fixed_ActualCancel.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/12/03.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_QSEMP_IMPLEMENT_QS_EMP_FIXED_ACTUAL_CANCEL_H_
#define _SRCR_EPISODE_QSEMP_IMPLEMENT_QS_EMP_FIXED_ACTUAL_CANCEL_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../QsEmp.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class SimpleSprite;

/*---------------------------------------------------------------------*//**
 *	アサ宅での休息イベント
 *
**//*---------------------------------------------------------------------*/
class QsEmp_Fixed_ActualCancel : public QsEmp
{
	//======================================================================
	// 定数
private:
	enum Step
	{
		STEP_NULL,
		STEP_WAIT_BEGIN,
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
	QsEmp_Fixed_ActualCancel();
	~QsEmp_Fixed_ActualCancel();
	void execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun);
	void draw(const RenderCtx* rc);

private:
	void cancelQuest();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 変数
private:
	SimpleSprite* _sprite;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_QSEMP_IMPLEMENT_QS_EMP_FIXED_ACTUAL_CANCEL_H_
