/***********************************************************************//**
 *	ShadowCtrlMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/05/01.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_SHADOW_CTRL_MATTER_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_SHADOW_CTRL_MATTER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../EventMatter.h"

#include "../../../../tfw/lib/Vector.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

#if TEXFACIAL_SRCB
class Shape;
class ShapeCtx;
#else
class HrAnimObjCtx;
#endif
class ShapeModel;
class RenderCtx;

TFW_END_NS

ETK_BEGIN_NS

class MessageWindow;

ETK_END_NS

SRCR_BEGIN_NS

class RealtimeMovieMatter;

/*---------------------------------------------------------------------*//**
 *	影制御マター
 *	
**//*---------------------------------------------------------------------*/
class ShadowCtrlMatter : public EventMatter
{
	//======================================================================
	// 定数
private:

	//======================================================================
	// クラス
public:
	class Setting
	{
	public:
		u16				_charid;		// ［必須］キャラクタ ID
		bool			_enable;		// ［オプション］有効・無効
		///(モデルの色調整中につき、将来実装)///u8				_opacity;		// ［オプション］不透明度 0 ～ 255
		f32				_radius;		// ［オプション］丸影半径
		Vector3F		_offsetPos;		// ［オプション］位置オフセット
		u32				_flagsSet;		// 有効変数フラグ

		Setting();
		void copy(const Setting* s);
	};
	
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部制御
public:
	void setting(const Setting* setting);
	static bool	analyzeSettingFromXml(Setting* setting, const XmlNode* node);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ShadowCtrlMatter();
	~ShadowCtrlMatter();
	bool setupFromXmlNode(const XmlNode* node, const EvMatterCreateParam* cparam);
	void begin(u32 param);

	//======================================================================
	// 変数
private:
	Setting _setting;			// 設定
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_SHADOW_CTRL_MATTER_H_
