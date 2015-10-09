/***********************************************************************//**
 *	ScrModeMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2015/05/20.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_SCR_MODE_MATTER_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_SCR_MODE_MATTER_H_

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
 *	リアルタイムムービー マター
 *	
**//*---------------------------------------------------------------------*/
class ScrModeMatter : public EventMatter
{
	//======================================================================
	// 定数
public:
	// 種類
	enum Mode
	{
		MODE_NULL,
		MODE_FIX,
		MODE_SCALE,
	};

	//======================================================================
	// クラス
public:
	class Setting
	{
	public:
		Mode			_mode;			// ［必須］モード
		s32				_nframe;		// ［オプション］継続フレーム数

		Setting() : _mode(MODE_NULL), _nframe(-1) {}
		void copy(const Setting* s)
		{
			_mode = s->_mode;
			_nframe = s->_nframe;
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
	ScrModeMatter();
	bool setupFromXmlNode(const XmlNode* node, const EvMatterCreateParam* cparam);
	void begin(u32 param);
	void end();
	void execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun);

	//======================================================================
	// 変数
private:
	Setting	_setting;
	s32 _vmodeBk;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_SCR_MODE_MATTER_H_
