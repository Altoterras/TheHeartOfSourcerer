/***********************************************************************//**
 *	TexFacialMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/02/18.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_TEX_FACIAL_MATTER_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_TEX_FACIAL_MATTER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../EventMatter.h"

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
 *	テクスチャ フェイシャル マター
 *	
**//*---------------------------------------------------------------------*/
class TexFacialMatter : public EventMatter
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
		u8				_kind;			// ［必須］種別
		bool			_enable;		// ［オプション］有効・無効
		u16				_rand;			// ［オプション］非表示から表示に切り替える確率 0 ～ 100（100 は即時）
		s32				_durfShow;		// ［オプション］連続表示フレーム数
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
	TexFacialMatter();
	~TexFacialMatter();
	bool setupFromXmlNode(const XmlNode* node, const EvMatterCreateParam* cparam);
	void begin(u32 param);

	//======================================================================
	// 変数
private:
	Setting _setting;			// 設定
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_SPRITE_MATTER_H_
