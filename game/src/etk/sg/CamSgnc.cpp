/***********************************************************************//**
 *  CamSgnc.cpp
 *	Envelopment Terras Kit
 *
 *  Created by Ryoutarou Kishi on 2009/07/17.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "CamSgnc.h"

// Friends
#include "../../tfw/g3d/sg/node/implement/Camera.h"
#include "../../tfw/collection/List.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

ETK_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// CamSgnc アクセサ

//==========================================================================
// CamSgnc メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
CamSgnc::CamSgnc()
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
CamSgnc::~CamSgnc()
{
}

/*---------------------------------------------------------------------*//**
	カメラの有効化スイッチ
**//*---------------------------------------------------------------------*/
bool CamSgnc::switchVisible(u16 nodeid)
{
	bool isSuccess = false;
	for(int icam = 0; icam < getChildNodeNum(); icam++)
	{
		Camera* cam = (Camera*)getChildNode(icam);
		if(cam->getId() == nodeid)
		{
			if(!cam->isVisible())
			{
				cam->setVisible(true);
			}
			isSuccess = true;
		}
		else
		{
			if(cam->isVisible())
			{
				cam->setVisible(false);
			}
		}
	}
	return isSuccess;
}

/*---------------------------------------------------------------------*//**
	子ノードを削除
**//*---------------------------------------------------------------------*/
bool CamSgnc::removeChildNode(SgNode* sgnode)
{
	if(!SgNodeChain::removeChildNode(sgnode))
	{
		return false;
	}

	// 有効化されたノードが削除された場合は最初に見つけたカメラを有効化
	if((sgnode->getType() == SGNTYPE_CAMERA) && (((Camera*)sgnode)->isVisible()))
	{
		((Camera*)sgnode)->setVisible(false);
		visibleLeftoverCamera();
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	子ノードを ID で削除
**//*---------------------------------------------------------------------*/
bool CamSgnc::removeChildNode(u16 sgnodeid)
{
	// 前もってポインタを得て、有効化されたカメラかを調べておく
	SgNode* sgnode = getChildNodeFromId(sgnodeid);
	if(sgnode == 0L)
	{
		return false;
	}
	bool isDeleteVisibleCamera = (sgnode->getType() == SGNTYPE_CAMERA) && (((Camera*)sgnode)->isVisible());

	// シーングラフから削除する
	if(!SgNodeChain::removeChildNode(sgnodeid))
	{
		return false;
	}

	// 有効化されたノードが削除された場合は最初に見つけたカメラを有効化
	if(isDeleteVisibleCamera)
	{
		((Camera*)sgnode)->setVisible(false);
		visibleLeftoverCamera();
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	残りのカメラに変更
**//*---------------------------------------------------------------------*/
void CamSgnc::visibleLeftoverCamera()
{
	for(int icam = 0; icam < getChildNodeNum(); icam++)
	{
		Camera* cam = (Camera*)getChildNode(icam);
		cam->setVisible(true);
		break;
	}
}

////////////////////////////////////////////////////////////////////////////

ETK_END_NS