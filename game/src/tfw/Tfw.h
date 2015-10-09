/***********************************************************************//**
 *	Tfw.h
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2009/04/13.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_H_
#define _TFW_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "Base.h"
#include "framemod/FrameExecDraw.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class SgManager;
class View;
class ResourcePool;
class IdManager;
class ExecCtx;
class RenderCtx;

/*---------------------------------------------------------------------*//**
 *	Terras Framework 統括クラス
 *	
**//*---------------------------------------------------------------------*/
class Tfw : public FrameExecDraw
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	inline SgManager*		getSgManager() const	{	return _sgmng;		}
	inline View*			getView() const			{	return _view;		}
	inline Renderer*		getRenderer() const		{	return _renderer;	}
	inline ResourcePool*	getResourcePool() const	{	return _respool;	}
	inline IdManager*		getIdManager() const	{	return _idmngr;		}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void resizeView(f32 width, f32 height, u32 dispflags);
	void resizeView(f32 width, f32 height);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Tfw();
	virtual ~Tfw();

	bool create(SgManager* sgmngEntr, View* viewEntr);
	void destroy();

	virtual void exec(ExecRes* res, const ExecCtx* ec);
	virtual void render(const RenderCtx* rc);

	//======================================================================
	// 変数
private:
	SgManager*		_sgmng;
	View*			_view;
	Renderer*		_renderer;
	ResourcePool*	_respool;
	IdManager*		_idmngr;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_H_