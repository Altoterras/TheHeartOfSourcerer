/***********************************************************************//**
 *	BasicModelBody.h
 *	Enlight BasicModel Application
 *
 *	Created by Ryoutarou Kishi on 2012/11/13.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _BASICMODEL_BODY_BODY_H_
#define _BASICMODEL_BODY_BODY_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../etk/body/EtkBody.h"

////////////////////////////////////////////////////////////////////////////
// クラス

namespace tfw 
{
	class Camera;
	class ExecRes;
	class ExecCtx;
	class FedTaskSys;
	class Font;
	class IdManager;
	class Keyboard;
	class PerformanceMeter;
	class ResourcePool;
	class ScreenTrace;
	class Serializer;
	class StopWatch;
	class Tfw;
	class TouchPanel;
	namespace smenu
	{
		class Menu;
	}
}
using namespace smenu;

namespace etk
{
	class ParticleSys;
	class SorSgm;
}

namespace game
{
	class SoumaScript;
}
using namespace game;

BASICMODEL_BEGIN_NS

class BasicModelFontSet;
class BasicModelGui;
class ModelViewer;

/*---------------------------------------------------------------------*//**
 *	アプリケーション統括
 *
**//*---------------------------------------------------------------------*/
class BasicModelBody : public EtkBody
{
	//======================================================================
	// 定数
public:
	// 生成フラグ
	static const u32	GCF_HD_RESOLUTION		= 0x00000001;	// 高解像度版
	static const u32	GCF_RD_RESOLUTION		= 0x00000002;	// 網膜解像度版
	static const u32	GCF_BOOT_PASTE_SAVES	= 0x00010000;	// データ引き継ぎとして開始
	static const u32	GCF_DEBUG_DOUBLE_FRAME	= 0x01000000;	// ２倍フレーム（デバッグ用）
	
	// 解像度フラグ
	static const u8		RSLF_HD						= 0x02;		// 高解像度
	static const u8		RSLF_RD						= 0x04;		// 網膜解像度

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// プロパティ
public:
//	SorSgm* getSceneGraphMngr() const;
	inline BasicModelFontSet* getFontSet() const { return _fontset; }
	inline BasicModelGui* getGui() const { return (BasicModelGui*)_gui; }

	// 標準スクリーン幅を得る
	f32 getStandardWidth() const { return 480; }
	// 標準スクリーン高さを得る
	f32 getStandardHeight() const { return 320; }
	// 論理スクリーン幅を得る
	f32 getLogicalWidth() const { return 480; }
	// 論理スクリーン高さを得る
	f32 getLogicalHeight() const { return 320; }
	// オプションフラグを得る
	inline u8 getOptionFlags() const { return _optflags; }
	// マルチタスキングモードかどうかを得る
	bool isMultiTasking() const;

	// デフォルトカメラを得る
	Camera* getDefaultCamera() const { return _camDefault; }

	// デフォルトカメラを設定する
	void setDefaultCamera(Camera* cam) { _camDefault = cam; }
	// オプションフラグを設定する
	inline void setOptionFlags(u8 optflags, bool on) { TFW_SET_FLAG(_optflags, optflags, on); }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	BasicModelBody();
	~BasicModelBody();

	bool init(s32 widthView, s32 heightView, f32 scaleContent, u32 dispflags, f32 spfRender, f32 spfLogic, LangId langidDefault, StartBodyCase sbcase, u32 bcflags, OsDepMod* osdep, PsnsBase* psns);
	void cleanup(TerminateBodyCase tbcase);
	
protected:
	void prepareUpdateFrameContext(f32 frameDelta);
	void execFrame(const ExecCtx* ec);
	void renderFrame(const RenderCtx* rc);
	ExecCtx* makeExecCtx();
	RenderCtx* makeRenderCtx();
	PauseScreen* makePauseScreen();
	ParticleSys* makeParticleSys();
	SgManager* makeSceneGraphManager();
	EtkGui* makeGui();
	Test* makeTest(f32 spfRender, f32 spfLogic);
	Font* acquireScreenTraceFont();

private:
	void updateFrame_exec(const ExecCtx* ec);
	void updateFrame_render(const RenderCtx* rc, Renderer* rdr);
	void exec(ExecRes* res, const ExecCtx* ec);

	//======================================================================
	// 変数
private:
	// 内包システム
	BasicModelFontSet* _fontset;
	ModelViewer* _modelview;

	// 外部システム
	Camera* _camDefault;

	// 作業データ
	u8 _optflags;				// オプションフラグ
};

BASICMODEL_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _BASICMODEL_BODY_BODY_H_
