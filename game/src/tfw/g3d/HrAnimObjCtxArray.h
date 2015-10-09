/***********************************************************************//**
 *  HrAnimObjCtxArray.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/12/29.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_G3D_HR_ANIM_OBJ_CTX_ARRAY_H_
#define _TFW_G3D_HR_ANIM_OBJ_CTX_ARRAY_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class HrAnimObj;
class HrAnimObjCtx;
class Shape;
class ShapeCtx;

/*---------------------------------------------------------------------*//**
 *	階層アニメオブジェクト可変値コンテキスト配列
 *
**//*---------------------------------------------------------------------*/
class HrAnimObjCtxArray
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	HrAnimObjCtxArray();
	~HrAnimObjCtxArray();
	bool create(HrAnimObj** aobjRoot, u16 numRootAobj, u16 numAobjTotal, u16 numShape, Shape** sarr, ShapeCtx** scarr);
	void destroy(HrAnimObj** aobjRoot, u16 numRootAobj);

	//======================================================================
	// メンバ変数
public:
	HrAnimObjCtx**		_aocarrObjCtx;
	u16					_numObjCtx;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_G3D_HR_ANIM_OBJ_CTX_ARRAY_H_