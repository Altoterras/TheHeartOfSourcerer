/***********************************************************************//**
 *  MagicClusterFactory.h
 *  Enlight Game Application
 *
 *  Created by Ryoutarou Kishi on 2009/12/18.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MAGIC_MAGIC_CLUSTER_FACTORY_H_
#define _SRCR_MAGIC_MAGIC_CLUSTER_FACTORY_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "MagicClusterCreateParam.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class MagicCluster;
class MagicSys;

/*---------------------------------------------------------------------*//**
 *	クラスター ファクトリ
 *	
**//*---------------------------------------------------------------------*/
class MagicClusterFactory
{
	//======================================================================
	// メソッド
public:
	static MagicCluster* makeCluster(MagicSys* mgcsysRef, u16 mcid, const MagicClusterCreateParam* mccprm);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MAGIC_MAGIC_CLUSTER_FACTORY_H_
