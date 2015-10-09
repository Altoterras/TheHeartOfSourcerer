/***********************************************************************//**
 *	Game.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/06/03.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_BODY_GAME_H_
#define _SRCR_BODY_GAME_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../etk/body/EtkBody.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS
	class Camera;
	class ExecRes;
	class ExecCtx;
	class FedTaskSys;
	class Font;
	class IdManager;
	class Keyboard;
	class PerformanceMeter;
	class ResourcePool;
	class Serializer;
	class Tfw;
	class TouchPanel;
	namespace smenu
	{
		class Menu;
	}
TFW_END_NS
using namespace smenu;

ETK_BEGIN_NS
	class SorSgm;
ETK_END_NS

SRCR_BEGIN_NS

class AutoDecorationEffect;
class Conductor;
class Confront;
class EventSys;
class GameFontSet;
class GameMode;
class GameNotifier;
class GaugeAnimEffect;
class GameGui;
class MagicSys;
class Map;
class MoveMap;
class MyParty;
class Party;
class QuestManager;
class SaveSys;
class ScoreBase;
class SoumaScript;
class SpiritManager;
class StoryManager;
class TrophyBase;
class Unit;
class UnitManager;

/*---------------------------------------------------------------------*//**
 *	ゲーム アプリケーション統括
 *
**//*---------------------------------------------------------------------*/
class Game : public EtkBody
{
	//======================================================================
	// 定数
public:
	// ゲーム開始種別
	enum EnterGameKind
	{
		EGKIND_NEW,			// ニューゲーム
		EGKIND_RESTART,		// ストーリーを最初からやり直す
		EGKIND_LOAD,		// ロードゲーム
		EGKIND_DEBUG,		// デバッグ
	};
	
	// ゲームオプションフラグ
	static const u8		OPTF_SOUMA_DEFAULT_LANG_JS	= 0x01;		// デフォルトソウマ言語 JS
	static const u8		OPTF_ARIA_WINDOUW_RAW_SRC	= 0x02;		// 詠唱ウインドウは生ソース表示
	static const u8		OPTF_AUTO_FLOW_MOVIE_MSG	= 0x04;		// ムービー時の台詞を待たずに自動的に流す
	
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	static inline Game*				getGame()						{	return (Game*)getBody();	}
	
	inline SorSgm*					getSceneGraph() const			{	return (SorSgm*)getSceneGraphManager();	}
	inline GameFontSet*				getFontSet() const				{	return _fontset;			}
	inline SaveSys*					getSaveSys() const				{	return _svsys;				}
	inline EventSys*				getEventSys() const				{	return _evsys;				}
	inline Map*						getMap() const					{	return _map;				}
	inline SpiritManager*			getSpiritManager() const		{	return _sprtmng;			}
	inline UnitManager*				getUnitManager() const			{	return _unitmng;			}
	inline GameMode*				getGameMode() const				{	return _gmode;				}
	inline GameNotifier*			getGameNotifier() const			{	return (GameNotifier*)_notif;	}
	inline Conductor*				getConductor() const			{	return _conductor;			}
	inline GameGui*					getGui() const					{	return (GameGui*)_gui;		}
	inline GaugeAnimEffect*			getGaugeAnimEffect() const		{	return _ggaeff;				}
	inline AutoDecorationEffect*	getAutoDecorationEffect() const	{	return _adeff;				}
	inline MagicSys*				getMagicSys() const				{	return _mgcsys;				}
	inline MoveMap*					getMoveMap() const				{	return _mvmap;				}
	inline SoumaScript*				getSoumaScript() const			{	return _script;				}
	inline StoryManager*			getStoryManager() const			{	return _strymng;			}
	inline QuestManager*			getQuestManager() const			{	return _qsmng;				}
	inline Confront*				getConfront() const				{	return _confront;			}
	inline ScoreBase*				getScore() const				{	return _score;				}
	inline TrophyBase*				getTrophy() const				{	return _trophy;				}

	// 標準スクリーン幅を得る
	f32 getStandardWidth() const { return 480; }
	// 標準スクリーン高さを得る
	f32 getStandardHeight() const { return 320; }
	// 論理スクリーン幅を得る
	f32 getLogicalWidth() const { return _widthLgc; }
	// 論理スクリーン高さを得る
	f32 getLogicalHeight() const { return _heightLgc; }
	// ワイド解像度で動作中かを得る
	bool isWideResolution() const;
	// オプションフラグを得る
	inline u8 getOptionFlags() const { return _optflags; }
	// マルチタスキングモードかどうかを得る
	bool isMultiTasking() const;

	// PC パーティを得る
	inline Party* getMyParty() const { return (Party*)_partyMy; }
	inline Party* getGameMyParty() const { return (Party*)_partyMy; }
	// デフォルトカメラを得る
	Camera* getDefaultCamera() const { return _camDefault; }

	// 対決モードかを得る
	bool isConfront() const;
	// アニメーション補間禁止中かを得る
	inline bool isNoAnimInterpolation() const { return _isNoAnimIntp; }

	// デフォルトカメラを設定する
	void setDefaultCamera(Camera* cam) { _camDefault = cam; }
	// オプションフラグを設定する
	inline void setOptionFlags(u8 optflags, bool on) { TFW_SET_FLAG(_optflags, optflags, on); }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void setLangId(etk::LangId langid);
	void resizeView(f32 width, f32 height);
	bool enterGame(EnterGameKind egkind, s32 svdid, s32 storyid);
	s32 startNoAnimInterpolation();
	void endNoAnimInterpolation(s32 hintCtrl);
	void changeScreenSizeMode(bool isScale);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Game();
	~Game();

	bool init(f32 widthLogical, f32 heightLogical, f32 widthView, f32 heightView, f32 scaleCoord, u32 dispflags, f32 spfRender, f32 spfLogic, LangId langidDefault, StartBodyCase sbcase, u32 bcflags, OsDepMod* osdep, PsnsBase* psns);
	void cleanup(TerminateBodyCase tbcase);

	bool serialize(Serializer* ser) const;
	bool deserialize(Serializer* ser);

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
	EtkNotifier* makeNotifier();
	Test* makeTest(f32 spfRender, f32 spfLogic);
	Font* acquireScreenTraceFont();
	void normalizeLogicalScreenSize();

private:
	void updateFrame_exec(const ExecCtx* ec);
	void updateFrame_render(const RenderCtx* rc, Renderer* rdr);
	void exec(ExecRes* res, const ExecCtx* ec);

	//======================================================================
	// 変数
private:
	// 内包システム
	GameFontSet* _fontset;
	SaveSys* _svsys;
	ScoreBase* _score;
	TrophyBase* _trophy;
	EventSys* _evsys;
	Map* _map;
	SpiritManager* _sprtmng;
	UnitManager* _unitmng;
	GameMode* _gmode;
	Conductor* _conductor;
	Confront* _confront;
	GaugeAnimEffect* _ggaeff;
	AutoDecorationEffect* _adeff;
	MagicSys* _mgcsys;
	MoveMap* _mvmap;
	SoumaScript* _script;
	StoryManager* _strymng;
	QuestManager* _qsmng;
	MyParty* _partyMy;				// PC パーティ

	// 外部システム
	Camera* _camDefault;

	// 作業データ
	f32 _widthLgc;					// 論理スクリーン幅
	f32 _heightLgc;					// 論理スクリーン高さ
	f32 _widthLgcWide;				// ワイド時の論理スクリーン幅
	f32 _widthLgcNoWide;			// 非ワイド時の論理スクリーン幅
	f32 _widthLgcReal;				// 実論理スクリーン幅
	f32 _heightLgcReal;				// 実論理スクリーン高さ
	bool _isNoAnimIntp;				// アニメーション補間を禁止する
	s32 _hintNoAnimIntpCtrl;		// アニメーション補間禁止の制御ヒント値
	s16 _cntScreenSizeMode;			// スクリーンサイズ変更カウンタ
	u8 _optflags;					// オプションフラグ
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_BODY_GAME_H_
