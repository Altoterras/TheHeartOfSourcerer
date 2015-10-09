/***********************************************************************//**
 *	IdManager.cpp
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2009/07/22.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "IdManager.h"

// Friends

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// IdManager メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	新しい ID を取得
**//*---------------------------------------------------------------------*/
u16 IdManager::getNewId()
{
	for(u16 idWk = 1; idWk < getIdNum(); idWk++)	// 0 は無効な ID
	{
		if(!getFlag(idWk))
		{
			setFlag(idWk, true);

			#if 0
				TRACE("{IdManager::getNewId} TEST >>> \n");
				for(u16 ii = 0; ii < 100; ii++)
				{
					TRACE(VcString::format("  [%d] %d\n", ii, isUsedId(ii)));
				}
				TRACE("{IdManager::getNewId} <<< \n");
			#endif

			return idWk;
		}
	}
	ASSERT(false);			// 足りなくなるのは想定外，もしくはリーク（削除忘れ）
	return 0;	// 無効な ID
}

/*---------------------------------------------------------------------*//**
	ID 解放
**//*---------------------------------------------------------------------*/
void IdManager::releaseId(u16 id)
{
	if(id == 0)
	{
		// ID 0 （無効な ID）の解放は無害とする
		ASSERT(!isUsedId(0));	// 使用されていないはず
		return;
	}
	if(getIdNum() <= id)
	{
		ASSERT(false);
		return;
	}

	ASSERT(isUsedId(id));	// 使用済みなはず
	setFlag(id, false);		// フラグを OFF に

	#if 0
		TRACE("<IdManager::releaseId> TEST >>> \n");
		for(u16 ii = 0; ii < 100; ii++)
		{
			TRACE(VcString::format("  [%d] %d\n", ii, isUsedId(ii)));
		}
		TRACE("<IdManager::getNewId> <<< \n");
	#endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

////////////////////////////////////////////////////////////////////////////

TFW_END_NS

