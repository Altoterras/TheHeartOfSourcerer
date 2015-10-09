/***********************************************************************//**
 *  SgNodeChain.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/04/18.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_G3D_SG_NODE_SG_NODE_CHAIN_H_
#define _TFW_G3D_SG_NODE_SG_NODE_CHAIN_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../Base.h"

#include "SgNode.h"
	
////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class List;
template<class TYPE> class ListIterator;
class SgManager;

/*---------------------------------------------------------------------*//**
 *	シーングラフ ノード連結クラス
 *
**//*---------------------------------------------------------------------*/
class SgNodeChain : public SgNode
{
	//======================================================================
	// アクセサ
public:
	s32 getChildNodeNum() const;
	SgNode* getChildNode(int index) const;
	SgNode* getChildNodeFromId(u16 sgnodeid) const;
	bool getIterator(ListIterator<SgNode*>* it) const;

	//======================================================================
	// メソッド
public:
	SgNodeChain();
	virtual ~SgNodeChain();
	virtual bool		create();
	virtual void		destroy();

	virtual bool		addChildNode(SgNode* sgnode);
	virtual bool		removeChildNode(SgNode* sgnode);
	virtual bool		removeChildNode(u16 sgnodeid);

	virtual void		exec(ExecRes* res, const ExecCtx* ec);
	virtual void		previseImplements(const RenderCtx* rc);
	virtual void		renderImplements(const RenderCtx* rc);

private:
	bool				addParentNode(SgNode* node);

	//======================================================================
	// メンバ変数
protected:
	List<SgNode*>*		_listParent;
	List<SgNode*>*		_listChild;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_G3D_SG_NODE_SG_NODE_CHAIN_H_

