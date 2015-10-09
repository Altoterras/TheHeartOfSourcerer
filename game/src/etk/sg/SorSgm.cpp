/***********************************************************************//**
 *  SorSgm.cpp
 *	Envelopment Terras Kit
 *
 *  Created by Ryoutarou Kishi on 2009/04/18.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SorSgm.h"

// Friends
#include "CamSgnc.h"
#include "OpacityBgSgnc.h"
#include "OpacityBgNoFogSgnc.h"
#include "BinaryTransparentBgSgnc.h"
#include "TranslucentBgSgnc.h"
#include "ModelEffectSgnc.h"
#include "ParticleEffectSgnc.h"
#include "TbfDefineEx.h"
#include "ZsortModelSgnc.h"
#include "ZsortModelNoFogSgnc.h"

// External
#include "../../tfw/Tfw.h"
#include "../../tfw/collection/List.h"
#include "../../tfw/common/IdManager.h"
#include "../../tfw/common/ResourcePool.h"
#include "../../tfw/framemod/RenderCtx.h"
#include "../../tfw/g3d/sg/node/SgNodeChain.h"
#include "../../tfw/g3d/sg/node/SgEntity.h"
#include "../../tfw/g3d/sg/node/implement/Camera.h"
#include "../../tfw/g3d/sg/node/implement/Model.h"
#include "../../tfw/gcmn/Texture.h"
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/lib/Vector.h"
#include "../../tfw/string/VcString.h"

// Library

////////////////////////////////////////////////////////////////////////////

ETK_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// SorSgm メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	現在有効なカメラを得る
**//*---------------------------------------------------------------------*/
Camera* SorSgm::getVisibleCamera() const
{
	CamSgnc* nodecCam = (CamSgnc*)getRootNode()->getChildNode(SorSgm::EK_CAM);
	ASSERT(nodecCam != 0L);
	for(int i = 0; i < nodecCam->getChildNodeNum(); i++)
	{
		Camera* cam = (Camera*)nodecCam->getChildNode(i);
		if(cam->isVisible())
		{
			return cam;
		}
	}
	return 0L;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	エンティティノード追加
**//*---------------------------------------------------------------------*/
bool SorSgm::addNode(SgEntity* sgnode, bool isAddResourcePool)
{
	// 子ノードに追加
	Ek ek = convSgTypeToEntityKind(sgnode);
	if(ek == SorSgm::NUM_EK)	{	return false;	}
	SgNodeChain* sgnc = getKindChain(ek);
	if(sgnc == 0L)				{	return false;	}
	addChildNode(sgnc, sgnode);

	// リソースプールに追加
	if(isAddResourcePool)
	{
		ResourcePool* respl = getOwner()->getResourcePool();
		respl->add(&sgnode, 1);
	}
	else
	{
		// リソースプールに追加しない場合は ID を手動追加する
		IdManager* idmngr = getOwner()->getIdManager();
		sgnode->setId(idmngr->getNewId());
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ノード追加
**//*---------------------------------------------------------------------*/
bool SorSgm::addNode(SgNode* sgnode)
{
	Ek ek = convSgTypeToEntityKind(sgnode);
	if(ek == SorSgm::NUM_EK)
	{
		return false;	// エンティティでなないものは非対応
	}

	return addNode((SgEntity*)sgnode, false);	// デフォルト動作はリソースプールに追加しない
}

/*---------------------------------------------------------------------*//**
	ノード削除
**//*---------------------------------------------------------------------*/
bool SorSgm::removeNode(SgNode* sgnode)
{
	Ek ek = convSgTypeToEntityKind(sgnode);
	if(ek == SorSgm::NUM_EK)	{	return false;	}
	SgNodeChain* sgnc = getKindChain(ek);
	if(sgnc == 0L)				{	return false;	}
	removeChildNode(sgnc, sgnode);

	// リソースプール管理ではない場合は ID を手動解放する
	if(!sgnode->isOnResourcePool())
	{
		IdManager* idmngr = getOwner()->getIdManager();
		ASSERT(idmngr->isUsedId(sgnode->getId()));
		idmngr->releaseId(sgnode->getId());
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	要素カテゴリでノードチェインを得る
**//*---------------------------------------------------------------------*/
SgNodeChain* SorSgm::getKindChain(Ek ek) const
{
	ASSERT(getRootNode() != 0L);
	SgNode* node = getRootNode()->getChildNode(ek);
	if(node == 0L)	{	return 0L;	}
	return (SgNodeChain*)node;
}

/*---------------------------------------------------------------------*//**
	ノードタイプをエンティティタイプに変換する
**//*---------------------------------------------------------------------*/
SorSgm::Ek SorSgm::convSgTypeToEntityKind(SgNode* sgnode)
{
	switch(sgnode->getType())
	{
	case SGNTYPE_CAMERA:
		return SorSgm::EK_CAM;
		break;
	case SGNTYPE_MODEL:
		#if TEST_ALL_ZSORT
			return SorSgm::EK_ZSORT;	// test
		#else
		{
			Model* mdl = (Model*)sgnode;
			u16 rendorder = mdl->getRenderOrder();
			return convRenderOrderToEntityKind(sgnode, rendorder);
		}
		#endif	// #if TEST_ALL_ZSORT
		break;
	case SGNTYPE_DIRECT:
		{
			// 汎用パラメータを使用して描画エンティティを得る
			s32 ek = sgnode->getGeneralParam();
			if((0 <= ek) && (ek < SorSgm::NUM_EK))
			{
				return (SorSgm::Ek)ek;
			}
			else
			{
				TRACE(VcString::format("{SorSgm::convSgTypeToEntityKind} invalid entity-kind ! : %d\n", ek));
				sgnode->setShow(false);
				return SorSgm::EK_OPCTY;
			}
		}
		break;
	}
	ASSERT(false);	// 未対応
	return SorSgm::NUM_EK;
}

/*---------------------------------------------------------------------*//**
	描画順をエンティティタイプに変換する
**//*---------------------------------------------------------------------*/
SorSgm::Ek SorSgm::convRenderOrderToEntityKind(SgNode* sgnode, u16 rendorder)
{
	if((		RO_FARBG_OPCTY_MIN	<= rendorder) && (rendorder <=	RO_FARBG_OPCTY_MAX	))	{	return SorSgm::EK_OPCTY_NOFOG;	}
	else if((	RO_FARBG_TRANS_MIN	<= rendorder) && (rendorder <=	RO_FARBG_TRANS_MAX	))	{	return SorSgm::EK_ZSORT_NOFOG;	}
	else if((	RO_OPCTY_MIN		<= rendorder) && (rendorder <=	RO_OPCTY_MAX		))	{	return SorSgm::EK_OPCTY;		}
	else if((	RO_BITRNS_MIN		<= rendorder) && (rendorder <=	RO_BITRNS_MAX		))	{	return SorSgm::EK_BITRNS;		}
	else if((	RO_STUCK_TRANS_MIN	<= rendorder) && (rendorder <=	RO_STUCK_TRANS_MAX	))	{	return SorSgm::EK_STUCK_TRANS;	}
	else if((	RO_TRANS_MIN		<= rendorder) && (rendorder <=	RO_TRANS_MAX		))	{	return SorSgm::EK_ZSORT;		}
	else if((	RO_EFF_MDL_MIN		<= rendorder) && (rendorder <=	RO_EFF_MDL_MAX		))	{	return SorSgm::EK_ZSORT;		}
	else if((	RO_EFF_PTCL_MIN		<= rendorder) && (rendorder <=	RO_EFF_PTCL_MAX		))	{	return SorSgm::EK_ZSORT;		}
	else if((	RO_ZSORT_MIN		<= rendorder) && (rendorder <=	RO_ZSORT_MAX		))	{	return SorSgm::EK_ZSORT;		}
	else
	{
		// 描画順が未定義か範囲外（いちおうエラーにしないことにする）
		TRACE(VcString::format("{SorSgm::convRenderOrderToEntityKind} invalid render-order ! : %d\n", rendorder));
		sgnode->setShow(false);
		return SorSgm::EK_OPCTY;
	}
}

/*---------------------------------------------------------------------*//**
	背景を表示・非表示にする
**//*---------------------------------------------------------------------*/
void SorSgm::showBg(bool isShow)
{
	ListIterator<SgNode*> itEk;
	if(!getRootNode()->getIterator(&itEk))	{	return;	}
	for(; itEk.has(); itEk.next())
	{
		SgNodeChain* sgnEk = (SgNodeChain*)itEk.object();	// 子ノードは全て SGNTYPE_CHAIN である

		ListIterator<SgNode*> itWk;
		if(!sgnEk->getIterator(&itWk))	{	ASSERT(false);	return;	}
		for(; itWk.has(); itWk.next())
		{
			SgNode* sgnWk = itWk.object();
			if(sgnWk->getType() == SGNTYPE_MODEL)
			{
				Model* mdl = (Model*)sgnWk;
				if(TFW_IS_FLAG(mdl->getExFlags(), TbfDefineEx::MEXF_BG) && !TFW_IS_FLAG(mdl->getModelFlag(), Model::MFLAG_NO_USE))
				{
					mdl->setShow(isShow);
				}
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	デバッグ出力
**//*---------------------------------------------------------------------*/
void SorSgm::debug_print()
{
#if defined(_DEBUG)
	TRACE("{SorSgm::debug_print} >>>\n");
	for(int i = 0; i < NUM_EK; i++)
	{
		SgNodeChain* cnode = getKindChain((Ek)i);
		switch(i)
		{
		case EK_CAM:			TRACE("    EK_CAM : ");			break;
		case EK_OPCTY_NOFOG:	TRACE("    EK_OPCTY_NOFOG : ");	break;
		case EK_ZSORT_NOFOG:	TRACE("    EK_ZSORT_NOFOG : ");	break;
		case EK_OPCTY:			TRACE("    EK_OPCTY : ");		break;
		case EK_BITRNS:			TRACE("    EK_BITRNS : ");		break;
		case EK_STUCK_TRANS:	TRACE("    EK_STUCK_TRNS : ");	break;
		case EK_ZSORT:			TRACE("    EK_ZSORT : ");		break;
		}
		TRACE("num = %d\n", cnode->getChildNodeNum());
	}
#endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool SorSgm::create(Tfw* tfwOwnerRef)
{
	if(!SgManager::create(tfwOwnerRef))	{	return false;	}

	// それぞれの要素カテゴリの追加（追加順番が重要）
	// カメラ - EK_CAM
	SgNodeChain* nodeCam = new CamSgnc();
	if(!nodeCam->create()) { return false; }
	addChildNode(getRootNode(), nodeCam);
	// フォグなし不透明モデル - EK_OPCTY_NOFOG
	SgNodeChain* nodeOpcNfogMdl	= new OpacityBgNoFogSgnc();
	if(!nodeOpcNfogMdl->create()) { return false; }
	addChildNode(getRootNode(), nodeOpcNfogMdl);
	// フォグなし Z ソート半透明モデル - EK_ZSORT_NOFOG
	SgNodeChain* nodeZsortNfogMdl = new ZsortModelNoFogSgnc();
	if(!nodeZsortNfogMdl->create()) { return false; }
	addChildNode(getRootNode(), nodeZsortNfogMdl);
	// 不透明モデル - EK_OPCTY
	SgNodeChain* nodeOpcMdl	= new OpacityBgSgnc();
	if(!nodeOpcMdl->create()) { return false; }
	addChildNode(getRootNode(), nodeOpcMdl);
	// 二値透明モデル - EK_BITRNS
	SgNodeChain* nodeBitrsMdl = new BinaryTransparentBgSgnc();
	if(!nodeBitrsMdl->create()) { return false; }
	addChildNode(getRootNode(), nodeBitrsMdl);
	// 密着半透明モデル - EK_STUCK_TRANS
	SgNodeChain* nodeStuckTrsMdl = new TranslucentBgSgnc();
	if(!nodeStuckTrsMdl->create()) { return false; }
	addChildNode(getRootNode(), nodeStuckTrsMdl);
	// Z ソート半透明モデル - EK_ZSORT
	SgNodeChain* nodeZsortMdl = new ZsortModelSgnc();
	if(!nodeZsortMdl->create()) { return false; }
	addChildNode(getRootNode(), nodeZsortMdl);

	return true;
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void SorSgm::render(const RenderCtx* rc)
{
	SgManager::previse(rc);

	SgManager::render(rc);
}

////////////////////////////////////////////////////////////////////////////

ETK_END_NS
