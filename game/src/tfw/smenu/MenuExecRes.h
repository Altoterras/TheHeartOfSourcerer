/***********************************************************************//**
 *  MenuExecRes.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/06/02.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_SMENU_EXEC_RES_H_
#define _TFW_SMENU_EXEC_RES_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "Base.h"

#include "../framemod/FrameExecDraw.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS
template<class TYPE> class TreeNode;

SMENU_BEGIN_NS
class MenuTreeNode;

/*---------------------------------------------------------------------*//**
 *	メニュー フレーム制御結果
 *	
**//*---------------------------------------------------------------------*/
class MenuExecRes : public ExecRes
{
	//======================================================================
	// 定数
public:
	static const int		ON_NULL			= 0;
	static const int		ON_FUNC			= 1;
	static const int		ON_DIVE			= 2;
	static const int		ON_CANCEL		= 3;
	static const int		ON_CSTMCHLD		= 4;
	static const int		ON_CLICKOUTSIDE	= 5;
	
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	inline s32*						returnValue()	{	return &_value1;								}
	inline TreeNode<MenuTreeNode>**	targetNode()	{	return (TreeNode<MenuTreeNode>**)&_obj1;		}
};

SMENU_END_NS
TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_SMENU_EXEC_RES_H_
