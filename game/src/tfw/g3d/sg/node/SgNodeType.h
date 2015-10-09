/***********************************************************************//**
 *  SgNodeType.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/04/20.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_G3D_SG_NODE_SG_NODE_TYPE_H_
#define _TFW_G3D_SG_NODE_SG_NODE_TYPE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../Base.h"

////////////////////////////////////////////////////////////////////////////
// 定数

TFW_BEGIN_NS

static const u16 SGNTYPE_NULL		= 0;
static const u16 SGNTYPE_CHAIN		= 1;
static const u16 SGNTYPE_CAMERA		= 11;
static const u16 SGNTYPE_MODEL		= 12;
static const u16 SGNTYPE_CUSTOM		= 10000;

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_G3D_SG_NODE_SG_NODE_TYPE_H_

