/***********************************************************************//**
 *  MagicClusterFactory.cpp
 *  Enlight Game Application
 *
 *  Created by Ryoutarou Kishi on 2009/12/18.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MagicClusterFactory.h"

// Friends
#include "MagicCluster.h"
#include "MagicClusterCreateParam.h"
#include "implement/CreationMc.h"
#include "implement/EmittedMc.h"
#include "implement/ExplosionMc.h"
#include "implement/ServedMc.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

/*---------------------------------------------------------------------*//**
	クラスタ作成
**//*---------------------------------------------------------------------*/
MagicCluster* MagicClusterFactory::makeCluster(MagicSys* mgcsysRef, u16 mcid, const MagicClusterCreateParam* mccprm)
{
	MagicCluster* mc = 0L;

	switch(mccprm->_mctype)
	{
	case MagicClusterType::CREATION:	mc = new CreationMc();	break;
	case MagicClusterType::EMITTED:		mc = new EmittedMc();	break;
	case MagicClusterType::EXPLOSION:	mc = new ExplosionMc();	break;
	case MagicClusterType::SERVED:		mc = new ServedMc();	break;
	}

	if(mc == 0L)
	{
		ASSERTM(false, "invlid MagicCluster create-param.");
		return 0L;
	}

	if(!mc->create(mgcsysRef, mcid, mccprm))
	{
		delete mc;
		ASSERTM(false, "MagicCluster::create failed.");
		return 0L;
	}

	return mc;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
