/***********************************************************************//**
 *	ModelSwitch.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/06/23.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MAP_MODEL_SWITCH_H_
#define _SRCR_MAP_MODEL_SWITCH_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Model;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	モデルスイッチ クラス
 *
**//*---------------------------------------------------------------------*/
class ModelSwitch
{
	//======================================================================
	// 定数
public:
	static const int	NUM_MODEL	= 2;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス

public:
	Model*	getModel(s32 idx);
	Model*	getAnyModel();
	void	switchModel(s32 idxShowModel);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御

public:
	ModelSwitch();
	virtual ~ModelSwitch();

	bool create(Model* mdl1, Model* mdl2);
	void destroy();

	//======================================================================
	// メンバ変数

private:
	Model**		_mdlarrRef;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MAP_MODEL_SWITCH_H_
