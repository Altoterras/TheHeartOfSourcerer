/***********************************************************************//**
 *  HrAnimObjCtxArray.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/12/29.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "HrAnimObjCtxArray.h"

// Friends
#include "HrAnimObj.h"
#include "HrAnimObjCtx.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// HrAnimObjCtxArray メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
HrAnimObjCtxArray::HrAnimObjCtxArray()
	: _aocarrObjCtx(0L)
	, _numObjCtx(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
HrAnimObjCtxArray::~HrAnimObjCtxArray()
{
	ASSERT(_aocarrObjCtx == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool HrAnimObjCtxArray::create(HrAnimObj** aobjRoot, u16 numRootAobj, u16 numAobjTotal, u16 numShape, Shape** sarr, ShapeCtx** scarr)
{
	_numObjCtx = numAobjTotal;
	_aocarrObjCtx = new HrAnimObjCtx*[_numObjCtx];
	for(int i = 0; i < numAobjTotal; i++)
	{
		_aocarrObjCtx[i] = new HrAnimObjCtx();
	}
	for(int i = 0; i < numRootAobj; i++)
	{
		if(!aobjRoot[i]->createContext(this, sarr, scarr, numShape))
		{
			destroy(aobjRoot, numRootAobj);
			return false;
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void HrAnimObjCtxArray::destroy(HrAnimObj** aobjRoot, u16 numRootAobj)
{
	for(int i = 0; i < numRootAobj; i++)
	{
		aobjRoot[i]->destroyContext(this);
	}
	for(int i = 0; i < _numObjCtx; i++)
	{
		delete _aocarrObjCtx[i];
	}
	delete[] _aocarrObjCtx;
	_aocarrObjCtx = 0L;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS