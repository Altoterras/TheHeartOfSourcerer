/***********************************************************************//**
 *  MagicClusterType.h
 *  Enlight Game Application
 *
 *  Created by Ryoutarou Kishi on 2009/12/18.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MAGIC_MAGIC_CLUSTER_TYPE_H_
#define _SRCR_MAGIC_MAGIC_CLUSTER_TYPE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	クラスター タイプ
 *	
**//*---------------------------------------------------------------------*/
class MagicClusterType
{
	//======================================================================
	// 定数
public:
	enum McType
	{
		_NULL_,
		CREATION,
		EMITTED,
		EXPLOSION,
		SERVED,
	};
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MAGIC_MAGIC_CLUSTER_TYPE_H_
