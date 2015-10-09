/***********************************************************************//**
 *  Array.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2011/04/08.
 *  Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_COLLECTION_ARRAY_H_
#define _TFW_COLLECTION_ARRAY_H_

////////////////////////////////////////////////////////////////////////////
// クラス

/*---------------------------------------------------------------------*//**
 *	Array - delete 対応ポインタ配列クラス
 *		delete のみ管理する
 *		要素追加・設定時にポインタのコピーのみが生じる
 *		値型やスタック変数を加えることはできない
 *		（外部で new したポインタのみを設定できる）
 *
**//*---------------------------------------------------------------------*/
#define TFW_PTR_ARRAY
#define TFW_ARRAY_CLASS_NAME		Array

#include "ArrayBase.h"

#undef TFW_ARRAY_CLASS_NAME
#undef TFW_PTR_ARRAY

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_COLLECTION_ARRAY_H_