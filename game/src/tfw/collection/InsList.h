/***********************************************************************//**
 *  InsList.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2011/04/09.
 *  Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_COLLECTION_INS_LIST_H_
#define _TFW_COLLECTION_INS_LIST_H_

////////////////////////////////////////////////////////////////////////////
// クラス

/*---------------------------------------------------------------------*//**
 *	InsList - インスタンス リンクド リスト
 *		非参照型の値型を格納できる配列
 *		削除時 delete はしない
 *
**//*---------------------------------------------------------------------*/
#define TFW_INS_LIST
#define TFW_LIST_CLASS_NAME			InsList
#define TFW_LISTNODE_CLASS_NAME		InsListNode
#define TFW_LISTIT_CLASS_NAME		InsListIterator
#define TFW_LIST_REF_TYPE			TYPE*
#define TFW_LIST_SET_TYPE			const TYPE*

#include "ListBase.h"

#undef TFW_LIST_SET_TYPE
#undef TFW_LIST_REF_TYPE
#undef TFW_LISTIT_CLASS_NAME
#undef TFW_LISTNODE_CLASS_NAME
#undef TFW_LIST_CLASS_NAME
#undef TFW_INS_LIST

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_COLLECTION_INS_LIST_H_