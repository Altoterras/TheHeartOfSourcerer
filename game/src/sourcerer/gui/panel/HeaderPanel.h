/***********************************************************************//**
 *	HeaderPanel.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/09.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_PANEL_HEADER_PANEL_H_
#define _SRCR_GUI_PANEL_HEADER_PANEL_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"

#include "../../../tfw/gui/Panel.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Font;

TFW_END_NS

SRCR_BEGIN_NS

class WindowFrame;

/*---------------------------------------------------------------------*//**
 *	ヘッダー パネル
 *
**//*---------------------------------------------------------------------*/
class HeaderPanel : public Panel
{
	//======================================================================
	// 定数
public:
	static const u32	HPF_UCASE_SOURCEEDIT		= 0x00000001;	// ソースエディット時

private:
	enum Button
	{
		BTN_LEFT,
		BTN_RIGHT,
		BTN_FUNC,
		NUM_BTN
	};

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	結果値
	 *
	**//*-----------------------------------------------------------------*/
	class Res
	{
		//==================================================================
		// 定数
	public:
		static const u16 F_CLICK_L		= 0x0001;
		static const u16 F_CLICK_R		= 0x0002;
		static const u16 F_CLICK_F		= 0x0004;
		static const u16 F_CLICK_BODY	= 0x0010;
		static const u16 F_TOUCH_BODY	= 0x0020;

		//==================================================================
		// メソッド

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// アクセサ
	public:
		// 左ボタンをクリックしたかを得る
		inline bool		isClickLeftButton() const		{	return TFW_IS_FLAG(_flags, F_CLICK_L);		}
		// 右ボタンをクリックしたかを得る
		inline bool		isClickRightButton() const		{	return TFW_IS_FLAG(_flags, F_CLICK_R);		}
		// 機能ボタンをクリックしたかを得る
		inline bool		isClickFuncButton() const		{	return TFW_IS_FLAG(_flags, F_CLICK_F);		}
		// ヘッダ自体をクリックしたかを得る
		inline bool		isClickBody() const				{	return TFW_IS_FLAG(_flags, F_CLICK_BODY);	}
		// ヘッダ自体をタップしたかを得る
		inline bool		isTouchBody() const				{	return TFW_IS_FLAG(_flags, F_TOUCH_BODY);	}

		// フラグ設定
		inline void setFlags(u32 flags, bool on)		{ TFW_SET_FLAG(_flags, flags, on);				 }

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// 内部制御
	public:
		Res() : _flags(0) {}
		void reset() { _flags = 0; }

		//==================================================================
		// 変数
	private:
		u16	_flags;
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 結果値を得る
	inline const Res* getResult() const { return &_res; }
	// 左ボタンパネルを得る
	inline Panel* leftButtonPanel() { return _pnlBtn[BTN_LEFT]; }
	// 右ボタンパネルを得る
	inline Panel* rightButtonPanel() { return _pnlBtn[BTN_RIGHT]; }
	// 機能ボタンパネルを得る
	inline Panel* funcButtonPanel() { return _pnlBtn[BTN_FUNC]; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void resetResult();
	void changeGuiString(const VcString* title, const VcString* labelLeft, const VcString* labelRight, const VcString* labelFunc);
	void complyChangeScreenSize();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	HeaderPanel();
	virtual ~HeaderPanel();
	virtual bool create(Font* fontRef, Texture* texRef, const VcString* title, const VcString* labelLeft, const VcString* labelRight, const VcString* labelFunc, u32 hpflags);
	virtual void destroy();
	virtual void exec(ExecRes* res, const ExecCtx* ec);
	virtual void draw(const RenderCtx* rc);

	//======================================================================
	// 変数
private:
	Panel* _pnlBtn[NUM_BTN];
	Res _res;
	u32 _hpflags;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_PANEL_HEADER_PANEL_H_
