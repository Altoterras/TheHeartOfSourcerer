/***********************************************************************//**
 *	StEmp_10101_1001_MagicTutorial01.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/22.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_STEMP_IMPLEMENT_ST_EMP_10101_1001_MAGIC_TUTORIAL01_H_
#define _SRCR_EPISODE_STEMP_IMPLEMENT_ST_EMP_10101_1001_MAGIC_TUTORIAL01_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../StEmp.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

SRCR_BEGIN_NS

class SimpleSprite;
class UnitArray;

/*---------------------------------------------------------------------*//**
 *	St10101 ソウマチュートリアル 1
 *
**//*---------------------------------------------------------------------*/
class StEmp_10101_1001_MagicTutorial01 : public StEmp
{
	//======================================================================
	// 定数
private:
	enum Step
	{
		STEP_NULL,
		STEP_PLEASE_TAP_SOUMAICON,
		STEP_SETUP_GUI_SOUMAICON,
		STEP_WAIT_TAP_SOUMAICON,
		STEP_PLEASE_TAP_SOURCEBOOK,
		STEP_WAIT_TAP_SOURCEBOOK,
		STEP_PLEASE_TAP_SOUMA_1,
		STEP_WAIT_TAP_SOUMA_1,
		STEP_PLEASE_TAP_RETURN,
		STEP_WAIT_TAP_RETURN,
		STEP_DO_SOUMA_1,
		STEP_END
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void begin(u32 param);
	void end();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	StEmp_10101_1001_MagicTutorial01();
	~StEmp_10101_1001_MagicTutorial01();
	void execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun);
	void draw(const RenderCtx* rc);

	void notifyDisappearMagicCluster(MagicCluster* mc, u32 dcflags);

private:
	void readTexture(u32 fileid);

	//======================================================================
	// 変数
protected:
	SimpleSprite* _sprite;
	UnitArray* _uarrEnableFocusBk;
	f32 _frameElapsedSouma;
	f32 _frameElapsedStep;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_STEMP_IMPLEMENT_ST_EMP_10101_1001_MAGIC_TUTORIAL01_H_
