/***********************************************************************//**
 *	QuestDef.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/14.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_QUEST_QUEST_DEF_H_
#define _SRCR_QUEST_QUEST_DEF_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class FileBase;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	クエスト定義
 *
**//*---------------------------------------------------------------------*/
class QuestDef
{
	//======================================================================
	// 定数
public:
	// 達成種別
	enum AchievementKind
	{
		ACHVKIND_NULL,
		ACHVKIND_CUSTOM,
		ACHVKIND_HAND_OVER_ITEM,
	};

	// 報酬種別
	enum RewardKind
	{
		REWKIND_NULL,
		REWKIND_CUSTOM,		// カスタム
		REWKIND_EXP,		// 経験値
		REWKIND_ITEM,		// アイテム
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// クエスト ID を得る
	inline u16 getQuestId() const { return _questid; }

	// 起動条件、最小ストーリー ID を得る
	inline u16 getBootMinStoryId() const { return _storyidMin; }
	// 起動条件、最大ストーリー ID を得る
	inline u16 getBootMaxStoryId() const { return _storyidMax; }
	// 起動条件、必要クリア済みクエスト ID を得る
	inline u16 getBootClearedQuestId() const { return _questidCleared; }
	// 起動条件、C01 シチュエーションフラグを得る
	inline u16 getBootSituationFlags() const { return _situflagsC01; }
	// 起動条件、C01 シチュエーションフラグ判定を反転させるかを得る
	inline bool isBootSituationFlagsNegative() const { return _isSituflagsC01Negative == 1; }
	// 起動条件、条件一致時の初期起動確率（1～100）を得る
	inline u16 getBootProbabilityFirst() const { return _probabilityFirst; }
	// 起動条件、条件一致時の繰り返し起動確率（1～100）を得る
	inline u16 getBootProbabilityRepeat() const { return _probabilityRepeat; }
	// 起動条件、排他起動グループ ID を得る
	inline u16 getBootXorGroupId() const { return _grpidXor; }

	// 基点、発生マップ ID を得る
	inline u16 getBaseMapId() const { return _mapidBasePos; }
	// 基点、クリアマップ ID を得る
	inline u16 getClearMapId() const { return _mapidClearPos; }
	// 基点、発生キャラ ID を得る
	inline u16 getBaseCharId() const { return _charidBasePos; }
	// 基点、クリアキャラ ID を得る
	inline u16 getClearCharId() const { return _charidClearPos; }
	// 基点、発生制御イベント ID を得る
	inline u16 getBaseCtrlEvid() const { return _ctrlevidBasePos; }
	// 基点、クリア制御イベント ID を得る
	inline u16 getClearCtrlEvid() const { return _ctrlevidClearPos; }

	// 達成、達成種別を得る
	inline AchievementKind getAchievementKind() const { return (AchievementKind)_achvkind; }
	// 達成、達成対象 ID を得る
	inline u16 getAchievementTargetId() const { return _didAchv; }

	// 報酬種別を得る
	inline RewardKind getRewardKind() const { return (RewardKind)_rewkind; }
	// 公開報酬種別を得る
	inline RewardKind getPublicRewardKind() const { return (RewardKind)_rewkindPub; }
	// 報酬値を得る
	inline u16 getRewardValue() const { return _valueReward; }

	// 難易度を得る
	inline u16 getDifficultyLevel() const { return _difflv; }

	// 汎用パラメータ、汎用パラメータ 1 を得る
	inline u16 getGeneralParam1() const { return _param1; }
	// 汎用パラメータ、汎用パラメータ 2 を得る
	inline u16 getGeneralParam2() const { return _param2; }
	// 汎用パラメータ、汎用パラメータ 3 を得る
	inline u16 getGeneralParam3() const { return _param3; }
	// 汎用パラメータ、汎用パラメータ 4 を得る
	inline u16 getGeneralParam4() const { return _param4; }

	// クエスト名テキストを得る
	inline const VcString* getNameText() const { return _strName; }
	// クエスト条件テキストを得る
	inline const VcString* getRequisiteText() const { return _strRequisite; }
	// 報酬テキストを得る
	inline const VcString* getRewardText() const { return _strReward; }
	// 発注者テキストを得る
	inline const VcString* getClientText() const { return _strClient; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	QuestDef();
	~QuestDef();
	bool loadFromDataFile(FileBase* file);
	bool setText(const VcString* strName, const VcString* strCondition, const VcString* strReward, const VcString* strClient);
	void cleanup();

	//======================================================================
	// 変数
private:
	u16	_questid;					// クエスト ID
	u16 _storyidMin;				// ［起動条件］このストーリー ID 以上である必要がある
	u16 _storyidMax;				// ［起動条件］このストーリー ID 以下である必要がある
	u16 _questidCleared;			// ［起動条件］このクエストをクリアしている必要がある
	u16 _situflagsC01;				// ［起動条件］シチュエーションフラグ
	u16 _isSituflagsC01Negative;	// ［起動条件］シチュエーションフラグ判定を反転するかどうか
	u16 _probabilityFirst;			// ［起動条件］条件一致時の初期起動確率（1～100）
	u16 _probabilityRepeat;			// ［起動条件］条件一致時の２回目以降起動確率（1～100）
	u16 _grpidXor;					// ［起動条件］排他起動グループ ID
	u16 _mapidBasePos;				// ［基点］発生マップ ID
	u16 _mapidClearPos;				// ［基点］クリアマップ ID
	u16 _charidBasePos;				// ［基点］発生キャラ ID
	u16 _charidClearPos;			// ［基点］クリアキャラ ID
	u16 _ctrlevidBasePos;			// ［基点］発生制御イベント ID
	u16 _ctrlevidClearPos;			// ［基点］クリア制御イベント ID
	u16 _achvkind;					// ［達成］達成種別
	u16 _didAchv;					// ［達成］達成対象 ID
	u16 _rewkind;					// ［報酬］報酬種別
	u16 _rewkindPub;				// ［報酬］公開報酬種別
	u16 _valueReward;				// ［報酬］報酬値
	u16 _difflv;					// ［その他］公開難易度
	u16 _param1;					// ［汎用パラメータ］汎用パラメータ 1
	u16 _param2;					// ［汎用パラメータ］汎用パラメータ 2
	u16 _param3;					// ［汎用パラメータ］汎用パラメータ 3
	u16 _param4;					// ［汎用パラメータ］汎用パラメータ 4
	VcString* _strName;				// クエスト名
	VcString* _strRequisite;		// クエスト条件
	VcString* _strReward;			// 報酬
	VcString* _strClient;			// 発注者
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_QUEST_QUEST_DEF_H_
