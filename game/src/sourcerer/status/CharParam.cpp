/***********************************************************************//**
 *	CharParam.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/21.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "CharParam.h"

// Friends

// External
#include "../../tfw/serialize/Serializer.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// CharParam メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	レベルアップ処理
**//*---------------------------------------------------------------------*/
void CharParam::levelup(const CharParam* cprmNext)
{
	ASSERT(_experience >= cprmNext->_experience);

	_level = cprmNext->getLevel();

	if(_energy < cprmNext->_energy)
	{
		_energy = cprmNext->_energy;
	}
	if(_unity < cprmNext->_unity)
	{
		_unity = cprmNext->_unity;
	}
	if(_activity < cprmNext->_activity)
	{
		_activity = cprmNext->_activity;
	}
	if(_stability < cprmNext->_stability)
	{
		_stability = cprmNext->_stability;
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	シリアライズ
**//*---------------------------------------------------------------------*/
bool CharParam::serialize(Serializer* ser) const
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		ASSERT(false);
		return false;
	}
	else						// ダイレクトモード
	{
		ser->store(&_experience);
		ser->store(&_unity);
		ser->store(&_activity);
		ser->store(&_stability);
		ser->store(&_energy);
		return true;
	}
}

/*---------------------------------------------------------------------*//**
	デシリアイズ
**//*---------------------------------------------------------------------*/
bool CharParam::deserialize(Serializer* ser)
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		ASSERT(false);
		return false;
	}
	else						// ダイレクトモード
	{
		ser->restore(&_experience);
		ser->restore(&_unity);
		ser->restore(&_activity);
		ser->restore(&_stability);
		ser->restore(&_energy);
		return true;
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
