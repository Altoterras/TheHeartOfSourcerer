/***********************************************************************//**
 *	MovieCutMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/08.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_MOVIE_CUT_MATTER_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_MOVIE_CUT_MATTER_H_

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

class RealtimeMovieMatter;

/*---------------------------------------------------------------------*//**
 *	ムービー カット定義マター
 *	
**//*---------------------------------------------------------------------*/
class MovieCutMatter : public EventMatter
{
	//======================================================================
	// クラス
public:
	class Setting
	{
	public:
		f32				_nframe;		// フレーム数

		Setting() : _nframe(1)
		{
		}
		void copy(const Setting* s)
		{
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
	MovieCutMatter();
	bool setupFromXmlNode(const XmlNode* node, const EvMatterCreateParam* cparam);
	void begin(u32 param);
	void end();
	void execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun);

private:
	RealtimeMovieMatter* getRtmvMatter();

	//======================================================================
	// 変数
private:
	Setting			_setting;
	f32				_frmcnt;		// フレームカウンタ
	u16				_evidRtmv;		// 対象とするリアルタイムムービーのイベント ID
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_MOVIE_CUT_MATTER_H_
