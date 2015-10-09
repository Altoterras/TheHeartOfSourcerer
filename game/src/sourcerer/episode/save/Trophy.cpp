/***********************************************************************//**
 *	Trophy.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/08/20.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Trophy.h"

// Friends
#include "SaveStructure.h"

// External
#include "../../../tfw/serialize/Serializer.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Trophy メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Trophy::Trophy(PsnsBase* psnsRef)
	: TrophyBase(psnsRef)
{
	ASSERT(NUM_TI <= NUM_TROPHY_MAX);
}

/*---------------------------------------------------------------------*//**
	シリアライズ
**//*---------------------------------------------------------------------*/
bool Trophy::serialize(Serializer* ser) const
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		ASSERT(sizeof(TrophySaveStructure) == TrophySaveStructure::SIZE_THIS);

		SaveStructure* svst = (SaveStructure*)ser->getStructure();
		TFW_COPY_MEMORY(svst->_trophysvst._tprate, _tprate, sizeof(u8) * NUM_TROPHY_MAX);
		TFW_COPY_MEMORY(svst->_trophysvst._tpflags, _tpflags, sizeof(u8) * NUM_TROPHY_MAX);
	}
	else						// ダイレクトモード
	{
		ser->store(_tprate, sizeof(u16) * NUM_TROPHY_MAX);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	デシリアライズ
**//*---------------------------------------------------------------------*/
bool Trophy::deserialize(Serializer* ser)
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		ASSERT(sizeof(TrophySaveStructure) == TrophySaveStructure::SIZE_THIS);

		const SaveStructure* svst = (SaveStructure*)ser->getStructure();
		TFW_COPY_MEMORY(_tprate, svst->_trophysvst._tprate, sizeof(u8) * NUM_TROPHY_MAX);
		TFW_COPY_MEMORY(_tpflags, svst->_trophysvst._tpflags, sizeof(u8) * NUM_TROPHY_MAX);
	}
	else						// ダイレクトモード
	{
		ser->restore(_tprate, sizeof(u16) * NUM_TROPHY_MAX);
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
