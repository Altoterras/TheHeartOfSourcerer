/***********************************************************************//**
 *	SpriteMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/07.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SpriteMatter.h"

// Friends
#include "../EventMatterType.h"
#include "../../EventSys.h"
#include "../../../body/Game.h"
#include "../../../body/GameExecCtx.h"
#include "../../../file/MlFileId.h"
#include "../../../file/GameFileIdDef.h"
#include "../../../gui/GameGui.h"

// External
#include "../../../../etk/sg/CamSgnc.h"
#include "../../../../etk/sg/SorSgm.h"
#include "../../../../etk/sg/TbfDefineEx.h"
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../tfw/gcmn/Texture.h"
#include "../../../../tfw/gcmn/View.h"
#include "../../../../tfw/lib/Color.h"
#include "../../../../tfw/lib/Rect.h"
#include "../../../../tfw/lib/XmlParser.h"
#include "../../../../tfw/lib/XmlParserUtils.h"
#include "../../../../tfw/string/StringUtils.h"
#include "../../../../tfw/ui/TouchPanel.h"


SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// SpriteMatter メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	動作設定
**//*---------------------------------------------------------------------*/
void SpriteMatter::setting(const Setting* setting)
{
	_setting.copy(setting);
}

/*---------------------------------------------------------------------*//**
	設定を XML から解析する
**//*---------------------------------------------------------------------*/
bool SpriteMatter::analyzeSettingFromXml(Setting* setting, const XmlNode* node)
{
	VcString swk;
	s32 iwk;
	bool bwk;
	Vector4F vwk;

	// ［必須］テクスチャファイル ID
	if(!XmlParserUtils::analyzeAtrb(&swk, node, "file_id"))
	{
		return false;
	}
	setting->_mlfileidTex->create(&swk);

	// ［オプション］レイアウトモード
	if(XmlParserUtils::analyzeAtrb(&swk, node, "layout"))
	{
		if(swk.equalsIgnoreCase("fit_screen_keep_ratio"))
		{
			setting->_layout = SpriteMatter::LAYOUT_FIT_SCREEN_KEEP_RATIO;
		}
	}

	// ［オプション］矩形
	if(XmlParserUtils::analyzeAtrb(&vwk, node, "rect"))
	{
		setting->_rect->copy(&vwk);
	}

	// ［オプション］UV
	if(XmlParserUtils::analyzeAtrb(&vwk, node, "uv"))
	{
		setting->_uv->copy(&vwk);
	}

	// ［オプション］サイドカラー
	if(XmlParserUtils::analyzeAtrb(&swk, node, "side_color"))
	{
		if(swk.getLength() <= 8)
		{
			setting->_colorSide = StringUtils::toUnsignedInteger(&swk, 16);
		}
	}

	// ［オプション］入力待ち
	if(XmlParserUtils::analyzeAtrb(&bwk, node, "wait"))
	{
		setting->_isWait = bwk;
	}

	// ［オプション］継続フレーム
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "durf"))
	{
		setting->_duration = (f32)iwk;
	}

	// ［オプション］フェードアウトフレーム
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "fade_out_frame_num"))
	{
		setting->_frameFadeOut = (f32)iwk;
	}

	// ［オプション］フェードインフレーム
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "fade_in_frame_num"))
	{
		setting->_frameFadeIn = (f32)iwk;
	}

	// ［オプション］入力待ち時の最低表示フレーム
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "wait_min_frame_num"))
	{
		setting->_frameWaitMin = (f32)iwk;
	}

#if 0	// 廃止 【2015/05/19 r-kishi】
	// ［オプション］矩形を標準解像度からフィットさせない
	if(XmlParserUtils::analyzeAtrb(&bwk, node, "no_fit_std_resolution"))
	{
		setting->_isNoFitStdRsl = bwk;
	}
#endif

	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
SpriteMatter::SpriteMatter()
	: EventMatter(EMTYPE_SPRITE)
	, _tex(0L)
	, _frameEnd(0.0f)
	, _alpha(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
SpriteMatter::~SpriteMatter()
{
	ASSERT(_tex == 0L);
}

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool SpriteMatter::setupFromXmlNode(const XmlNode* node, const EvMatterCreateParam* cparam)
{
	if(!EventMatter::setupFromXmlNode(node, cparam))	{	return false;	}

	// - - - - - - - - - - - - - - - - - - - - - - - - -
	// 動作設定解析

	analyzeSettingFromXml(&_setting, node);
	
	return true;
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
void SpriteMatter::begin(u32 param)
{
	EventMatter::begin(param);

	SorSgm* sgm = Game::getGame()->getSceneGraph();

	// テクスチャを読み込む
	u32 fileid = _setting._mlfileidTex->getFileId(Env_getLangId());
	ASSERT(fileid != 0);
	Texture::Format fmtTex = Texture::FMT_PNG;
	Texture* tex = new Texture();
	if(!tex->create(fileid, fmtTex))
	{
		delete tex;
		tex = 0L;
		ASSERT(false);
		return;
	}
	_tex = tex;

	// その他初期化
	_frameEnd = _setting._isWait ? F32_PMAX : _setting._duration;
	_alpha = (_setting._frameFadeIn > 0.0f) ?  0 : 255;
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void SpriteMatter::end()
{
	// テクスチャを削除
	if(_tex != 0L)
	{
		_tex->destroy();
		delete _tex;
		_tex = 0L;
	}

	EventMatter::end();
}

/*---------------------------------------------------------------------*//**
	毎フレーム処理実装
**//*---------------------------------------------------------------------*/
void SpriteMatter::execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun)
{
	GameExecCtx* gec = (GameExecCtx*)ec;

	// ウェイトモード時のタップ対応
	if(_setting._isWait && (frameBegun > _setting._frameWaitMin))
	{
		TouchPanel* ui = gec->getTouchPanel(); 
		if(!ui->isReacted())
		{
			if(ui->isClickRelease(0L))
			{
				// 終了フレームを設定
				if(_frameEnd == F32_PMAX)	// 未設定の場合
				{
					_frameEnd = frameBegun + _setting._frameFadeOut;
				}
			}
		}
	}

	// フェード処理
	f32 alpha = 1.0f;
	if(_setting._frameFadeIn > 0.0f)
	{
		if(frameBegun < _setting._frameFadeIn)
		{
			f32 fcur = frameBegun;
			f32 fmax = _setting._frameFadeIn;
			if(fcur < fmax)	{	alpha = (f32)::sinf(fcur / fmax * TFW_PI * 0.5f);	}
			else			{	alpha = 1.0f;										}
		}
	}
	if(_setting._frameFadeOut > 0.0f)
	{
		if((_frameEnd - _setting._frameFadeOut) <= frameBegun)
		{
			f32 fcur = frameBegun - (_frameEnd - _setting._frameFadeOut);
			f32 fmax = _setting._frameFadeOut;
			alpha = 1.0f - (f32)::sinf(fcur / fmax * TFW_PI * 0.5f);
		}
	}
	if(frameBegun > _frameEnd)
	{
		alpha = 0.0f;
	}
	_alpha = (u8)(alpha * 255.0f);

	// 終了
	if(frameBegun >= _frameEnd)
	{
		end();
	}
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void SpriteMatter::draw(const RenderCtx* rc)
{
	if(_tex == 0L)	{	return;	}
	Renderer* rdr = rc->getRenderer();
	rdr->setSolidColor(255, 255, 255, _alpha);
	rdr->bindTexture(_tex);

	if(_setting._layout == SpriteMatter::LAYOUT_FIT_SCREEN_KEEP_RATIO)	// アスペクト比維持で最大表示
	{
#if 1
		ColorU8 colSide((u8)((_setting._colorSide >> 24) & 0xff), (u8)((_setting._colorSide >> 16) & 0xff), (u8)((_setting._colorSide >> 8) & 0xff), (u8)(((_setting._colorSide >> 0) & 0xff) * _alpha / 255.0f));
		RendererUtils::draw2dTextureToScreenFitLongSide(rdr, Game::getGame()->getTfw()->getView(), _setting._uv, &colSide);
#else
		f32 aspectTex = (_setting._uv->width()) / _setting._uv->height();
		tfw::View* view = Game::getGame()->getTfw()->getView();
		if(aspectTex == view->getLogicalAspect())
		{
			rect.set(0.0f, 0.0f, view->getLogicalWidth(), view->getLogicalHeight());
		}
		else if(aspectTex < view->getLogicalAspect())
		{
			rect._v[3] = view->getLogicalHeight();
			rect._v[2] = rect._v[3] * aspectTex;
			rect._v[0] = (view->getLogicalWidth() - rect._v[2]) * 0.5f;
			rect._v[1] = 0.0f;
		}
		else
		{
			rect._v[2] = view->getLogicalWidth();
			rect._v[3] = rect._v[2] / aspectTex;
			rect._v[0] = 0.0f;
			rect._v[1] = (view->getLogicalHeight() - rect._v[3]) * 0.5f;
		}
		RendererUtils::draw2dTextureRect(rdr, &rect, _setting._uv);

		if((_setting._colorSide != 0) && ((rect._v[0] != 0.0f) || (rect._v[1] != 0.0f)))
		{
			rdr->setSolidColor((u8)((_setting._colorSide >> 24) & 0xff), (u8)((_setting._colorSide >> 16) & 0xff), (u8)((_setting._colorSide >> 8) & 0xff), (u8)(((_setting._colorSide >> 0) & 0xff) * _alpha / 255.0f));
			if(rect._v[0] != 0.0f)
			{
				RendererUtils::draw2dRect(rdr, 0.0f,                    0.0f, rect._v[0], view->getLogicalHeight());
				RendererUtils::draw2dRect(rdr, rect._v[0] + rect._v[2], 0.0f, rect._v[0], view->getLogicalHeight());
			}
			else if(rect._v[1] != 0.0f)
			{
				RendererUtils::draw2dRect(rdr, 0.0f, 0.0f,                    view->getLogicalWidth(), rect._v[1]);
				RendererUtils::draw2dRect(rdr, 0.0f, rect._v[1] + rect._v[3], view->getLogicalWidth(), rect._v[1]);
			}
		}
#endif
	}
#if 0
	else if(!_setting._isNoFitStdRsl && ((Game::getGame()->getStandardWidth() != Game::getGame()->getLogicalWidth()) || (Game::getGame()->getStandardHeight() != Game::getGame()->getLogicalHeight())))	// 全画面フィット
	{
		// 標準解像度に対してフィットさせる
		f32 xs = Game::getGame()->getLogicalWidth() / (f32)Game::getGame()->getStandardWidth();
		f32 ys = Game::getGame()->getLogicalHeight() / (f32)Game::getGame()->getStandardHeight();
		RectF32 rect(*_setting._rect);
		rect._v[0] *= xs;
		rect._v[1] *= ys;
		rect._v[2] *= xs;
		rect._v[3] *= ys;
		RendererUtils::draw2dTextureRect(rdr, &rect, _setting._uv);
	}
#endif
	else
	{
		RendererUtils::draw2dTextureRect(rdr, _setting._rect, _setting._uv);
	}
}

//==========================================================================
// SpriteMatter::Setting メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
SpriteMatter::Setting::Setting()
	: _mlfileidTex(new MlFileId())
	, _layout(SpriteMatter::LAYOUT_SPEC_RECT)
	, _rect(new RectF32())
	, _uv(new RectF32(0.0f, 0.0f, 1.0f, 1.0f))
	, _colorSide(0)
	, _isWait(false)
//	, _isNoFitStdRsl(false)
	, _duration(0.0f)
	, _frameFadeOut(0.0f)
	, _frameFadeIn(0.0f)
	, _frameWaitMin(0.0f)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
SpriteMatter::Setting::~Setting()
{
	delete _rect;
	delete _uv;

	_mlfileidTex->destroy();
	delete _mlfileidTex;
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void SpriteMatter::Setting::copy(const Setting* s)
{
	_mlfileidTex->copy(s->_mlfileidTex);
	_layout = s->_layout;
	_rect->copy(s->_rect);
	_uv->copy(s->_uv);
	_colorSide = s->_colorSide;
	_isWait = s->_isWait;
//	_isNoFitStdRsl = s->_isNoFitStdRsl;
	_duration = s->_duration;
	_frameFadeOut = s->_frameFadeOut;
	_frameFadeIn = s->_frameFadeIn;
	_frameWaitMin = s->_frameWaitMin;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
