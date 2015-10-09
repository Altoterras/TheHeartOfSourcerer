/***********************************************************************//**
 *	SpriteMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/07.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_SPRITE_MATTER_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_SPRITE_MATTER_H_

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

class MlFileId;

/*---------------------------------------------------------------------*//**
 *	スプライト表示マター
 *	
**//*---------------------------------------------------------------------*/
class SpriteMatter : public EventMatter
{
	//======================================================================
	// 定数
public:
	// レイアウトモード
	enum Layout
	{
		LAYOUT_NULL = 0,				// （無効値）
		LAYOUT_SPEC_RECT,				// 矩形指定（デフォルト）
		LAYOUT_FIT_SCREEN_KEEP_RATIO,	// アスペクト比を維持しながら最大化
	};

	//======================================================================
	// クラス
public:
	class Setting
	{
	public:
		MlFileId*		_mlfileidTex;	// ［必須］テクスチャファイル ID
		Layout			_layout;		// ［オプション］レイアウトモード
		RectF32*		_rect;			// ［オプション］矩形
		RectF32*		_uv;			// ［オプション］UV
		u32				_colorSide;		// ［オプション］サイドカラー
		bool			_isWait;		// ［オプション］入力待ち
//		bool			_isNoFitStdRsl;	// ［オプション］標準解像度に対してフィットさせない
		f32				_duration;		// ［オプション］継続フレーム
		f32				_frameFadeOut;	// ［オプション］フェードアウトフレーム
		f32				_frameFadeIn;	// ［オプション］フェードインフレーム
		f32				_frameWaitMin;	// ［オプション］入力待ち時の最低表示フレーム

		Setting();
		~Setting();
		void copy(const Setting* s);
	};
	
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 矩形を得る
	inline RectF32* rect() { return _setting._rect; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部制御
public:
	void setting(const Setting* setting);
	static bool	analyzeSettingFromXml(Setting* setting, const XmlNode* node);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	SpriteMatter();
	~SpriteMatter();
	bool setupFromXmlNode(const XmlNode* node, const EvMatterCreateParam* cparam);
	void begin(u32 param);
	void end();
	void execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun);
	void draw(const RenderCtx* rc);

	//======================================================================
	// 変数
private:
	Setting _setting;
	Texture* _tex;
	f32 _frameEnd;
	u8 _alpha;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_SPRITE_MATTER_H_
