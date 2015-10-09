/***********************************************************************//**
 *	ValFaderMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/08.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_VAL_FADER_MATTER_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_VAL_FADER_MATTER_H_

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
class ValFaderMatter : public EventMatter
{
	//======================================================================
	// 定数
private:
	static const u32 TARGET_TYPE_MODEL		= 1;			// Model
	static const u32 TARGET_TYPE_SPRITE		= 2;			// Sprite

	static const u32 TARGET_ELMF_POS_X		= 0x00000001;	// 位置 X
	static const u32 TARGET_ELMF_POS_Y		= 0x00000002;	// 位置 Y
	static const u32 TARGET_ELMF_POS_Z		= 0x00000004;	// 位置 Z
	static const u32 TARGET_ELMF_COLOR_R	= 0x00100000;	// 色 R
	static const u32 TARGET_ELMF_COLOR_G	= 0x00200000;	// 色 G
	static const u32 TARGET_ELMF_COLOR_B	= 0x00400000;	// 色 B
	static const u32 TARGET_ELMF_COLOR_A	= 0x00800000;	// 色 A

	//======================================================================
	// クラス
public:
	class Setting
	{
	public:
		VcString*		_name;			// 名前
		u16				_spid;			// SPID
		u32				_telmf;			// 対象要素フラグ
		f32				_valFrom;		// 開始値
		f32				_valTo;			// 終了値
		s32				_nframe;		// フレーム数

		Setting() : _name(0L), _spid(0), _telmf(0), _valFrom(0.0f), _valTo(0.0f), _nframe(1)
		{
		}
		~Setting()
		{
			delete _name;
		}
		void copy(const Setting* s)
		{
			_name = (s->_name != 0L) ? new VcString(s->_name) : 0L;
			_spid = s->_spid;
			_telmf = s->_telmf;
			_valFrom = s->_valFrom;
			_valTo = s->_valTo;
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
	void setting(const Setting* s);
	static bool	analyzeSettingFromXml(Setting* s, const XmlNode* node);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ValFaderMatter();
	~ValFaderMatter();
	bool setupFromXmlNode(const XmlNode* node, const EvMatterCreateParam* cparam);
	void destroy();
	void begin(u32 param);
	void end();
	void execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun);

private:
	bool findTargetBySpid(u16 spid);
	bool findTargetByName(const VcString* name);
	void searchTarget();
	void applyValue();

	//======================================================================
	// 変数
private:
	Setting			_setting;
	void*			_target;		// 対象
	u32				_ttype;			// 対象の型
	f32				_value;			// 値
	f32				_frmcnt;		// フレームカウンタ
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_VAL_FADER_MATTER_H_
