/***********************************************************************//**
 *	StEmp_10204_1002_ClearVol01Lite.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/09/09.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_STEMP_IMPLEMENT_ST_EMP_10204_1002_CLEAR_VOL01_LITE_H_
#define _SRCR_EPISODE_STEMP_IMPLEMENT_ST_EMP_10204_1002_CLEAR_VOL01_LITE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../StEmp.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class SaveLoadWindow;

/*---------------------------------------------------------------------*//**
 *	Vol01 ゲームクリア処理またー
 *
**//*---------------------------------------------------------------------*/
class StEmp_10204_1002_ClearVol01Lite : public StEmp
{
	//======================================================================
	// 定数
private:
	enum Step
	{
		STEP_NULL,
		STEP_SAVE_FIRST,
		STEP_DO_SAVE,
		STEP_END
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
	StEmp_10204_1002_ClearVol01Lite();
	~StEmp_10204_1002_ClearVol01Lite();
	void execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun);

	//======================================================================
	// 変数
protected:
	SaveLoadWindow* _wndSave;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_STEMP_IMPLEMENT_ST_EMP_10204_1002_CLEAR_VOL01_LITE_H_
