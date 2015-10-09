/***********************************************************************//**
 *	QsEmp_Fixed_AchievePerform.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/12/03.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "QsEmp_Fixed_AchievePerform.h"

// Friends
#include "../QsEmpType.h"
#include "../../../common/SimpleSprite.h"
#include "../../../event/EventSys.h"
#include "../../../event/matter/EventMatterPresvParam.h"
#include "../../../file/GameFileIdDef.h"
#include "../../../body/Game.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../quest/QuestArticle.h"
#include "../../../quest/QuestDef.h"
#include "../../../quest/QuestManager.h"
#include "../../../sound/GameSoundDef.h"

// External
#include "../../../../etk/effect/ScreenEffect.h"
#include "../../../../etk/sound/SoundMngr.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define MSGWND_CTRL_HINT	675432

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// QsEmp_Fixed_AchievePerform メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
void QsEmp_Fixed_AchievePerform::begin(u32 param)
{
	QsEmp::begin(param);

	// スプライト作成
	_sprite = new SimpleSprite(); ASSERT(_sprite != 0L);
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void QsEmp_Fixed_AchievePerform::end()
{
	// スプライト削除
	if(_sprite != 0L)
	{
		_sprite->destroy();
		delete _sprite;
		_sprite = 0L;
	}

	QsEmp::end();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
QsEmp_Fixed_AchievePerform::QsEmp_Fixed_AchievePerform()
	: QsEmp(QSEMPTYPE_ACHIEVE_PERFORM)
	, _sprite(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
QsEmp_Fixed_AchievePerform::~QsEmp_Fixed_AchievePerform()
{
	ASSERT(_sprite == 0L);
}

/*---------------------------------------------------------------------*//**
	フレーム処理実装
**//*---------------------------------------------------------------------*/
void QsEmp_Fixed_AchievePerform::execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun)
{
	GameGui* gui = Game::getGame()->getGui(); ASSERT(gui != 0L);

	switch(getStep())
	{
	case STEP_WAIT_DELAY:
		if(frameBegun >= 60.0f)
		{
			// 次のステップへ
			advanceStep();
		}
		break;
	case STEP_WAIT_BEGIN:		// 継続監視
		if(!getOwner()->checkBegunOtherMatter(this, true))	// 自身以外のイベントが起動していない
		{
			// 次のステップへ
			advanceStep();

			// スプライトを読み込む
			_sprite->create(FILEID_CONV_LANG(QUESTEVENT_SPRITE_QUESTACHIEVE_EN_PNG), 0, true, 0x000000ff, true);

			// SE 再生
			Game::getGame()->getSoundManager()->playUiSe(GameSoundDef::SE_QUEST_ADV, false);
		}
		break;
	case STEP_WAIT_EFFECT:
		if(_sprite->isEnd())
		{
			// クエスト条件達成
			achiveQuest();

			end();
		}
		break;
	}

	if((_sprite != 0L) && _sprite->isValid()) { _sprite->exec(ec); }
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void QsEmp_Fixed_AchievePerform::draw(const RenderCtx* rc)
{
	switch(getStep())
	{
	case STEP_WAIT_EFFECT:
		if(_sprite->isValid())
		{
			_sprite->draw(rc);
		}
		break;
	}
}

/*---------------------------------------------------------------------*//**
	クエスト条件達成
**//*---------------------------------------------------------------------*/
void QsEmp_Fixed_AchievePerform::achiveQuest()
{
	QuestManager* qstmng = Game::getGame()->getQuestManager();
	if(qstmng == 0L)										{	return;	}

	EventMatterPresvParam* empp = reinterpret_cast<EventMatterPresvParam*>(this->getPreserveParam());
	if(empp == 0L)											{	return;	}
	if(empp->_kind != EventMatterPresvParam::KIND_QUEST)	{	return;	}

	QuestArticle* qsta = (QuestArticle*)empp->_d;
	if(qsta == 0L)											{	return;	}

	qstmng->achieveQuest(qsta->getDef()->getQuestId());
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
