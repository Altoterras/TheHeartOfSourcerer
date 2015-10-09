/***********************************************************************//**
 *  List.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2011/04/09.
 *  Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_COLLECTION_LIST_H_
#define _TFW_COLLECTION_LIST_H_

////////////////////////////////////////////////////////////////////////////
// クラス

/*---------------------------------------------------------------------*//**
 *	List - delete 対応ポインタ リンク ドリスト
 *		delete のみ管理する
 *		要素追加・設定時にポインタのコピーのみが生じる
 *		値型やスタック変数を加えることはできない
 *		（外部で new したポインタのみを設定できる）
 *
**//*---------------------------------------------------------------------*/
#define TFW_PTR_LIST
#define TFW_LIST_CLASS_NAME			List
#define TFW_LISTNODE_CLASS_NAME		ListNode
#define TFW_LISTIT_CLASS_NAME		ListIterator
#define TFW_LIST_REF_TYPE			TYPE
#define TFW_LIST_SET_TYPE			TYPE

#include "ListBase.h"

#undef TFW_LIST_CLASS_NAME
#undef TFW_LISTNODE_CLASS_NAME
#undef TFW_LISTIT_CLASS_NAME
#undef TFW_LIST_REF_TYPE
#undef TFW_LIST_SET_TYPE
#undef TFW_PTR_LIST

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_COLLECTION_LIST_H_