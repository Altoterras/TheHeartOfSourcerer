/***********************************************************************//**
 *  SgNode.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/04/14.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_G3D_SG_NODE_SG_NODE_H_
#define _TFW_G3D_SG_NODE_SG_NODE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../Base.h"

#include "SgNodeType.h"
#include "../../../framemod/FrameExecDraw.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

/*---------------------------------------------------------------------*//**
	シーングラフ ノード基底クラス

**//*---------------------------------------------------------------------*/
class SgNode : public FrameExecDraw
{
	//======================================================================
	// 定数
protected:
	// 内部フラグ
	static const u16	PRFLAG_SHOW	= 0x0001;	// 表示中
	static const u16	PRFLAG_ONSG	= 0x0002;	// シーングラフに設置されている
	static const u16	PRFLAG_USED = 0x0004;	// その他の場所で使用されている
	static const u16	PRFLAG_ONRP = 0x0008;	// リソースプールに管理されている
	static const u16	PRFLAG_CNDEST = 0x0010;	// ノードチェイン削除時に destroy する

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	inline u16 getType() const { return _type; }
	inline u16 getId() const { return _id; }
	inline void setId(u16 id) { _id = id; }
	inline bool isShow() const { return TFW_IS_FLAG(_prflag, PRFLAG_SHOW); }
	inline void setShow(bool isShow) { TFW_SET_FLAG(_prflag, PRFLAG_SHOW, isShow); }
	inline bool isOnSg() const { return TFW_IS_FLAG(_prflag, PRFLAG_ONSG); }
	inline void setOnSg(bool isOn) { TFW_SET_FLAG(_prflag, PRFLAG_ONSG, isOn); }
	inline bool isUsed() const { return TFW_IS_FLAG(_prflag, PRFLAG_USED); }
	inline void setUsed(bool isOn) { TFW_SET_FLAG(_prflag, PRFLAG_USED, isOn); }
	inline bool isOnResourcePool() const { return TFW_IS_FLAG(_prflag, PRFLAG_ONRP); }
	inline void setOnResourcePool(bool isOn) { TFW_SET_FLAG(_prflag, PRFLAG_ONRP, isOn); }
	inline bool isDestroyWhenNodeDestruction() const { return TFW_IS_FLAG(_prflag, PRFLAG_CNDEST); }
	inline void setDestroyWhenNodeDestruction(bool isOn) { TFW_SET_FLAG(_prflag, PRFLAG_CNDEST, isOn); }
	inline u16 getGeneralParam() const { return _param; }
	inline void setGeneralParam(u16 param) { _param = param; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	SgNode();
	virtual ~SgNode();
	virtual bool create(int type);
	virtual void destroy();
	virtual void exec(ExecRes* res, const ExecCtx* ec) {}
	virtual void previse(const RenderCtx* rc);
	virtual void render(const RenderCtx* rc);

	void copy(const SgNode* sgnodeSrc);

protected:
	virtual void previseImplements(const RenderCtx* rc) {}
	virtual void renderImplements(const RenderCtx* rc) {}

	//======================================================================
	// メンバ変数
private:
	u16 _type;
	u16 _id;
	u16 _prflag;
	u16 _param;
};

TFW_END_NS
	
////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_G3D_SG_NODE_SG_NODE_H_

