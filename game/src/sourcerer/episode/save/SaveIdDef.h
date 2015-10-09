/***********************************************************************//**
 *	SaveIdDef.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/28.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_SAVE_SAVE_ID_DEF_H_
#define _SRCR_EPISODE_SAVE_SAVE_ID_DEF_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	セーブ ID
 *
**//*---------------------------------------------------------------------*/
class SaveIdDef
{
	//======================================================================
	// 定数
public:
	// ※ここに定義を追加したら、注意深くリスタート時にクリアするフラグを StoryManager::newGame に実装すること
	static const s32 SFID_ST10101_MEET_CORPO					= 1001;		// [St10101] オープニング後にコルポと合流した
	static const s32 SFID_ST10301_CORPO_SAY_OUTHOUSE			= 1002;		// [St10301] コルポが納屋に寄ってくれと言った
	static const s32 SFID_ST10301_READ_CERJA_LETTER				= 1003;		// [St10301] セルジャの手紙を読んだ
	static const s32 SFID_ST10304_TALK_KABAL_BEFORE_REBECCA		= 1004;		// [St10304] レベッカと話す前にカバルと話している
	static const s32 SFID_ST10306_TOOK_CERJA_LETTER				= 1005;		// [St10306] セルジャの手紙を取得している
	static const s32 SFID_ST10401_DONE_YASAN_THANK				= 1006;		// [St10401] コルポのアサ出迎えイベントをデフォルトのものと置き換えて、ヤサンからソウマを受け取るイベントを再生した
	static const s32 SFID_ST10401_CLEAR_VOL01					= 1007;		// [St10401] 上巻（Vol01）をクリアしている
	static const s32 SFID_ST10204_GET_LUMIN_SOUMA_FROM_HERMIT	= 1008;		// [St10204] ハーミットから発光のソウマを得ている
	static const s32 SFID_ST10204_MEET_KOTSUME					= 1009;		// [St10401] コツメと合流している
	static const s32 SFID_ST10401_CLEAR_ASA_EVENT				= 1010;		// [St10204] アサイベントをクリアしている
	static const s32 SFID_ST10304_GET_FIRE_SOUMA_FROM_REBECCA	= 1011;		// [St10304] レベッカと話して火の強化ソウマを得ている
	static const s32 SFID_ST10306_CLEAR_CORPO_EVENT				= 1012;		// [St10306] コルポイベントをクリアしている
	static const s32 SFID_ST10305_ONCE_DONE_REIKA_APPEAR		= 1013;		// [St10305] 一度レイカを出現させている（レイカの勘違いでレイカが消えている）
	static const s32 SFID_ST10203_CORPO_SAY_TOGETHER			= 1014;		// [St10203] アサダウン後コルポ合流時のイベントを見ている
	static const s32 SFID_ST10101_PICKUP_TUTORIAL_STONE			= 1015;		// [St10101] アイテム拾いチュートリアルを見ている

	static const s32 SFID_ST10203_DONE_KABAL_TALK				= 1021;		// [St10203] カバルと会話してアイテムを取得している
	static const s32 SFID_ST10204_DONE_KABAL_TALK				= 1022;		// [St10204] カバルと会話してアイテムを取得している
	static const s32 SFID_ST10401_DONE_KABAL_TALK				= 1023;		// [St10401] カバルと会話してアイテムを取得している
	static const s32 SFID_ST10000_GET_ITEM_SYNTHE_SOUMA			= 1024;		// [St10000] アイテム合成ソウマを得ている
	static const s32 SFID_ST10000_TALK_STONE_COLLECTOR			= 1025;		// [St10000] 石コレクター（タケリ）と話している

	static const s32 SFID_ST10501_START_VOL02					= 1031;		// [St10501] 下巻を開始している
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_SAVE_SAVE_ID_DEF_H_
