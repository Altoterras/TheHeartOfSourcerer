/***********************************************************************//**
 *	PsnsIos.h
 *	Enlight iOS Application
 *	
 *	Created by Ryoutarou Kishi on 2012/04/14.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _IOS_PSNS_IOS_H_
#define _IOS_PSNS_IOS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../tfw/psns/PsnsBase.h"

#import "MainViewController.h"

////////////////////////////////////////////////////////////////////////////
// クラス

/*---------------------------------------------------------------------*//**
 *	iOS プラットフォーム ソーシャル ネットワーク システム
 *	
**//*---------------------------------------------------------------------*/
class PsnsIos : public tfw::PsnsBase
{
	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	達成情報
	 *	
	**//*-----------------------------------------------------------------*/
	class AchievementInfo
	{
	public:
		s32 _itp;					// 対応するTrophy インデックス
		const char* _aid;			// GameCenter の ID

		AchievementInfo(s32 itp, const char* aid) : _itp(itp), _aid(aid) {}
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// プロパティ
public:
	// ゲームセンター操作子の参照をセットする
	inline void setMainViewController(MainViewController* vcMainRef) { _vcMainRef = vcMainRef; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool getCurrentPlayerId(tfw::VcString* idstr) const;
	bool updateScore(u32 score);
	bool updateTrophy(s32 itp, u8 rate);
	bool resetTrophies();
	bool isValidUserAccount();
	bool showPsnsScreen(ScreenKind sk);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	PsnsIos() : _vcMainRef(0L) {}
	
private:
	bool isValidGameCenter() const;
	const AchievementInfo* findAchvInfo(s32 itp) const;

	//======================================================================
	// メンバ変数
private:
	MainViewController* _vcMainRef;
};

////////////////////////////////////////////////////////////////////////////

#endif	// _IOS_PSNS_IOS_H_