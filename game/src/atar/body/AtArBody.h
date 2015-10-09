/***********************************************************************//**
 *	AtArBody.h
 *	Enlight AtAr Application
 *
 *	Created by Ryoutarou Kishi on 2012/11/13.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ATAR_BODY_AT_AR_BODY_H_
#define _ATAR_BODY_AT_AR_BODY_H_

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
	class FrameScene;
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

ATAR_BEGIN_NS

class AtArFontSet;
class AtArGui;
class LoadingScreen;
class MaterialManager;
class MovieFileTextureIos;
class TorchIos;
class VideoCameraIos;

/*---------------------------------------------------------------------*//**
 *	アプリケーション統括
 *
**//*---------------------------------------------------------------------*/
class AtArBody : public EtkBody
{
	//======================================================================
	// 定数
public:
	// 生成フラグ
	static const u32	GCF_HD_RESOLUTION		= 0x00000001;	// 高解像度版
	static const u32	GCF_RD_RESOLUTION		= 0x00000002;	// 網膜解像度版
	static const u32	GCF_BOOT_PASTE_SAVES	= 0x00010000;	// データ引き継ぎとして開始
	static const u32	GCF_DEBUG_DOUBLE_FRAME	= 0x01000000;	// ２倍フレーム（デバッグ用）
	
	// シーン
	enum Scene
	{
		SCENE_TITLE,
		SCENE_AR,
		SCENE_MOVIE,
		NUM_SCENE
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// プロパティ
public:
	inline AtArFontSet* getFontSet() const { return _fontset; }
	inline AtArGui* getGui() const { return (AtArGui*)_gui; }
	inline MaterialManager* getMaterialManager() const { return _mtrlmng; }
	inline MovieFileTextureIos* getMovieFileTexture() const { return _mtex; }
	inline TorchIos* getTorch() const { return _torch; }
	inline VideoCameraIos* getVideoCamera() const { return _vcam; }
	inline LoadingScreen* getLoadingScreen() const { return _ldscr;	}
	
	// 標準スクリーン幅を得る
	f32 getStandardWidth() const { return 480.0f; }
	// 標準スクリーン高さを得る
	f32 getStandardHeight() const { return 320.0f; }
	// 論理スクリーン幅を得る
	f32 getLogicalWidth() const { return 480.0f; }
	// 論理スクリーン高さを得る
	f32 getLogicalHeight() const { return 320.0f; }
	// オプションフラグを得る
	inline u8 getOptionFlags() const { return _optflags; }
	// マルチタスキングモードかどうかを得る
	bool isMultiTasking() const { return false; }

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
	AtArBody();
	~AtArBody();

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
	AtArFontSet* _fontset;
	FrameScene* _fsScenes[NUM_SCENE];
	MaterialManager* _mtrlmng;			// 素材管理
	MovieFileTextureIos* _mtex;			// ムービーテクスチャ
	TorchIos* _torch;					// 懐中電灯
	VideoCameraIos* _vcam;				// ビデオカメラ
	LoadingScreen* _ldscr;				// ローディング画面

	// 外部システム
	Camera* _camDefault;

	// 作業データ
	Scene _scene;
	u8 _optflags;				// オプションフラグ
};

ATAR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ATAR_BODY_AT_AR_BODY_H_
