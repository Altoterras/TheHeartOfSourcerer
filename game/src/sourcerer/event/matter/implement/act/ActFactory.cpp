/***********************************************************************//**
 *	ActFactory.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/13.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ActFactory.h"

// Friends
#include "implement/AnimChg.h"
#include "implement/AppearParticle.h"
#include "implement/Call.h"
#include "implement/CamConf.h"
#include "implement/ChangeMap.h"
#include "implement/ChangeStat.h"
#include "implement/ChangeStory.h"
#include "implement/EnterConfront.h"
#include "implement/Flag.h"
#include "implement/GetThing.h"
#include "implement/If.h"
#include "implement/IntervalSave.h"
#include "implement/Mcut.h"
#include "implement/Msg.h"
#include "implement/MsgNextPage.h"
#include "implement/ObtainCtrl.h"
#include "implement/PlayMusic.h"
#include "implement/PlaySe.h"
#include "implement/Prog.h"
#include "implement/Rtmv.h"
#include "implement/ScoreUp.h"
#include "implement/ScrEff.h"
#include "implement/ScrMode.h"
#include "implement/SetPos.h"
#include "implement/ShadowCtrl.h"
#include "implement/ShowObj.h"
#include "implement/Sprite.h"
#include "implement/TexFacial.h"
#include "implement/ValFader.h"

// External

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ActFactory メソッド

/*---------------------------------------------------------------------*//**
	Act 作成
**//*---------------------------------------------------------------------*/
Act* ActFactory::makeAct(const VcString* nameAct)
{
	Act* act = 0L;

	if(nameAct->equals("AnimChg"))			{	act = new AnimChg();		}	// アニメーション変更
	else if(nameAct->equals("Particle"))	{	act = new AppearParticle();	}	// パーティクル出現
	else if(nameAct->equals("Call"))		{	act = new Call();			}	// イベント呼び出し
	else if(nameAct->equals("CamConf"))		{	act = new CamConf();		}	// カメラ設定
	else if(nameAct->equals("ChangeMap"))	{	act = new ChangeMap();		}	// マップ変更
	else if(nameAct->equals("ChangeStat"))	{	act = new ChangeStat();		}	// ステータス変更
	else if(nameAct->equals("ChangeStory"))	{	act = new ChangeStory();	}	// ストーリー変更
	else if(nameAct->equals("Confront"))	{	act = new EnterConfront();	}	// 戦闘に入る
	else if(nameAct->equals("Flag"))		{	act = new Flag();			}	// フラグ制御
	else if(nameAct->equals("GetThing"))	{	act = new GetThing();		}	// アイテムなどの取得
	else if(nameAct->equals("If"))			{	act = new If();				}	// 条件分岐 If
	else if(nameAct->equals("Mcut"))		{	act = new Mcut();			}	// ムービーカット定義
	else if(nameAct->equals("Msg"))			{	act = new Msg();			}	// メッセージ表示
	else if(nameAct->equals("MsgNxtPg"))	{	act = new MsgNextPage();	}	// メッセージを次のページへ送る
	else if(nameAct->equals("ObtainCtrl"))	{	act = new ObtainCtrl();		}	// 制御権を取得する
	else if(nameAct->equals("Music"))		{	act = new PlayMusic();		}	// BGM 再生
	else if(nameAct->equals("Save"))		{	act = new IntervalSave();	}	// 途中セーブ
	else if(nameAct->equals("Se"))			{	act = new PlaySe();			}	// SE 再生
	else if(nameAct->equals("Prog"))		{	act = new Prog();			}	// カスタムプログラム実行
	else if(nameAct->equals("Rtmv"))		{	act = new Rtmv();			}	// リアルタイムムービー
	else if(nameAct->equals("Score"))		{	act = new ScoreUp();		}	// スコアアップ
	else if(nameAct->equals("ScrEff"))		{	act = new ScrEff();			}	// スクリーンエフェクト
	else if(nameAct->equals("ScrMode"))		{	act = new ScrMode();		}	// スクリーンモード
	else if(nameAct->equals("SetPos"))		{	act = new SetPos();			}	// 姿勢変更
	else if(nameAct->equals("ShadowCtrl"))	{	act = new ShadowCtrl();		}	// 影制御
	else if(nameAct->equals("ShowObj"))		{	act = new ShowObj();		}	// オブジェクト表示／非表示
	else if(nameAct->equals("Sprite"))		{	act = new Sprite();			}	// スプライト表示
	else if(nameAct->equals("TexFacial"))	{	act = new TexFacial();		}	// テクスチャフェイシャル
	else if(nameAct->equals("ValFader"))	{	act = new ValFader();		}	// バリューフェーダー
	
	return act;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
