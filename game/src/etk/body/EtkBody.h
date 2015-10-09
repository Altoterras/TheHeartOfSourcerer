/***********************************************************************//**
 *  EtkBody.h
 *  Envelopment Terras Kit
 *
 *  Created by Ryoutarou Kishi on 2012/05/22.
 *  Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ETK_BODY_ETK_BODY_H_
#define _ETK_BODY_ETK_BODY_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../tfw/Tfw.h"

////////////////////////////////////////////////////////////////////////////
// マクロ

#if defined(_DEBUG)
	#define PLOT_PM(col1_, col2_)	EtkBody::getBody()->plotPerformanceMeter((col1_), (col2_));
#else
	#define PLOT_PM(col1_, col2_)	//
#endif

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class FedTaskSys;
class Font;
class IdManager;
class Keyboard;
class MotionController;
class PsnsBase;
class ResourcePool;
class ScreenTrace;
class SgManager;
class StopWatch;
class Tfw;
class TouchPanel;

TFW_END_NS

ETK_BEGIN_NS

class EtkGui;
class EtkNotifier;
class OsDepMod;
class ParticleSys;
class PauseScreen;
class ScreenEffect;
class SoundMngr;
class Test;

/*---------------------------------------------------------------------*//**
 *	標準ボディ基底クラス
 *
**//*---------------------------------------------------------------------*/
class EtkBody
{
	//======================================================================
	// 定数
public:
	// ボディ開始時のケース
	enum StartBodyCase
	{
		SBCASE_OPEN,		// 通常開始
		SBCASE_ENTERFG,		// マルチタスク時にフォアグラウンドに入る
	};
	
	// ボディ終了時のケース
	enum TerminateBodyCase
	{
		TBCASE_CLOSE,		// 通常終了
		TBCASE_ENTERBG,		// マルチタスク時にバックグラウンドに入る
	};

	// ボディ生成フラグ
	static const u32	BCF_HD_RESOLUTION		= 0x00000001;	// 高解像度版
	static const u32	BCF_RD_RESOLUTION		= 0x00000002;	// 網膜解像度版
///	static const u32	BCF_WIDE_DISPLAY		= 0x00000010;	// ワイド(16：9)画面
	static const u32	BCF_NO_ROT_DISPLAY		= 0x00000020;	// 画面回転に反応しない
	static const u32	BCF_SAVE_RESOURCE_RUN	= 0x00001000;	// 省リソース動作モード
	static const u32	BCF_BOOT_PASTE_SAVES	= 0x00010000;	// データ引き継ぎとして開始
	static const u32	BCF_DEBUG_DOUBLE_FRAME	= 0x01000000;	// ２倍フレーム（デバッグ用）
	
	// 解像度フラグ
	static const u8		RSLF_HD					= 0x02;			// 高解像度
	static const u8		RSLF_RD					= 0x04;			// Retina 解像度
///	static const u8		RSLF_WD					= 0x08;			// ワイド解像度

	// 拡張フラグ
	static const u8		EXF_SAVE_RESOURCE_RUN	= 0x01;			// 省リソース動作モード

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 各システムの取得
	inline Tfw* getTfw() const { return _tfw; }
	inline TouchPanel* getTouchPanel() const { return _touchp; }
	inline Keyboard* getKeyboard() const { return _keybrd; }
	inline MotionController* getMotionController() const { return _moctrlr;	}
	SgManager* getSceneGraphManager() const { return _tfw->getSgManager(); }
	ResourcePool* getResourcePool() const { return _tfw->getResourcePool(); }
	IdManager* getIdManager() const;
	inline ScreenEffect* getSceneScreenEffect() const { return _screffScn; }
	inline ScreenEffect* getWholeScreenEffect() const { return _screffWhl; }
	inline ParticleSys* getParticleSys() const { return _ptclsys; }
	inline SoundMngr* getSoundManager() const { return _sndmng; }
	inline ScreenTrace* getScreenTrace() const { return _scrtrc; }
	inline FedTaskSys* getTaskSys() const { return _tasksys; }
	inline EtkGui* getGuiManager() const { return _gui; }
	inline OsDepMod* getOsDepMod() const { return _osdepRef; }
	inline PsnsBase* getPsns() const { return _psnsRef; }
#if defined(_DEBUG)
	inline Test* getTest() const { return _test; }
#endif
	static inline EtkBody* getBody() { return _body; }

	// 標準スクリーン幅を得る
	virtual f32 getStandardWidth() const = 0;
	// 標準スクリーン高さを得る
	virtual f32 getStandardHeight() const = 0;
	// 論理スクリーン幅を得る
	virtual f32 getLogicalWidth() const = 0;
	// 論理スクリーン高さを得る
	virtual f32 getLogicalHeight() const = 0;
	// 実スクリーン幅を得る
	f32 getScreenWidth() const;
	// 実スフリーン高さを得る
	f32 getScreenHeight() const;
	// 言語 ID を得る
	inline LangId getLangId() const { return _langid; }
	// 2D 解像描画比を得る
	inline f32 get2drRate() const { return _scaleResoInv; }
	// 解像度フラグを得る
	inline u8 getResolutionFlags() const { return _rslflags; }
	// 拡張フラグを得る
	inline u8 getExFlags() const { return _exflags; }
	// 基準フレームレートを得る
	inline f32 getBasicFrameRate() const { return _spfBase; }
	// 3D 描画スキップ中かを得る
	inline bool isSkip3d() const { return _isSkip3d; }
	// マルチタスキングモードかどうかを得る
	virtual bool isMultiTasking() const = 0;
	// バックグラウンド中かを得る
	inline bool isBackground() const { return _isBackground; }
	// 終了要求中かを得る
	inline bool isExitRequest() const { return _isExitReq; }

	// ストップウォッチを設定する
	inline void setStopWatch(StopWatch* swRef) { _swRef= swRef; }
	// 3D 描画スキップを設定する
	inline void setSkip3d(bool isSkip) { _isSkip3d = isSkip; }
	// 時間がかかるフレームであることを設定する
	inline void setWaitFrame() { _isWaitFrame = true; }
	// 終了要求を設定する
	inline void setExitRequest() { _isExitReq = true; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual void setLangId(LangId langid);
	virtual void resizeView(f32 width, f32 height);
	virtual void enterBackground();
	virtual void leaveBackground();
	virtual void updateFrame(f32 frameDelta);
	virtual void renderFrame();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EtkBody();
	virtual ~EtkBody();
	virtual bool init(f32 widthLogical, f32 heightLogical, f32 widthView, f32 heightView, f32 scaleCoord, u32 dispflags, f32 spfRender, f32 spfLogic, LangId langidDefault, StartBodyCase sbcase, u32 bcflags, OsDepMod* osdep, PsnsBase* psns);
	virtual void cleanup(TerminateBodyCase tbcase);

	void plotPerformanceMeter(u32 colorCategory, u32 colorDetail);

protected:
	virtual void prepareUpdateFrameContext(f32 frameDelta);
	virtual void execFrame(const ExecCtx* ec);
	virtual void execPauseFrame(const ExecCtx* ec);
	virtual void renderFrame(const RenderCtx* rc);
	virtual ExecCtx* makeExecCtx() = 0;
	virtual RenderCtx* makeRenderCtx() = 0;
	virtual PauseScreen* makePauseScreen() = 0;
	virtual ParticleSys* makeParticleSys() = 0;
	virtual SgManager* makeSceneGraphManager() = 0;
	virtual EtkGui* makeGui() = 0;
	virtual EtkNotifier* makeNotifier() = 0;
	virtual Test* makeTest(f32 spfRender, f32 spfLogic) = 0;
	virtual Font* acquireScreenTraceFont() = 0;

	//======================================================================
	// 変数
protected:
	// 内包システム
	Tfw* _tfw;
	TouchPanel* _touchp;
	Keyboard* _keybrd;
	MotionController* _moctrlr;
	PauseScreen* _pausescr;
	ScreenEffect* _screffScn;
	ScreenEffect* _screffWhl;
	ParticleSys* _ptclsys;
	SoundMngr* _sndmng;
	ScreenTrace* _scrtrc;
	FedTaskSys* _tasksys;		// 制御・描画用タスクシステム．現在主に 2D 描画の順序管理に使用
	EtkGui* _gui;
	EtkNotifier* _notif;
#if defined(_DEBUG)
	Test* _test;
#endif

	// 外部システム
	OsDepMod* _osdepRef;
	PsnsBase* _psnsRef;
	StopWatch* _swRef;

	// 作業データ
	LangId _langid;				// 言語モード
	f32 _scaleResoInv;			// 2D 描画用の解像度逆比
	u8 _rslflags;				// 解像度フラグ
	u8 _exflags;				// 拡張フラグ
	f32 _spfBase;				// 基準フレームレート
	ExecCtx* _ecUpdateFrame;	// フレーム更新用のフレーム制御コンテキスト
	RenderCtx* _rcUpdateFrame;	// フレーム更新用のフレーム描画コンテキスト
	bool _isSkip3d;				// 3D 関連の処理をスキップ（メニューやムービー再生時用）
	bool _isWaitFrame;			// 読み込み待ちなどで必然的に時間がかかるフレームである
	bool _isBackground;			// アプリがバックグランドである
	bool _isExitReq;			// アプリ終了要求
	u32 _cntBodyExec;			// 汎用ボディカウンタ（フレームカウンタではない）
	f32 _frameBodyElapsed;		// ボディ経過フレームカウンタ
#if defined(	_DEBUG)
	bool _isDoubleFrameDebug;	// ２倍フレームデバッグ
#endif

	// 自己参照
	static EtkBody* _body;
};

ETK_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ETK_BODY_ETK_BODY_H_
