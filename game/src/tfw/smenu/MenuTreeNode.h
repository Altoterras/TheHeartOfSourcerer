/***********************************************************************//**
 *  MenuTreeNode.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/06/02.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_SMENU_MENU_TREE_NODE_H_
#define _TFW_SMENU_MENU_TREE_NODE_H_

#include "../Base.h"
#include "Base.h"

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

class XmlNode;
class VcString;
class CStringBase;
class PanelBase;

SMENU_BEGIN_NS

class MenuPanelReceptor;

/*---------------------------------------------------------------------*//**
 *	メニュー ツリー ノード
 *	
**//*---------------------------------------------------------------------*/
class MenuTreeNode
{
	//======================================================================
	// 定数
public:
	enum SysFlag
	{
		SF_FUNCTION			= 0x00000001,
		SF_MENU_CLOSE		= 0x00000002,
		SF_CUSTOM_CHILD		= 0x00000004,
		SF_DEBUG			= 0x00000008,
		SF_CHECKED			= 0x00000010,
		SF_DISABLED			= 0x00000020,
		SF_HIDDEN			= 0x00000040,
		SF_UNUSED			= 0x00000080,
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// ID を得る
	inline int						getId() const					{	return _id;				}
	// 名前の取得・設定
	inline const VcString*			getName() const					{	return _name;			}
	void							setName(const VcString* name);
	// 値の取得・設定
	inline const VcString*			getValue() const				{	return _value;			}
	void							setValue(const VcString* valu);
	// 説明文の取得・設定
	inline const VcString*			getExplain() const				{	return _explain;		}
	void							setExplain(const VcString* str);
	// パネルレセプターを得る
	inline const MenuPanelReceptor*	getPanelReceptor() const		{	return _pnlrcp;			}
	inline MenuPanelReceptor*		panelReceptor() const			{	return _pnlrcp;			}
	// 汎用フラグの取得・設定
	inline int						getGeneralFlags() const			{	return _flagsGeneral;	}
	inline void						setGeneralFlags(u32 flags)		{	_flagsGeneral = flags;	}
	// 汎用パラメータの取得・設定
	inline u64						getGeneralParam() const			{	return _paramGeneral;	}
	inline void						setGeneralParam(u64 param)		{	_paramGeneral = param;	}
	// ファンクションノードかどうか
	inline bool						isFunction() const				{	return TFW_IS_FLAG(_flagsSys, SF_FUNCTION);		}
	// メニュークローズノードかどうか
	inline bool						isMenuClose() const				{	return TFW_IS_FLAG(_flagsSys, SF_MENU_CLOSE);	}
	// カスタムノードかどうか
	inline bool						isCustomChild() const			{	return TFW_IS_FLAG(_flagsSys, SF_CUSTOM_CHILD);	}
	// デバッグノードかどうか
	inline bool						isDebug() const					{	return TFW_IS_FLAG(_flagsSys, SF_DEBUG);		}
	// チェック状態の取得・設定
	inline bool						isChecked() const				{	return TFW_IS_FLAG(_flagsSys, SF_CHECKED);		}
	inline void						setChecked(bool is)				{	TFW_SET_FLAG(_flagsSys, SF_CHECKED, is);		}
	// 無効化状態の取得・設定
	inline bool						isDisabled() const				{	return TFW_IS_FLAG(_flagsSys, SF_DISABLED);		}
	inline void						setDisabled(bool is)			{	TFW_SET_FLAG(_flagsSys, SF_DISABLED, is);		}
	// 隠し状態の取得・設定
	inline bool						isHidden() const				{	return TFW_IS_FLAG(_flagsSys, SF_HIDDEN);		}
	inline void						setHidden(bool is)				{	TFW_SET_FLAG(_flagsSys, SF_HIDDEN, is);			}
	// 未使用状態の取得・設定
	inline bool						isUnused() const				{	return TFW_IS_FLAG(_flagsSys, SF_UNUSED);		}
	inline void						setUnused(bool is)				{	TFW_SET_FLAG(_flagsSys, SF_UNUSED, is);			}
	// パネルを得る
	PanelBase*						getPanel() const;
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	MenuTreeNode();
	~MenuTreeNode();
	bool create(const XmlNode* node);
	bool create(const CStringBase* name, bool isFunction, bool isMenuClose, bool isCustomChild, u32 flagsGeneral, u64 paramGeneral);
	void destroy();
	void copy(const MenuTreeNode* src);

	//======================================================================
	// 変数
private:
	int					_id;
	VcString*			_name;
	VcString*			_value;
	VcString*			_explain;
	MenuPanelReceptor*	_pnlrcp;
	u32					_flagsSys;
	u32					_flagsGeneral;
	u64					_paramGeneral;
};

////////////////////////////////////////////////////////////////////////////

SMENU_END_NS
TFW_END_NS

#endif	// _TFW_SMENU_MENU_TREE_NODE_H_
