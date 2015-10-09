/***********************************************************************//**
 *  SgManager.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/04/18.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_G3D_SG_SGMANAGER_H_
#define _TFW_G3D_SG_SGMANAGER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"
#include "../../framemod/FrameExecDraw.h"
	
////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Tfw;
class SgNode;
class SgEntity;
class SgNodeChain;
class RenderCtx;

/*---------------------------------------------------------------------*//**
	シーングラフ管理クラス

**//*---------------------------------------------------------------------*/
class SgManager : public FrameExecDraw
{
	//======================================================================
	// アクセサ

public:
	inline SgNodeChain*	getRootNode() const	{	return _nodeRoot;		}
	inline Tfw*			getOwner() const	{	return _tfwOwnerRef;	}

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual SgNode*		getNodeFromId(u16 sgnodeid) const;
	virtual bool		addNode(SgNode* sgnode) = 0;
	virtual bool		removeNode(SgNode* sgnode) = 0;

protected:
	bool				addChildNode(SgNodeChain* sgncParent, SgNode* sgnode);
	bool				removeChildNode(SgNodeChain* sgncParent, SgNode* sgnode);
	bool				removeChildNode(SgNodeChain* sgncParent, u16 sgnodeid);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	SgManager();
	virtual ~SgManager();
	virtual bool		create(Tfw* tfwOwnerRef);
	virtual void		destroy();
	virtual bool		reset();

	virtual void		exec(ExecRes* res, const ExecCtx* ec);
	virtual void		previse(const RenderCtx* rc);
	virtual void		render(const RenderCtx* rc);

	//======================================================================
	// メンバ変数

private:
	Tfw*				_tfwOwnerRef;
	SgNodeChain*		_nodeRoot;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_G3D_SG_SGMANAGER_H_

