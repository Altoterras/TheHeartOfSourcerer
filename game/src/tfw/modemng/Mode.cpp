/***********************************************************************//**
 *	Mode.cpp
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/06/23.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Mode.h"

// Friends
#include "Role.h"
#include "../collection/List.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Mode メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	予約されたモード数を取得
**//*---------------------------------------------------------------------*/
int Mode::getReservedModeNum() const
{
	int cnt = 0;
	for(int i = 0; i < _lenStack; i++)
	{
		if(_modeReserved[i] == 0)
		{
			break;
		}
		cnt++;
	}
	return cnt;
}

/*---------------------------------------------------------------------*//**
	スタックされたモード数を取得
**//*---------------------------------------------------------------------*/
int Mode::getStackedModeNum() const
{
	int cnt = 0;
	for(int i = 0; i < _lenStack; i++)
	{
		if(_modeStack[i] == 0)
		{
			break;
		}
		cnt++;
	}
	return cnt;
}

/*---------------------------------------------------------------------*//**
	一つ前のモードを取得
**//*---------------------------------------------------------------------*/
short Mode::getPrevMode() const
{
	if(_lenStack >= 2)
	{
		return _modeStack[1];
	}
	return MODE_NULL;
}

/*---------------------------------------------------------------------*//**
	汎用パラメータセット
**//*---------------------------------------------------------------------*/
void Mode::setParameter(void* objParamRef)
{
	_objParamRef = objParamRef;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	新規モード設定
**//*---------------------------------------------------------------------*/
void Mode::changeMode(short modeid, bool isReset)
{
	ASSERT(modeid >= 0);
	
	// モードスタックをリセット
	if(isReset)
	{
		for(int i = 0; i < _lenStack; i++)
		{
			_modeStack[i] = MODE_NULL;
		}
		for(int i = 0; i < _lenStack; i++)
		{
			_modeReserved[i] = MODE_NULL;
		}
	}
	
	// モードスタックを保存
	shiftArrayAsc(_modeStack, _lenStack, MODE_NULL);

	// 新規設定
	_modeStack[0] = modeid;

	// モード変更
	changeModeActually(_modeStack[0]);
}

/*---------------------------------------------------------------------*//**
	モードの巻き戻し
**//*---------------------------------------------------------------------*/
void Mode::changePrevMode()
{
	// モードスタックの巻き戻し
	shiftArrayDesc(_modeStack, _lenStack, MODE_NULL);

	// モード変更
	changeModeActually(_modeStack[0]);
}

/*---------------------------------------------------------------------*//**
	モード予約
**//*---------------------------------------------------------------------*/
void Mode::reserveMode(short modeid)
{
	ASSERT(modeid >= 0);
	
	#if 0
		ShiftArrayAsc(_modeReserved, MODE_NULL);
		_modeReserved[0] = mode;
	#else	// ↓試し
		for(int i = 0; i < _lenStack; i++)
		{
			if(_modeReserved[i] == 0)
			{
				_modeReserved[i] = modeid;
				break;
			}
		}
	#endif
}


/*---------------------------------------------------------------------*//**
	モードキャンセル予約
**//*---------------------------------------------------------------------*/
void Mode::reserveModeCancel(short modeidCancel)
{
	ASSERT(modeidCancel > 0);
	
	shiftArrayAsc(_modeReserved, _lenStack, MODE_NULL);
	_modeReserved[0] = (short)(- modeidCancel);	// マイナスの値をセット
}

/*---------------------------------------------------------------------*//**
	予約されたモードの反映
**//*---------------------------------------------------------------------*/
void Mode::changeFromReservedMode()
{
	while(_modeReserved[0] != MODE_NULL)
	{
		if(_modeReserved[0] < 0)	// モードキャンセル
		{
			if(_modeStack[0] == (-_modeReserved[0]))
			{
				// 現在のモードのキャンセルだったら
				changePrevMode();
			}
			else
			{
				// 次の機会まで持ち越し
				break;
			}
		}
		else						// 指定された新規モードセット
		{
			changeMode(_modeReserved[0], false);
		}
		
		shiftArrayDesc(_modeReserved, _lenStack, MODE_NULL);
	}
}

/*---------------------------------------------------------------------*//**
	登録されている次の子モードを予約
**//*---------------------------------------------------------------------*/
short Mode::getNextMode()
{
	if(_listChildMode == 0L)	{	return MODE_NULL;	}

	short modeidNext = MODE_NULL;

	// 現在の子モードから次の子モード ID を選択
	if(_modeCurChild != 0L)
	{
		ListIterator<Mode*> it = _listChildMode->iterator();
		while(it.has())
		{
			Mode* modeWk = it.next();
			if(modeWk->_modeidSelf == _modeCurChild->getSelfModeId())
			{
				if(it.has())
				{
					Mode* modeNext = it.next();
					modeidNext = modeNext->getSelfModeId();
				}
				break;
			}
		}
	}

	// 現在の子モードから見つからなかった場合は、子モードリストの最初の要素を選択
	if(modeidNext == MODE_NULL)
	{
		modeidNext = _listChildMode->getHeadObject()->getSelfModeId();
	}

	return modeidNext;
}

/*---------------------------------------------------------------------*//**
	モード開始
**//*---------------------------------------------------------------------*/
bool Mode::begin(void* objParamRef)
{
	_cntFrame= 1;

	return notifyBegin(objParamRef);
}

/*---------------------------------------------------------------------*//**
	モード終了
**//*---------------------------------------------------------------------*/
void Mode::end(void* objParamRef)
{
	notifyEnd(objParamRef);

	_cntFrame = 0;

	// 子モードも終了
	if(_modeCurChild != 0L)
	{
		if(_modeCurChild->isBegan())	{	_modeCurChild->end(objParamRef);	}
		notifyChildModeEnd(_modeCurChild->getSelfModeId(), _objParamRef);
		_modeCurChild = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	モードの関係を調査する
**//*---------------------------------------------------------------------*/
Mode::Relation Mode::checkRelation(const Mode* modeTrg) const
{
	if(modeTrg != 0L)
	{
		return REL_NULL;
	}
	return checkRelation(modeTrg->getSelfModeId());
}

/*---------------------------------------------------------------------*//**
	モードの関係を調査する
**//*---------------------------------------------------------------------*/
Mode::Relation Mode::checkRelation(short modeidTrg) const
{
	// 子や子孫か
	if(_listChildMode != 0L)
	{
		for(ListIterator<Mode*> it = _listChildMode->iterator(); it.has(); it.next())
		{
			const Mode* modeChild = it.object();
			if(modeChild == 0L)
			{
				continue;
			}
			if(modeChild->getSelfModeId() == modeidTrg)
			{
				return REL_CHILD;
			}
			const Mode* modeDescendant = modeChild->findChildModeDive(modeidTrg);
			if(modeDescendant != 0L)
			{
				return REL_DESCENDANT;
			}
		}
	}

	Mode* modeParent = getParentMode();
	if(modeParent != 0L)
	{
		// 親か
		if(modeParent->getSelfModeId() == modeidTrg)
		{
			return REL_PARENT;
		}

		// 兄弟か
		if(modeParent->_listChildMode != 0L)
		{
			for(ListIterator<Mode*> it = modeParent->_listChildMode->iterator(); it.has(); it.next())
			{
				const Mode* modeParentChild = it.object();
				if(modeParentChild == 0L)
				{
					continue;
				}
				if(modeParentChild->getSelfModeId() == modeidTrg)
				{
					return REL_SIBLING;
				}
			}
		}

		// 先祖か
		while((modeParent = modeParent->getParentMode()) != 0L)
		{
			if(modeParent->getSelfModeId() == modeidTrg)
			{
				return REL_ANCESTOR;
			}
		}
	}

	return REL_NULL;
}

/*---------------------------------------------------------------------*//**
	子モードを探す
**//*---------------------------------------------------------------------*/
Mode* Mode::findChildModeDive(short modeid) const
{
	if(_listChildMode != 0L)
	{
		for(ListIterator<Mode*> it = _listChildMode->iterator(); it.has(); it.next())
		{
			Mode* modeChild = it.object();
			if(modeChild == 0L)
			{
				continue;
			}
			if(modeChild->getSelfModeId() == modeid)
			{
				return modeChild;
			}
			Mode* modeFound = modeChild->findChildModeDive(modeid);
			if(modeFound != 0L)
			{
				return modeFound;
			}
		}
	}

	return 0L;
}

/*---------------------------------------------------------------------*//**
	予約されているかどうかを得る
**//*---------------------------------------------------------------------*/
bool Mode::isReservedMode(bool isDiveChild) const
{
	if(_modeReserved[0] != MODE_NULL)
	{
		return true;
	}

	if((_listChildMode != 0L) && isDiveChild)
	{
		bool isReservedChild = false;
		for(ListIterator<Mode*> it = _listChildMode->iterator(); it.has(); it.next())
		{
			if(it.object()->isReservedMode(true))
			{
				isReservedChild = true;
			}
		}
		return isReservedChild;
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	予約されているモードをクリアする
**//*---------------------------------------------------------------------*/
void Mode::clearReservedMode(bool isDiveChild) const
{
	if(_modeReserved[0] != 0)	// 予約モードがある
	{
		::memset(_modeReserved, 0, sizeof(short) * _lenStack);
	}

	if((_listChildMode != 0L) && isDiveChild)
	{
		for(ListIterator<Mode*> it = _listChildMode->iterator(); it.has(); it.next())
		{
			it.object()->clearReservedMode(true);
		}
	}
}

/*---------------------------------------------------------------------*//**
	子モードの数を得る
**//*---------------------------------------------------------------------*/
s32	Mode::getChildModeNum() const
{
	return (_listChildMode != 0L) ? (s32)_listChildMode->getCount() : 0;
}

/*---------------------------------------------------------------------*//**
	子モードを得る
**//*---------------------------------------------------------------------*/
Mode* Mode::getChildMode(s32 index)
{
	if((_listChildMode == 0L) || ((index < 0) && (index <= (s32)_listChildMode->getCount())))
	{
		return 0L;
	}
	return _listChildMode->getObject(index);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Mode::Mode()
	: _modeStack(0L)
	, _modeReserved(0L)
	, _cntFrame(0)
	, _lenStack(0)
	, _isCreated(false)
	, _isExecRslvRsvd(true)	// デフォルトで setFromReservedMode の自動呼出し
	, _modeidSelf(MODE_NULL)
	, _objParamRef(0L)
	, _listChildMode(0L)
	, _modeCurChild(0L)
	, _modeParent(0L)
	, _roleSelf(0L)
{
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Mode::Mode(int numStack, Role* roleSelf)
	: _modeStack(0L)
	, _modeReserved(0L)
	, _cntFrame(0)
	, _lenStack(0)
	, _isCreated(false)
	, _isExecRslvRsvd(true)	// デフォルトで setFromReservedMode の自動呼出し
	, _modeidSelf(MODE_NULL)
	, _objParamRef(0L)
	, _listChildMode(0L)
	, _modeCurChild(0L)
	, _modeParent(0L)
	, _roleSelf(0L)
	, _isRoleEntr(false)
{
	create(numStack, roleSelf, true);
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Mode::~Mode()
{
	if(_isCreated)	{	destroy();	}

	ASSERT(_modeStack == 0L);
	ASSERT(_modeReserved == 0L);
	ASSERT(_listChildMode == 0L);
	ASSERT(_roleSelf == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool Mode::create()
{
	return create(0, 0L, false);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool Mode::create(int numStack, Role* roleSelf, bool isRoleDlg)
{
	ASSERT(_isCreated == false);

	// 配列
	_lenStack = numStack + 1;
	_modeStack = new short[_lenStack];
	if(_modeStack == 0L)	{	goto Error;	}
	_modeReserved = new short[_lenStack];
	if(_modeReserved == 0L)	{	goto Error;	}
	
	// 配列リセット
	for(int i = 0; i < _lenStack; i++)
	{
		_modeStack[i] = MODE_NULL;
		_modeReserved[i] = MODE_NULL;
	}

	if(roleSelf != 0L)
	{
		_isRoleEntr = isRoleDlg;
		if(_isRoleEntr)	{	if(!roleSelf->create(this))	{	goto Error;	}	}
		_roleSelf = roleSelf;
	}
	
	_isCreated = true;
	return true;

Error:
	_isCreated = true;	// destroy を通すために一時的にフラグを ON にする（マルチスレッド化時は要検討）
	destroy();
	return false;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void Mode::destroy()
{
	ASSERT(_isCreated == true);

	// 配列削除
	delete[] _modeStack;
	delete[] _modeReserved;
	_modeStack = 0L;
	_modeReserved = 0L;
	_lenStack = 0;

	if(_listChildMode != 0L)
	{
		for(ListIterator<Mode*> it = _listChildMode->iterator(); it.has(); it.next())
		{
			Mode* modeChild = it.object();
			modeChild->destroy();
			delete modeChild;
		}
		delete _listChildMode;
		_listChildMode = 0L;
	}

	if(_roleSelf != 0L)
	{
		if(_isRoleEntr)
		{
			_roleSelf->destroy();
			delete _roleSelf;
		}
		_roleSelf = 0L;
	}

	_isCreated = false;
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void Mode::exec(ExecRes* res, const ExecCtx* ec)
{
	// 開始していない場合は抜ける
	if(_cntFrame == 0)		{	return;							}

	// 予約モードの解決
	if(_isExecRslvRsvd)		{	changeFromReservedMode();		}	// changeFromReservedMode の自動呼出し

	// 子モード実行
	if(_modeCurChild != 0L)	{	if(_modeCurChild->isBegan())	{	_modeCurChild->exec(res, ec);	}	}

	// ロール処理
	if(_roleSelf != 0L)		{	_roleSelf->exec(res, ec);		}

	_cntFrame++;
}

/*---------------------------------------------------------------------*//**
	3D 描画
**//*---------------------------------------------------------------------*/
void Mode::render(const RenderCtx* rc)
{
	// 開始していない場合は抜ける
	if(_cntFrame == 0)		{	return;							}

	// 子モード描画
	if(_modeCurChild != 0L)	{	if(_modeCurChild->isBegan())	{	_modeCurChild->render(rc);		}	}

	// ロール描画
	if(_roleSelf != 0L)		{	_roleSelf->render(rc);			}
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void Mode::draw(const RenderCtx* rc)
{
	// 開始していない場合は抜ける
	if(_cntFrame == 0)		{	return;							}

	// モード描画
	if(_modeCurChild != 0L)	{	if(_modeCurChild->isBegan())	{	_modeCurChild->draw(rc);		}	}

	// ロール描画
	if(_roleSelf != 0L)	{	_roleSelf->draw(rc);	}
}

/*---------------------------------------------------------------------*//**
	子モード登録
**//*---------------------------------------------------------------------*/
bool Mode::registerMode(short modeid, Mode* mode)
{
	// モード登録
	if(_listChildMode == 0L)
	{
		_listChildMode = new List<Mode*>(false);
		if(_listChildMode == 0L)	{	goto Error;	}
	}
	mode->_modeParent = this;
	mode->_modeidSelf = modeid;
	_listChildMode->addTail(mode);

	// registerMode(xxx, new XxxMode()); のように未作成の子モードを渡した場合は
	// このメソッドで代わりにオーバーライド用の create を呼ぶ
	// また、失敗した場合は mode を削除するようにする
	if(!mode->_isCreated)
	{
		if(!mode->create())
		{
			goto Error;
		}
	}

	return true;

Error:
	delete mode;
	return false;
}

/*---------------------------------------------------------------------*//**
	モード変更
**//*---------------------------------------------------------------------*/
void Mode::changeModeActually(short modeid)
{
	if(_listChildMode == 0L)	{	return;	}

	Mode* modeTrg = 0L;

	ListIterator<Mode*> it = _listChildMode->iterator();
	while(it.has())
	{
		Mode* modeWk = it.next();
		if(modeWk->_modeidSelf == modeid)
		{
			modeTrg = modeWk;
			break;
		}
	}

	// 既存モードの終了通知
	if(_modeCurChild != 0L)
	{
		if(_modeCurChild->isBegan())	{	_modeCurChild->end(_objParamRef);	}
		notifyChildModeEnd(_modeCurChild->getSelfModeId(), _objParamRef);
		_modeCurChild = 0L;
	}

	// モード変更
	if(modeTrg == 0L)
	{
		_modeCurChild = 0L;
	}
	else
	{
		Mode* modeNew = modeTrg;
		if(modeNew != 0L)
		{
			if(modeNew->begin(_objParamRef))	// 開始通知
			{
				_modeCurChild = modeNew;
				notifyChildModeBegin(_modeCurChild->getSelfModeId(), _objParamRef);
			}
			else
			{
				_modeCurChild = 0L;
			}
		}
	}

	notifyChildModeChanged(modeid, _objParamRef);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Mode　ユーティリティ

/*---------------------------------------------------------------------*//**
	配列の順方向シフト
		[A][B][C][ ][ ]
		↓
		[ ][A][B][C][ ]
**//*---------------------------------------------------------------------*/
void Mode::shiftArrayAsc(short* arr, int len, short valueNull)
{
	for(int i = len - 2; i >= 0; i--)
	{
		arr[i + 1] = arr[i];
	}
	arr[0] = valueNull;
}

/*---------------------------------------------------------------------*//**
	配列の順方向シフト
		[A][B][C][ ][ ]
		↓
		[ ][A][B][C][ ]
**//*---------------------------------------------------------------------*/
void Mode::shiftArrayAsc(int* arr, int len, int valueNull)
{
	for(int i = len - 2; i >= 0; i--)
	{
		arr[i + 1] = arr[i];
	}
	arr[0] = valueNull;
}

/*---------------------------------------------------------------------*//**
	配列の逆方向シフト
		[A][B][C][ ][ ]
		↓
		[B][C][ ][ ][ ]
**//*---------------------------------------------------------------------*/
void Mode::shiftArrayDesc(short* arr, int len, short valueNull)
{
	for(int i = 0; i < len - 1; i++)
	{
		arr[i] = arr[i + 1];
	}
	arr[len - 1] = valueNull;
}

/*---------------------------------------------------------------------*//**
	配列の逆方向シフト
		[A][B][C][ ][ ]
		↓
		[B][C][ ][ ][ ]
**//*---------------------------------------------------------------------*/
void Mode::shiftArrayDesc(int* arr, int len, int valueNull)
{
	for(int i = 0; i < len - 1; i++)
	{
		arr[i] = arr[i + 1];
	}
	arr[len - 1] = valueNull;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
