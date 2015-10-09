/***********************************************************************//**
 *	ModelSwitch.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/06/23.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ModelSwitch.h"

// Friends

// External
#include "../../tfw/g3d/sg/node/implement/Model.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ModelSwitch メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	モデルを取得する
**//*---------------------------------------------------------------------*/
Model* ModelSwitch::getModel(s32 idx)
{
	ASSERT((0 <= idx) && (idx < NUM_MODEL));
	return _mdlarrRef[idx];
}

/*---------------------------------------------------------------------*//**
	モデルを取得する
		関連付けられたモデルの１つは NULL であることがあるので、
		NULL でない方を返答する
**//*---------------------------------------------------------------------*/
Model* ModelSwitch::getAnyModel()
{
	for(int i = 0; i < NUM_MODEL; i++)
	{
		if(_mdlarrRef[i] != 0L)
		{
			return _mdlarrRef[i];
		}
	}

	return 0L;
}

/*---------------------------------------------------------------------*//**
	モデルの表示状態を切り替える
**//*---------------------------------------------------------------------*/
void ModelSwitch::switchModel(s32 idxShowModel)
{
	for(int i = 0; i < NUM_MODEL; i++)
	{
		if(_mdlarrRef[i] != 0L)
		{
			bool isShow = (i == idxShowModel);
			if(_mdlarrRef[i]->isShow() != isShow)
			{
				_mdlarrRef[i]->setShow(isShow);
			}
		}
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ModelSwitch::ModelSwitch() :
	_mdlarrRef(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
ModelSwitch::~ModelSwitch()
{
	ASSERT(_mdlarrRef == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool ModelSwitch::create(Model* mdl1, Model* mdl2)
{
	_mdlarrRef = new Model*[NUM_MODEL];
	_mdlarrRef[0] = mdl1;
	_mdlarrRef[1] = mdl2;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void ModelSwitch::destroy()
{
	delete[] _mdlarrRef;
	_mdlarrRef = 0L;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS