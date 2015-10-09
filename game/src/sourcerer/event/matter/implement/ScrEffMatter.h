/***********************************************************************//**
 *	ScrEffMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/07.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_SCR_EFF_MATTER_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_SCR_EFF_MATTER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../EventMatter.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Rect;
typedef Rect<f32> RectF32;
class Texture;

TFW_END_NS

ETK_BEGIN_NS

class MessageWindow;

ETK_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	スクリーンエフェクト マター
 *	
**//*---------------------------------------------------------------------*/
class ScrEffMatter : public EventMatter
{
	//======================================================================
	// 定数
public:
	// 種類
	enum Kind
	{
		KIND_NULL,
		KIND_FILL,
		KIND_FADEIN,
		KIND_FADEOUT,
	};

	//======================================================================
	// クラス
public:
	class Setting
	{
	public:
		Kind			_kind;			// ［必須］種類
		s32				_nframe;		// ［必須］フレーム数
		s32				_rgb;			// ［オプション］色
		
		Setting() : _kind(KIND_NULL), _nframe(0), _rgb(-1) {}
		void copy(const Setting* s)
		{
			_kind = s->_kind;
			_nframe = s->_nframe;
			_rgb = s->_rgb;
		}
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部制御
public:
	void setting(const Setting* setting);
	static bool	analyzeSettingFromXml(Setting* setting, const XmlNode* node);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ScrEffMatter();
	bool setupFromXmlNode(const XmlNode* node, const EvMatterCreateParam* cparam);
	void begin(u32 param);
	void end();
	void execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun);

	//======================================================================
	// 変数
private:
	Setting	_setting;
	s32 _hintCtrlScreff;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_SCR_EFF_MATTER_H_
