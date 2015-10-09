/***********************************************************************//**
 *  InsArray.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/05/20.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_COLLECTION_INS_ARRAY_H_
#define _TFW_COLLECTION_INS_ARRAY_H_

////////////////////////////////////////////////////////////////////////////
// クラス

/*---------------------------------------------------------------------*//**
 *	InsArray - 値型配列
 *		非参照型の値型を格納できる配列
 *		削除時 delete はしない
 *
**//*---------------------------------------------------------------------*/
#define TFW_INS_ARRAY
#define TFW_ARRAY_CLASS_NAME		InsArray

#include "ArrayBase.h"

#undef TFW_ARRAY_CLASS_NAME
#undef TFW_INS_ARRAY

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_COLLECTION_INS_ARRAY_H_