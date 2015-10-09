/***********************************************************************//**
 *	Spid.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/16.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Spid.h"

// Friends
#include "../../unit/Unit.h"

// External
#include "../../../etk/sg/SorSgm.h"
#include "../../../tfw/collection/List.h"
#include "../../../tfw/g3d/HrAnimObj.h"
#include "../../../tfw/g3d/HrAnimObjCtx.h"
#include "../../../tfw/g3d/HrAnimObjCtxArray.h"
#include "../../../tfw/g3d/HrAnimSet.h"
#include "../../../tfw/g3d/HrAnimSetCtx.h"
#include "../../../tfw/g3d/Shape.h"
#include "../../../tfw/g3d/sg/node/SgNodeChain.h"
#include "../../../tfw/g3d/sg/node/implement/ShapeModel.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Spid メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	シーングラフから階層アニメオブジェコンテキストを検索する
**//*---------------------------------------------------------------------*/
bool Spid::find(HrAnimObjCtx** haobjctx, u16 spid, SorSgm* sgm)
{
	SgNode* sgnFound = 0L;
	Shape* shapeFound = 0L;
	if(!findDive(&sgnFound, &shapeFound, spid, sgm->getRootNode()))
	{
		return false;
	}

	switch(sgnFound->getType())
	{
	case SGNTYPE_MODEL:
		{
			Model* mdl = (Model*)sgnFound;
			if(mdl->getModelClass() == Model::MODELCLASS_SHAPE)
			{
				if(findHrAnimObjCtxFromModel(haobjctx, shapeFound, (ShapeModel*)mdl))
				{
					return true;
				}
			}
		}
		break;
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	ユニットから階層アニメオブジェコンテキストを検索する
**//*---------------------------------------------------------------------*/
bool Spid::find(HrAnimObjCtx** haobjctx, u16 spid, Unit* unit)
{
	for(int i = 0; i < unit->getModelNum(); i++)
	{
		if(find(haobjctx, spid, unit->getModel(i)))
		{
			return true;
		}
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	モデルから階層アニメオブジェコンテキストを検索する
**//*---------------------------------------------------------------------*/
bool Spid::find(HrAnimObjCtx** haobjctx, u16 spid, Model* mdl)
{
	Shape* shapeFound = 0L;
	if(findShape(&shapeFound, spid, mdl))
	{
		if(mdl->getModelClass() == Model::MODELCLASS_SHAPE)
		{
			if(findHrAnimObjCtxFromModel(haobjctx, shapeFound, (ShapeModel*)mdl))
			{
				return true;
			}
		}
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	シーングラフから該当モデルを検索する
**//*---------------------------------------------------------------------*/
bool Spid::find(Model** mdl, u16 spid, SorSgm* sgm)
{
	SgNode* sgnFound = 0L;
	Shape* shapeFound = 0L;
	if(!findDive(&sgnFound, &shapeFound, spid, sgm->getRootNode()))
	{
		return false;
	}

	switch(sgnFound->getType())
	{
	case SGNTYPE_MODEL:
		{
			*mdl = (Model*)sgnFound;
			return true;
		}
		break;
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	モデルから SPID 付与階層アニメオブジェコンテキストを列挙する
**//*---------------------------------------------------------------------*/
void Spid::enumerateHrAnimObjCtx(List<EnumRes*>* listRes, Model* mdl)
{
	if(mdl->getModelClass() == Model::MODELCLASS_SHAPE)
	{
		ShapeModel* smdl = (ShapeModel*)mdl;
		for(int i = 0; i < smdl->getShapeNum(); i++)
		{
			Shape* shape = smdl->getShape(i);
			if(shape->getSpid() != 0)
			{
				// シェイプ発見
				EnumRes* eres = new EnumRes();
				eres->_spid = shape->getSpid();
				eres->_shape = shape;
				findHrAnimObjCtxFromModel(&eres->_haobjctx, shape, smdl);
				listRes->addTail(eres);
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	モデルからルートの階層アニメオブジェコンテキストを得る
**//*---------------------------------------------------------------------*/
bool Spid::findRoot(HrAnimObjCtx** haobjctx, Model* mdl)
{
	if(mdl->getModelClass() == Model::MODELCLASS_SHAPE)
	{
		ShapeModel* smdl = (ShapeModel*)mdl;

		// HrAnimObj を特定する
		HrAnimObj* haoFound = 0L;
		HrAnimSet* has = smdl->getAnimSet();
		if(has != 0L)
		{
			if(has->getObjRootNum() > 0)
			{
				haoFound = has->getObjRoot(0);
			}
		}
		if(haoFound == 0L)	// 見つからなかった
		{
			return false;
		}

		// HrAnimObjCtx を特定する
		HrAnimSetCtx* hasc = smdl->getAnimSetContext();
		if((hasc != 0L) && (hasc->_aocaObjCtxArray != 0L))
		{
			HrAnimObjCtx* haoc = hasc->_aocaObjCtxArray->_aocarrObjCtx[ haoFound->getIndex() ];
			*haobjctx = haoc;
			return true;
		}
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	シーングラフノードを潜って、検索する
**//*---------------------------------------------------------------------*/
bool Spid::findDive(SgNode** sgnFound, Shape** shapeFound, u16 spid, SgNode* sgnCur)
{
	switch(sgnCur->getType())
	{
	case SGNTYPE_CHAIN:
		{
			SgNodeChain* sgc = (SgNodeChain*)sgnCur;
			ListIterator<SgNode*> it;
			sgc->getIterator(&it);
			while(it.has())
			{
				if(findDive(sgnFound, shapeFound, spid, it.object()))
				{
					return true;
				}
				it.next();
			}
		}
		break;
	case SGNTYPE_MODEL:
		if(findShape(shapeFound, spid, (Model*)sgnCur))
		{
			*sgnFound = sgnCur;
			return true;
		}
		break;
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	モデルからシェイプを検索する
**//*---------------------------------------------------------------------*/
bool Spid::findShape(Shape** shapeFound, u16 spid, Model* mdl)
{
	if(mdl->getModelClass() == Model::MODELCLASS_SHAPE)
	{
		ShapeModel* smdl = (ShapeModel*)mdl;
		for(int i = 0; i < smdl->getShapeNum(); i++)
		{
			Shape* shape = smdl->getShape(i);
			if(shape->getSpid() == spid)
			{
				*shapeFound = shape;
				return true;
			}
		}
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	SPID 該当シェイプとモデルから HrAnimObjCtx を検索する
**//*---------------------------------------------------------------------*/
bool Spid::findHrAnimObjCtxFromModel(HrAnimObjCtx** haobjctx, Shape* shapeSpid, ShapeModel* smdl)
{
	// HrAnimObj を特定する
	HrAnimObj* haoFound = 0L;
	HrAnimSet* has = smdl->getAnimSet();
	if(has != 0L)
	{
		for(int i = 0; i < (int)has->getObjRootNum(); i++)
		{
			HrAnimObj* hao = has->getObjRoot(i);
			haoFound = HrAnimObj::find(hao, shapeSpid);
			if(haoFound != 0L)
			{
				break;
			}
		}
	}
	if(haoFound == 0L)	// 見つからなかった
	{
		return false;
	}

	// HrAnimObjCtx を特定する
	HrAnimSetCtx* hasc = smdl->getAnimSetContext();
	if((hasc != 0L) && (hasc->_aocaObjCtxArray != 0L))
	{
		HrAnimObjCtx* haoc = hasc->_aocaObjCtxArray->_aocarrObjCtx[ haoFound->getIndex() ];
		*haobjctx = haoc;
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
