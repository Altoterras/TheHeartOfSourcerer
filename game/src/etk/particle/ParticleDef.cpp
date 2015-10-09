/***********************************************************************//**
 *	ParticleDef.cpp
 *	Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2010/05/24.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ParticleDef.h"

// Friends
#include "../../tfw/file/FileBase.h"

// External

ETK_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ParticleDef メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ParticleDef::ParticleDef()
	: _ptcldid(0)
	, _modelid(0)
	, _frameLife(0)
	, _xInitVel(0)
	, _yInitVel(0)
	, _zInitVel(0)
	, _param1(0)
	, _param2(0)
	, _param3(0)
	, _alphafunc(0)
	, _colflags(0)
{
}

/*---------------------------------------------------------------------*//**
	データファイルからデータ部を読み込む
**//*---------------------------------------------------------------------*/
bool ParticleDef::loadFromDataFile(FileBase* file)
{
	u8 wk8;
	u16 wk16;

	// 定義 ID
	if(file->read(&wk16, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_ptcldid = wk16;

	// モデル ID
	if(file->read(&wk16, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_modelid = wk16;

	// 生存フレーム
	if(file->read(&wk16, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_frameLife = wk16;

	// 初速度 X
	if(file->read(&wk16, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_xInitVel = wk16;

	// 初速度 Y
	if(file->read(&wk16, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_yInitVel = wk16;

	// 初速度 Z
	if(file->read(&wk16, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_zInitVel = wk16;

	// 汎用パラメータ１
	if(file->read(&wk16, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_param1 = wk16;

	// 汎用パラメータ２
	if(file->read(&wk16, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_param2 = wk16;

	// 汎用パラメータ３
	if(file->read(&wk16, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_param3 = wk16;

	// 半透明合成式
	if(file->read(&wk8, sizeof(u8)) != sizeof(u8))		{	return false;	}
	_alphafunc = wk8 << 4;

	// 色フラグ
	if(file->read(&wk8, sizeof(u8)) != sizeof(u8))		{	return false;	}
	_colflags = wk8;

	// 色 1 - 赤
	if(file->read(&wk8, sizeof(u8)) != sizeof(u8))		{	return false;	}
	_col1._v[0] = wk8;

	// 色 1 - 緑
	if(file->read(&wk8, sizeof(u8)) != sizeof(u8))		{	return false;	}
	_col1._v[1] = wk8;

	// 色 1 - 青
	if(file->read(&wk8, sizeof(u8)) != sizeof(u8))		{	return false;	}
	_col1._v[2] = wk8;

	// 色 1 - α
	if(file->read(&wk8, sizeof(u8)) != sizeof(u8))		{	return false;	}
	_col1._v[3] = wk8;

	// 色 2 - 赤
	if(file->read(&wk8, sizeof(u8)) != sizeof(u8))		{	return false;	}
	_col2._v[0] = wk8;

	// 色 2 - 緑
	if(file->read(&wk8, sizeof(u8)) != sizeof(u8))		{	return false;	}
	_col2._v[1] = wk8;

	// 色 2 - 青
	if(file->read(&wk8, sizeof(u8)) != sizeof(u8))		{	return false;	}
	_col2._v[2] = wk8;

	// 色 2 - α
	if(file->read(&wk8, sizeof(u8)) != sizeof(u8))		{	return false;	}
	_col2._v[3] = wk8;

	return true;
}

/*---------------------------------------------------------------------*//**
	解放
**//*---------------------------------------------------------------------*/
void ParticleDef::cleanup()
{
}

////////////////////////////////////////////////////////////////////////////

ETK_END_NS
