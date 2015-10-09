/***********************************************************************//**
 *  CamSgnc.h
 *	Envelopment Terras Kit
 *
 *  Created by Ryoutarou Kishi on 2009/07/17.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ETK_SG_IMPLEMENT_SOR_CAM_SGNC_H_
#define _ETK_SG_IMPLEMENT_SOR_CAM_SGNC_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../tfw/g3d/sg/node/SgNodeChain.h"
	
////////////////////////////////////////////////////////////////////////////
// クラス

ETK_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	カメラ ノード連結クラス
 *
**//*---------------------------------------------------------------------*/
class CamSgnc : public SgNodeChain
{
	//======================================================================
	// アクセサ
public:

	//======================================================================
	// メソッド
public:
	CamSgnc();
	virtual ~CamSgnc();

	bool				switchVisible(u16 nodeid);

	virtual bool		removeChildNode(SgNode* sgnode);
	virtual bool		removeChildNode(u16 sgnodeid);

private:
	void				visibleLeftoverCamera();

	//======================================================================
	// 変数
private:
};

ETK_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ETK_SG_IMPLEMENT_SOR_CAM_SGNC_H_

