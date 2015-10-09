/***********************************************************************//**
 *	EsFunctionBox.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2010/05/26.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsFunctionBox.h"

// Friends
#include "EsBytecodeBuilder.h"
#include "EsFunction.h"
#include "EsItNode.h"

// External
#include "../../tfw/collection/List.h"

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EsFunctionBox メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	セットアップ
**//*---------------------------------------------------------------------*/
void EsFunctionBox::setup(EsNameItNode* nodeFunc, EsFunction* objFunc, EsFunctionBox* funcboxParent, EsFunctionBox* funcboxSibling, bool isInLoop, u32 depthNest, u32 bflags)
{
	_node = nodeFunc;
	_obj = objFunc;
	_parent = funcboxParent;
	_sibling = funcboxSibling;
	_isInLoop = isInLoop;
	_depthNest = depthNest;
	_bflags = bflags;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsFunctionBox::EsFunctionBox()
	: _node(0L)
	, _sibling(0L)
	, _child(0L)
	, _parent(0L)
	, _isInLoop(false)
	, _depthNest(0)
	, _bflags(0L)
{
}

/*---------------------------------------------------------------------*//**
	関数解析の後仕上げ処理 ⇒ analyzeFunctions
**//*---------------------------------------------------------------------*/
void EsFunctionBox::postprocFunction(u32* bflags)
{
	// 引数判定
	{
		List<EsFunctionBox*> listQueue;

		// 引数を集める ⇒ FindFunArgs
		pickFuncArgs(this, -1, &listQueue);

		// 集めた関数ボックスについて更に引数処理をする
		ListIterator<EsFunctionBox*> itCur = listQueue.iterator();
		if(itCur.has())
		{
			EsFunctionBox* funcboxWk = itCur.object();
			EsNameItNode* nodeFuncWk = funcboxWk->getFuncNode();
			ASSERT(TFW_IS_FLAG(nodeFuncWk->getDefFlag(), EsNameItNode::DF_FUNCARG));
			EsItNode* nodeFuncBodyWk = nodeFuncWk->getExpressionNode();
			if(nodeFuncBodyWk->getTokenType() == TKN_UPVARS)
			{
				ASSERT(nodeFuncBodyWk->getNodeType() == NT_NAMESET);
				EsAtomList* atomlist = ((EsNameSetItNode*)nodeFuncBodyWk)->namesSet();
				ASSERT(atomlist->getCount() > 0);
				for(int i = 0; i < atomlist->getCount(); i++)
				{
					EsAtomList::EsAtomListEntity* ale = atomlist->getEntity(i);
					EsNameItNode* nodeLexdepDef = ale->getDefName()->findFinalDefNode();	// ⇒ JSDefinition *lexdep = ALE_DEFN(ale)->resolve();
					if(	!nodeLexdepDef->isFreeVar() &&
						!TFW_IS_FLAG(nodeLexdepDef->getDefFlag(), EsNameItNode::DF_FUNCARG) &&
						TFW_IS_FLAG(nodeLexdepDef->getDefFlag(), EsNameItNode::DF_EXTYPE_FUNC)	)
					{
						nodeLexdepDef->setFuncArg();
						EsFunctionBox* funcboxLexdep = nodeLexdepDef->getFunctionBox();
						listQueue.addTail(funcboxLexdep);

						if(funcboxLexdep->getChild() != 0L)
						{
							pickFuncArgs(funcboxLexdep->getChild(), funcboxLexdep->getNestDepth(), &listQueue);
						}
					}
				}
			}
			itCur.next();
		}
	}

	// 関数の種類を決定する
	decideFuncKind(bflags);
}

/*---------------------------------------------------------------------*//**
	関数の引数を見つけ、フラグなどを設定し、リストに追加する ⇒ FindFunArgs
**//*---------------------------------------------------------------------*/
u32 EsFunctionBox::pickFuncArgs(EsFunctionBox* funcbox, s32 depth, List<EsFunctionBox*>* listQueue)
{
	u32 nestlvSkipMinAll = EsUpvarInfo::DEPTH_FREE_STATIC;	// ⇒ FREE_STATIC_LEVEL

	EsFunctionBox* funcboxWk = funcbox;
	while(funcboxWk != 0L)
	{
		EsNameItNode* nodeFuncWk = funcbox->getFuncNode();
		s32 depthWk = depth;

		// 引数フラグを立てる
		if(TFW_IS_FLAG(funcboxWk->getBuildFlag(), EsBytecodeBuilder::BF_FUNC_NEED_CALLOBJ))
		{
			nodeFuncWk->setFuncArg();
			for(EsFunctionBox* funcboxChildWk = funcboxWk->getChild(); funcboxChildWk != 0L; funcboxChildWk = funcboxChildWk->getSibling())
			{
				funcboxChildWk->getFuncNode()->setFuncArg();
			}
		}

		u32 depthSkipMin = EsUpvarInfo::DEPTH_FREE_STATIC;	// ⇒ FREE_STATIC_LEVEL

		// 上位ブロックの変数を参照するノードの場合
		EsItNode* nodeFuncBodyWk = nodeFuncWk->getExpressionNode();
		if(nodeFuncBodyWk->getTokenType() == TKN_UPVARS)	// 名称セットノードの場合
		{
			ASSERT(nodeFuncBodyWk->getNodeType() == NT_NAMESET);
			EsAtomList* atomlist = ((EsNameSetItNode*)nodeFuncBodyWk)->namesSet();
			ASSERT(atomlist->getCount() > 0);
			for(int i = 0; i < atomlist->getCount(); i++)
			{
				EsAtomList::EsAtomListEntity* ale = atomlist->getEntity(i);
				EsNameItNode* nodeLexdepDef = ale->getDefName()->findFinalDefNode();	// ⇒ JSDefinition *lexdep = ALE_DEFN(ale)->resolve();
				if(!nodeLexdepDef->isFreeVar())
				{
					u32 depthUpverSkip = (u32)nodeLexdepDef->getUpvarInfo()->getSkipDepth();	// ⇒ uintN upvarLevel = lexdep->frameLevel();
					if((s32)depthUpverSkip <= depthWk)
					{
						nodeFuncWk->setFuncArg();
					}

					u32 nestlvgSkip = (funcboxWk->getNestDepth() + 1) - depthUpverSkip;
					if(depthSkipMin > depthUpverSkip)
					{
						depthSkipMin = nestlvgSkip;
					}
				}
			}
		}

		// 引数を持つものをキューに追加
		if(TFW_IS_FLAG(nodeFuncWk->getDefFlag(), EsNameItNode::DF_FUNCARG))	// ⇒ if (fn->isFunArg())
		{
			listQueue->addTail(funcbox);
			depthWk = (s32)funcbox->getNestDepth();
		}

		// 子ファンクションも処理．スキップレベル（ネスト深度）も更新
		if(funcboxWk->getChild() != 0L)
		{
			u32 nestlvChildMin = pickFuncArgs(funcboxWk->getChild(), depthWk, listQueue);
			ASSERT(nestlvChildMin > 0);
			if(nestlvChildMin != EsUpvarInfo::DEPTH_FREE_STATIC)
			{
				nestlvChildMin--;
				if((nestlvChildMin != 0) && (depthSkipMin > nestlvChildMin))
				{
					depthSkipMin = nestlvChildMin;
				}
			}
		}

		// 最小のスキップレベルを関数に設定する
		if(depthSkipMin != EsUpvarInfo::DEPTH_FREE_STATIC)
		{
			funcboxWk->getFunction()->setSkipDepth(depthSkipMin);
			if(nestlvSkipMinAll > depthSkipMin)
			{
				nestlvSkipMinAll = depthSkipMin;
			}
		}

		funcboxWk = funcboxWk->getSibling();
	}

	return nestlvSkipMinAll;
}

/*---------------------------------------------------------------------*//**
	関数の種類を決定する ⇒ setFunctionKinds
**//*---------------------------------------------------------------------*/
void EsFunctionBox::decideFuncKind(u32* bflags)
{
	EsFunctionBox* funcbox = this;
	EsFunctionBox* funcboxParent = funcbox->getParent();

	while(funcbox != 0L)
	{
		ASSERT(funcbox->getParent() == funcboxParent);
		// 子に再帰
		if(funcbox->getChild() != 0L)
		{
			funcbox->getChild()->decideFuncKind(bflags);
		}

		EsFunction* func = funcbox->getFunction();
		EsNameItNode* nodeFunc = funcbox->getFuncNode();
		EsItNode* nodeFuncBody = nodeFunc->getExpressionNode();
		ASSERT(nodeFuncBody != 0L);	// コールされる関数名と定義された関数名が食い違うときに起こることがあるようだ．基本的にはあり得ないかデータミス．【2015/03/05 r-kishi】

		if(TFW_IS_FLAG(funcbox->getBuildFlag(), EsBytecodeBuilder::BF_FUNC_NEED_CALLOBJ))
		{
			ASSERT(func->isInterpreted());
		}
		else if(nodeFuncBody->getTokenType() == TKN_UPVARS)
		{
			ASSERT(nodeFuncBody->getNodeType() == NT_NAMESET);
			EsAtomList* atomlist = ((EsNameSetItNode*)nodeFuncBody)->namesSet();
			ASSERT(atomlist->getCount() > 0);
			EsAtomList::EsAtomListEntity* ale = 0L;

			if(TFW_IS_FLAG(nodeFunc->getDefFlag(), EsNameItNode::DF_FUNCARG))
			{
				u32 numUpvars = 0;	// ⇒ uintN nupvars = 0;

				for(s32 i = 0; i < atomlist->getCount(); i++)
				{
					ale = atomlist->getEntity(i);
					EsNameItNode* nodeLexdepDef = ale->getDefName()->findFinalDefNode();	// ⇒ JSDefinition *lexdep = ALE_DEFN(ale)->resolve();
					if(!nodeLexdepDef->isFreeVar())
					{
						numUpvars++;
						EsFunctionBox* funcboxWk = funcbox;	// ⇒ JSFunctionBox *afunbox = funbox;
						u32 depthLexdep = nodeLexdepDef->getUpvarInfo()->getSkipDepth();	// ⇒ uintN lexdepLevel = lexdep->frameLevel();
						ASSERT(depthLexdep <= funcbox->getNestDepth());
						bool isBreakAtomForLoop = false;
						while(funcboxWk->getNestDepth() != depthLexdep)
						{
							funcboxWk = funcboxWk->getParent();
							ASSERT(funcboxWk != 0L);
							if((funcboxWk == 0L) || TFW_IS_FLAG(funcboxWk->getFuncNode()->getDefFlag(), EsNameItNode::DF_FUNCARG))
							{
								isBreakAtomForLoop = true;	// for も抜ける
								break;
							}
						}
						if(isBreakAtomForLoop)	{	break;	}

						if(funcboxWk->isInLoop())
						{
							break;
						}

						if(TFW_IS_FLAG((funcboxWk->getParent() != 0L) ? funcboxWk->getParent()->getBuildFlag() : *bflags, EsBytecodeBuilder::BF_FUNC_NEED_CALLOBJ))
						{
							break;
						}

						EsNameItNode* nodeFuncWk = funcboxWk->getFuncNode();
						EsFunction* funcWk = funcboxWk->getFunction();
						if(!TFW_IS_FLAG(funcWk->getFlags(), EsFunction::F_LAMBDA))
						{
							if(nodeLexdepDef->isBindingDefinition())
							{
								break;
							}
							if(nodeLexdepDef->getPos()->getBegin()->ge(nodeFuncWk->getPos()->getBegin()))	// ⇒ if (lexdep->pn_pos >= afunbox->node->pn_pos)
							{
								break;
							}
						}

						if(!TFW_IS_FLAG(nodeLexdepDef->getDefFlag(), EsNameItNode::DF_INITED))	// ⇒ if (!lexdep->isInitialized())
						{
							break;
						}

						EsNameItNode::DefKind dkindLexdep = nodeLexdepDef->getDefKind();

						if(dkindLexdep != EsNameItNode::DK_CONST)
						{
							if(TFW_IS_FLAG(nodeLexdepDef->getDefFlag(), EsNameItNode::DF_ASSIGNED))	// ⇒ if (lexdep->isAssigned())
							{
								break;
							}
							if(	(dkindLexdep == EsNameItNode::DK_ARG) &&
								TFW_IS_FLAG((funcboxWk->getParent() != 0L) ? funcboxWk->getParent()->getBuildFlag() : *bflags, EsBytecodeBuilder::BF_FUNC_USES_ARGUMENTS)	)
							{
								break;
							}
						}

						if(dkindLexdep != EsNameItNode::DK_FUNC)
						{
							if(nodeLexdepDef->getPos()->getEnd()->ge(nodeFuncWk->getPos()->getEnd()))	// ⇒ if (lexdep->pn_pos.end >= afunbox->node->pn_pos.end)
							{
								break;
							}

							u32 blockidLexdep = nodeLexdepDef->getBlockId();
							if(	TFW_IS_FLAG(nodeLexdepDef->getDefFlag(), EsNameItNode::DF_TOPLEVEL) )
							{
								// ⇒ MinBlockId
								bool isMin = false;
								do
								{
									if(nodeFuncWk->getBlockId() < blockidLexdep)	{	goto END_CHECK_MIN;	}
									if(TFW_IS_FLAG(nodeFuncWk->getDefFlag(), EsNameItNode::DF_EXTYPE_DEFINE))
									{
										for(EsItNode* nodeWkl = nodeFuncWk->getLinkNode(); nodeWkl != 0L; nodeWkl = nodeWkl->getLinkNode())
										{
											ASSERT(nodeWkl->getNodeType() == NT_NAME);
											if(((EsNameItNode*)nodeWkl)->getBlockId() < blockidLexdep) {	goto END_CHECK_MIN;	}
										}
									}
									isMin = true;
								}
								while(false);
								END_CHECK_MIN:

								if(!isMin)
								{
									break;
								}
							}
							else
							{
								if(!TFW_IS_FLAG(nodeLexdepDef->getDefFlag(), EsNameItNode::DF_BLOCKCHILD))
								{
									break;
								}
								if(!TFW_IS_FLAG(nodeFuncWk->getDefFlag(), EsNameItNode::DF_BLOCKCHILD))
								{
									break;
								}

								// ⇒ OneBlockId
								bool isEq = false;
								do
								{
									if(nodeFuncWk->getBlockId() != blockidLexdep)	{	goto END_CHECK_EQ;	}
									if(TFW_IS_FLAG(nodeFuncWk->getDefFlag(), EsNameItNode::DF_EXTYPE_DEFINE))
									{
										for(EsItNode* nodeWkl = nodeFuncWk->getLinkNode(); nodeWkl != 0L; nodeWkl = nodeWkl->getLinkNode())
										{
											ASSERT(nodeWkl->getNodeType() == NT_NAME);
											if(((EsNameItNode*)nodeWkl)->getBlockId() != blockidLexdep) {	goto END_CHECK_EQ;	}
										}
									}
									isEq = true;
								}
								while(false);
								END_CHECK_EQ:

								if(!isEq)
								{
									break;
								}
							}

						}
					}
				}

				if(numUpvars == 0)
				{
					TFW_SET_FLAG(*func->flags(), EsFunction::F_CLOSURE_NULL, true);
				}
				else if(ale != 0L)
				{
					TFW_SET_FLAG(*func->flags(), EsFunction::F_CLOSURE_FLAT, true);

					switch(nodeFunc->getOpcodeType())
					{
					case OP_DEFFUN:			nodeFunc->setOpcodeType(OP_DEFFUN_FC);		break;
					case OP_DEFLOCALFUN:	nodeFunc->setOpcodeType(OP_DEFLOCALFUN_FC);	break;
					case OP_LAMBDA:			nodeFunc->setOpcodeType(OP_LAMBDA_FC);		break;
					case OP_NOP:														break;
					default:				ASSERT(false);								break;
					}
				}
			}
			else
			{
				bool isMutation = TFW_IS_FLAG(funcbox->getBuildFlag(), EsBytecodeBuilder::BF_FUNC_SETS_OUTER_NAME);	// ⇒ bool mutation = !!(funbox->tcflags & TCF_FUN_SETS_OUTER_NAME);

				u32 numUpvars = 0;

				for(s32 i = 0; i < atomlist->getCount(); i++)
				{
					ale = atomlist->getEntity(i);
					EsNameItNode* nodeLexdepDef = ale->getDefName()->findFinalDefNode();	// ⇒ JSDefinition *lexdep = ALE_DEFN(ale)->resolve();
					if(!nodeLexdepDef->isFreeVar())
					{
						numUpvars++;
						if(TFW_IS_FLAG(nodeLexdepDef->getDefFlag(), EsNameItNode::DF_ASSIGNED))
						{
							break;
						}
					}
				}

				if(ale == 0L)
				{	
					isMutation = false;
				}

				if(numUpvars == 0)
				{
					TFW_SET_FLAG(*func->flags(), EsFunction::F_CLOSURE_NULL, true);
				}
				else if(!isMutation && !TFW_IS_FLAG(funcbox->getBuildFlag(), EsBytecodeBuilder::BF_FUNC_IN_GENERATOR))
				{
					TFW_SET_FLAG(*func->flags(), EsFunction::F_CLOSURE_NULL, true);
				}
			}
		}
		else
		{
			TFW_SET_FLAG(*func->flags(), EsFunction::F_CLOSURE_NULL, true);
		}

		if(func->isInterpreted())	// ⇒ if (FUN_KIND(fun) == JSFUN_INTERPRETED) {
		{
			if(nodeFuncBody->getTokenType() != TKN_UPVARS)
			{
				if(funcboxParent != 0L)
				{
					TFW_SET_FLAG(*funcboxParent->buildFlag(), EsBytecodeBuilder::BF_FUNC_NEED_CALLOBJ, true);
				}
			}
			else
			{
				ASSERT(nodeFuncBody->getNodeType() == NT_NAMESET);
				EsAtomList* atomlist = ((EsNameSetItNode*)nodeFuncBody)->namesSet();
				ASSERT(atomlist->getCount() > 0);
				for(s32 i = 0; i < atomlist->getCount(); i++)
				{
					EsAtomList::EsAtomListEntity* ale = atomlist->getEntity(i);
					EsNameItNode* nodeLexdepDef = ale->getDefName()->findFinalDefNode();	// ⇒ JSDefinition *lexdep = ALE_DEFN(ale)->resolve();
					if(!nodeLexdepDef->isFreeVar())
					{
						EsFunctionBox* funcboxWk = funcbox->getParent();
						u32 depthLexdep = nodeLexdepDef->getUpvarInfo()->getSkipDepth();	// ⇒ uintN lexdepLevel = lexdep->frameLevel();
						while(funcboxWk != 0L)
						{
							if(	((funcboxWk->getNestDepth() + 1) == depthLexdep) ||
								((depthLexdep == 0) && TFW_IS_FLAG(nodeLexdepDef->getDefFlag(), EsNameItNode::DF_LET))	)
							{
								TFW_SET_FLAG(*funcboxWk->buildFlag(), EsBytecodeBuilder::BF_FUNC_NEED_CALLOBJ, true);
							}
							funcboxWk = funcboxWk->getParent();
						}
						if(	(funcboxWk == 0L) &&
							TFW_IS_FLAG(*bflags, EsBytecodeBuilder::BF_IN_FUNCTION)	)
						{
							TFW_SET_FLAG(*bflags, EsBytecodeBuilder::BF_FUNC_NEED_CALLOBJ, true);
						}
					}
				}
			}
		}

		funcbox = funcbox->getSibling();
	}
}


////////////////////////////////////////////////////////////////////////////

ES_END_NS
