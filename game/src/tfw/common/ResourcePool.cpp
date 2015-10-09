/***********************************************************************//**
 *  ResourcePool.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/07/22.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ResourcePool.h"

// Friends
#include "IdManager.h"
#include "../collection/List.h"
#include "../string/VcString.h"
#include "../image/ImageBase.h"
#include "../gcmn/Texture.h"
#include "../gcmn/TexAnim.h"
#include "../g3d/Shape.h"
#include "../g3d/Material.h"
#include "../g3d/HrAnimObj.h"
#include "../g3d/HrAnimSet.h"
#include "../g3d/CamAnim.h"
#include "../g3d/sg/node/SgEntity.h"
#include "../g3d/sg/node/implement/Model.h"
#include "../g3d/sg/node/implement/BillboardModel.h"
#include "../g3d/sg/node/implement/ShapeModel.h"
#include "../collision/Gcol.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ResourcePool メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	既存のシェイプを検索
**//*---------------------------------------------------------------------*/
Shape* ResourcePool::findShape(const CStringBase* defname) const
{
	for(ListIterator<Shape*> it = _listShape->iterator(); it.has(); it.next())
	{
		Shape* shapeWk = it.object();
		const VcString* defnameWk = shapeWk->getDefineName();
		if(defnameWk != 0L)
		{
			if(defnameWk->equals(defname->getRaw()))
			{
				return shapeWk;
			}
		}
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	既存の階層アニメセットを検索
**//*---------------------------------------------------------------------*/
HrAnimSet* ResourcePool::findHrAnimSet(Shape* shape) const
{
	for(ListIterator<HrAnimSet*> it = _listHrAset->iterator(); it.has(); it.next())
	{
		HrAnimSet* hasetWk = it.object();
		for(int ihaobj = 0; ihaobj < hasetWk->getObjRootNum(); ihaobj++)
		{
			HrAnimObj* haobj = hasetWk->getObjRoot(ihaobj);
			if(HrAnimObj::find(haobj, shape) != 0L)
			{
				return hasetWk;
			}
		}
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	シェイプ追加
**//*---------------------------------------------------------------------*/
void ResourcePool::add(Shape** shapearrEntr, s32 numShape)
{
	for(int i = 0; i < numShape; i++)
	{
		bool isAdd = true;

		// 重複チェック
		for(ListIterator<Shape*> it = _listShape->iterator(); it.has(); it.next())
		{
			Shape* shape = it.object();
			if(shape == shapearrEntr[i])
			{
				isAdd = false;
				break;
			}
		}

		// リストに追加しつつ ID を付加
		if(isAdd)
		{
			shapearrEntr[i]->setId(_idmngrRef->getNewId());
			_listShape->addTail(shapearrEntr[i]);
		}
	}

	// add 用配列を削除
	delete[] shapearrEntr;
}

/*---------------------------------------------------------------------*//**
	テクスチャ追加
**//*---------------------------------------------------------------------*/
void ResourcePool::add(Texture** texarrEntr, s32 numTex)
{
	for(int i = 0; i < numTex; i++)
	{
		bool isAdd = true;

		// 重複チェック
		for(ListIterator<Texture*> it = _listTex->iterator(); it.has(); it.next())
		{
			Texture* tex = it.object();
			if(tex == texarrEntr[i])
			{
				isAdd = false;
				break;
			}
		}

		// リストに追加しつつ ID を付加
		if(isAdd)
		{
			texarrEntr[i]->setId(_idmngrRef->getNewId());
			_listTex->addTail(texarrEntr[i]);

			// イメージも追加
			add(texarrEntr[i]->getImage());
			texarrEntr[i]->markResourcePoolManage();	// リソースプール管理に設定する
		}
	}
	// add 用配列を削除
	delete[] texarrEntr;
}

/*---------------------------------------------------------------------*//**
	イメージ追加
**//*---------------------------------------------------------------------*/
void ResourcePool::add(ImageBase* imgEntr)
{
	bool isAdd = true;

	// 重複チェック
	for(ListIterator<ImageBase*> it = _listImg->iterator(); it.has(); it.next())
	{
		ImageBase* img = it.object();
		if(img == imgEntr)
		{
			isAdd = false;
			break;
		}
	}

	// リストに追加
	if(isAdd)
	{
		//imgarrEntr[i]->setId(_idmngrRef->getNewId());
		_listImg->addTail(imgEntr);
	}
}

/*---------------------------------------------------------------------*//**
	テクスチャアニメーション追加
**//*---------------------------------------------------------------------*/
void ResourcePool::add(TexAnim** taarrEntr, s32 numTexAnim)
{
	for(int i = 0; i < numTexAnim; i++)
	{
		bool isAdd = true;

		// 重複チェック
		for(ListIterator<TexAnim*> it = _listTexAnim->iterator(); it.has(); it.next())
		{
			TexAnim* texanim = it.object();
			if(texanim == taarrEntr[i])
			{
				isAdd = false;
				break;
			}
		}

		// リストに追加
		if(isAdd)
		{
			//taarrEntr[i]->setId(_idmngrRef->getNewId());
			_listTexAnim->addTail(taarrEntr[i]);
		}
	}
	// add 用配列を削除
	delete[] taarrEntr;
}

/*---------------------------------------------------------------------*//**
	マテリアル追加
**//*---------------------------------------------------------------------*/
void ResourcePool::add(Material** mtrlarrEntr, s32 numMtrl)
{
	for(int i = 0; i < numMtrl; i++)
	{
		bool isAdd = true;

		// 重複チェック
		for(ListIterator<Material*> it = _listMtrl->iterator(); it.has(); it.next())
		{
			Material* mtrl = it.object();
			if(mtrl == mtrlarrEntr[i])
			{
				isAdd = false;
				break;
			}
		}

		// リストに追加しつつ ID を付加
		if(isAdd)
		{
			mtrlarrEntr[i]->setId(_idmngrRef->getNewId());
			_listMtrl->addTail(mtrlarrEntr[i]);
		}
	}
	// add 用配列を削除
	delete[] mtrlarrEntr;
}

/*---------------------------------------------------------------------*//**
	アニメ階層追加
**//*---------------------------------------------------------------------*/
void ResourcePool::add(HrAnimObj** haobjarrEntr, s32 numAobj)
{
	for(int i = 0; i < numAobj; i++)
	{
		bool isAdd = true;

		// 重複チェック
		for(ListIterator<HrAnimObj*> it = _listHrAobj->iterator(); it.has(); it.next())
		{
			HrAnimObj* haobj = it.object();
			if(haobj == haobjarrEntr[i])
			{
				isAdd = false;
				break;
			}
		}

		// リストに追加
		if(isAdd)
		{
			_listHrAobj->addTail(haobjarrEntr[i]);
		}
	}
	// add 用配列を削除
	delete[] haobjarrEntr;
}

/*---------------------------------------------------------------------*//**
	階層アニメセット追加
**//*---------------------------------------------------------------------*/
void ResourcePool::add(HrAnimSet** hasetarrEntr, s32 numAset)
{
	// リストに追加
	for(int i = 0; i < numAset; i++)
	{
		bool isAdd = true;

		// 重複チェック
		for(ListIterator<HrAnimSet*> it = _listHrAset->iterator(); it.has(); it.next())
		{
			HrAnimSet* haset = it.object();
			if(haset == hasetarrEntr[i])
			{
				isAdd = false;
				break;
			}
		}

		// リストに追加
		if(isAdd)
		{
			_listHrAset->addTail(hasetarrEntr[i]);
		}
	}
	// add 用配列を削除
	delete[] hasetarrEntr;
}

/*---------------------------------------------------------------------*//**
	カメラアニメ追加
**//*---------------------------------------------------------------------*/
void ResourcePool::add(CamAnim* camanimEntr)
{
	_listCamA->addTail(camanimEntr);
}

/*---------------------------------------------------------------------*//**
	地面コリジョン追加
**//*---------------------------------------------------------------------*/
void ResourcePool::add(Gcol** gcolarrEntr, s32 numGcol)
{
	// リストに追加
	for(int i = 0; i < numGcol; i++)
	{
		bool isAdd = true;

		// 重複チェック
		for(ListIterator<Gcol*> it = _listGcol->iterator(); it.has(); it.next())
		{
			Gcol* gcol = it.object();
			if(gcol == gcolarrEntr[i])
			{
				isAdd = false;
				break;
			}
		}

		// リストに追加
		if(isAdd)
		{
			_listGcol->addTail(gcolarrEntr[i]);
		}
	}
	// add 用配列を削除
	delete[] gcolarrEntr;
}

/*---------------------------------------------------------------------*//**
	シーングラフエンティティ追加
**//*---------------------------------------------------------------------*/
void ResourcePool::add(SgEntity** sgearrEntr, s32 numEntity)
{
	for(int i = 0; i < numEntity; i++)
	{
		// ノードへの ID 付加（ID マネージャから ID を発行してもらう）
		sgearrEntr[i]->setId(_idmngrRef->getNewId());
		// リソースプール管理に設定
		sgearrEntr[i]->setOnResourcePool(true);

		// リストに追加
		_listSgEnt->addTail(sgearrEntr[i]);
	}
}

/*---------------------------------------------------------------------*//**
	使用していない SgEntity （主にモデル）のリソースを削除
**//*---------------------------------------------------------------------*/
void ResourcePool::removeUnusedSgEntityResource()
{
	// 使用中フラグ用作業配列を作成
	bool* isUsedShape = new bool[_listShape->getCount()];
	bool* isUsedTex = new bool[_listTex->getCount()];
	bool* isUsedImg = new bool[_listImg->getCount()];
	bool* isUsedTexAnim = new bool[_listTexAnim->getCount()];
	bool* isUsedMtrl = new bool[_listMtrl->getCount()];
	bool* isUsedHrAobj = new bool[_listHrAobj->getCount()];
	bool* isUsedHrAset = new bool[_listHrAset->getCount()];
	bool* isUsedCamA = new bool[_listCamA->getCount()];
	bool* isUsedGcol = new bool[_listGcol->getCount()];
	bool* isUsedSgEnt = new bool[_listSgEnt->getCount()];
	TFW_ZERO_MEMORY(isUsedShape, sizeof(bool) * _listShape->getCount());
	TFW_ZERO_MEMORY(isUsedTex, sizeof(bool) * _listTex->getCount());
	TFW_ZERO_MEMORY(isUsedImg, sizeof(bool) * _listImg->getCount());
	TFW_ZERO_MEMORY(isUsedTexAnim, sizeof(bool) * _listTexAnim->getCount());
	TFW_ZERO_MEMORY(isUsedMtrl, sizeof(bool) * _listMtrl->getCount());
	TFW_ZERO_MEMORY(isUsedHrAobj, sizeof(bool) * _listHrAobj->getCount());
	TFW_ZERO_MEMORY(isUsedHrAset, sizeof(bool) * _listHrAset->getCount());
	TFW_ZERO_MEMORY(isUsedCamA, sizeof(bool) * _listCamA->getCount());
	TFW_ZERO_MEMORY(isUsedGcol, sizeof(bool) * _listGcol->getCount());
	TFW_ZERO_MEMORY(isUsedSgEnt, sizeof(bool) * _listSgEnt->getCount());

	// リスト中の SgEntity について調べる
	int idxListSgent = 0;
	for(ListIterator<SgEntity*> itListSgent = _listSgEnt->iterator(); itListSgent.has(); itListSgent.next())
	{
		SgEntity* sgent= itListSgent.object();
		isUsedSgEnt[idxListSgent] = sgent->isOnSg() || sgent->isUsed();

		// 使用中のもののみ調べる
		if(!isUsedSgEnt[idxListSgent])
		{
			// 次へ
			idxListSgent++;
			continue;
		}

		if(sgent->getType() == SGNTYPE_MODEL)	// SgEntity はモデルである
		{
			Model* model = (Model*)sgent;
			if(model->getModelClass() == Model::MODELCLASS_SHAPE)	// モデルはシェイプモデルである
			{
				ShapeModel* smdl = (ShapeModel*)model;

				// 使用中のシェイプについて調べる
				for(int idxUsedShape = 0; idxUsedShape < smdl->getShapeNum(); idxUsedShape++)
				{
					const Shape* shapeUsed = smdl->getShape(idxUsedShape);
					checkUsedShape(shapeUsed, isUsedShape, isUsedMtrl, isUsedTex, isUsedImg, isUsedTexAnim);
				}

				// 使用中の階層アニメセットについて調べる
				const HrAnimSet* hasetUsed = smdl->getAnimSet();
				// プールリストの中から探し出して使用フラグを立てる
				u32 idxListHaset = 0;
				for(ListIterator<HrAnimSet*> itListHaset = _listHrAset->iterator(); itListHaset.has(); itListHaset.next())
				{
					const HrAnimSet* hasetPl = itListHaset.object();
					if(hasetUsed == hasetPl)
					{
						ASSERT(idxListHaset < _listHrAset->getCount());
						isUsedHrAset[idxListHaset] = true;	// 使用フラグを立てる
						
						// 使用中の階層アニメオブジェについて調べる
						for(int idxUsedHabojRoot = 0; idxUsedHabojRoot < hasetUsed->getObjRootNum(); idxUsedHabojRoot++)
						{
							checkUsedHrAnimObjDive(hasetUsed->getObjRoot(idxUsedHabojRoot), isUsedHrAobj);
						}

						break;
					}
					idxListHaset++;
				}

				// 使用中の地面当たりについて調べる
				const Gcol* gcolUsed = smdl->getGcol();
				// プールリストの中から探し出して使用フラグを立てる
				u32 idxListGcol = 0;
				for(ListIterator<Gcol*> itListGcol = _listGcol->iterator(); itListGcol.has(); itListGcol.next())
				{
					const Gcol* gcolPl = itListGcol.object();
					if(gcolUsed == gcolPl)
					{
						ASSERT(idxListGcol < _listShape->getCount());
						isUsedGcol[idxListGcol] = true;	// 使用フラグを立てる
						break;
					}
					idxListGcol++;
				}
			}
			else if(model->getModelClass() == Model::MODELCLASS_BILLBOARD)	// モデルはビルボードモデルである
			{
				BillboardModel* bmdl = (BillboardModel*)model;

				// 使用中のシェイプについて調べる
				{
					const Shape* shapeUsed = bmdl->getShape();
					checkUsedShape(shapeUsed, isUsedShape, isUsedMtrl, isUsedTex, isUsedImg, isUsedTexAnim);
				}
			}

			// 使用中のカメラアニメについて調べる
			const CamAnim* camaUsed = model->getCamAnim();
			if(camaUsed != 0L)
			{
				u32 idxListCamA = 0;
				for(ListIterator<CamAnim*> itListCamA = _listCamA->iterator(); itListCamA.has(); itListCamA.next())
				{
					const CamAnim* camaPl = itListCamA.object();
					if(camaUsed == camaPl)
					{
						ASSERT(idxListCamA < _listCamA->getCount());
						isUsedCamA[idxListCamA] = true;	// 使用フラグを立てる
						break;
					}
					idxListCamA++;
				}
			}
		}

		idxListSgent++;
	}

	// 未使用のリソースを削除する
	{
		// 最初に一番外側であるシーングラフノードを削除する
		int idx = 0;
		ListIterator<SgEntity*> it = _listSgEnt->iterator();
		while(it.has())
		{
			if(!isUsedSgEnt[idx])
			{
				ListNode<SgEntity*>* node = it.node();
				SgEntity* sgent= node->getObject();
				_idmngrRef->releaseId(sgent->getId());	// ID 解放
				sgent->destroy();
				delete sgent;

				idx++;
				it.next();
				_listSgEnt->removeNode(node);
				continue;
			}
			idx++;
			it.next();
		}
	}
	{
		int idx = 0;
		ListIterator<Shape*> it = _listShape->iterator();
		while(it.has())
		{
			if(!isUsedShape[idx])
			{
				ListNode<Shape*>* node = it.node();
				Shape* shape = node->getObject();
				_idmngrRef->releaseId(shape->getId());	// ID 解放
				shape->destroy();
				delete shape;

				idx++;
				it.next();
				_listShape->removeNode(node);
				continue;
			}
			idx++;
			it.next();
		}
	}
	{
		int idx = 0;
		ListIterator<Texture*> it = _listTex->iterator();
		while(it.has())
		{
			if(!isUsedTex[idx])
			{
				ListNode<Texture*>* node = it.node();
				Texture* tex = node->getObject();
				_idmngrRef->releaseId(tex->getId());	// ID 解放
				tex->destroy();
				delete tex;

				idx++;
				it.next();
				_listTex->removeNode(node);
				continue;
			}
			idx++;
			it.next();
		}
	}
	{
		int idx = 0;
		ListIterator<ImageBase*> it = _listImg->iterator();
		while(it.has())
		{
			if(!isUsedImg[idx])
			{
				ListNode<ImageBase*>* node = it.node();
				ImageBase* img = node->getObject();
				img->destroy();
				delete img;

				idx++;
				it.next();
				_listImg->removeNode(node);
				continue;
			}
			idx++;
			it.next();
		}
	}
	{
		int idx = 0;
		ListIterator<TexAnim*> it = _listTexAnim->iterator();
		while(it.has())
		{
			if(!isUsedTexAnim[idx])
			{
				ListNode<TexAnim*>* node = it.node();
				TexAnim* texanim = node->getObject();
				texanim->destroy();
				delete texanim;

				idx++;
				it.next();
				_listTexAnim->removeNode(node);
				continue;
			}
			idx++;
			it.next();
		}
	}
	{
		int idx = 0;
		ListIterator<Material*> it = _listMtrl->iterator();
		while(it.has())
		{
			if(!isUsedMtrl[idx])
			{
				ListNode<Material*>* node = it.node();
				Material* mtrl = node->getObject();
				_idmngrRef->releaseId(mtrl->getId());	// ID 解放
				mtrl->destroy();
				delete mtrl;

				idx++;
				it.next();
				_listMtrl->removeNode(node);
				continue;
			}
			idx++;
			it.next();
		}
	}
	{
		int idx = 0;
		ListIterator<HrAnimObj*> it = _listHrAobj->iterator();
		while(it.has())
		{
			if(!isUsedHrAobj[idx])
			{
				ListNode<HrAnimObj*>* node = it.node();
				HrAnimObj* haobj = node->getObject();
				delete haobj;

				idx++;
				it.next();
				_listHrAobj->removeNode(node);
				continue;
			}
			idx++;
			it.next();
		}
	}
	{
		int idx = 0;
		ListIterator<HrAnimSet*> it = _listHrAset->iterator();
		while(it.has())
		{
			if(!isUsedHrAset[idx])
			{
				ListNode<HrAnimSet*>* node = it.node();
				HrAnimSet* haset = node->getObject();
				haset->destroy();
				delete haset;

				idx++;
				it.next();
				_listHrAset->removeNode(node);
				continue;
			}
			idx++;
			it.next();
		}
	}
	{
		int idx = 0;
		ListIterator<CamAnim*> it = _listCamA->iterator();
		while(it.has())
		{
			if(!isUsedCamA[idx])
			{
				ListNode<CamAnim*>* node = it.node();
				CamAnim* cama = node->getObject();
				cama->destroy();
				delete cama;

				idx++;
				it.next();
				_listCamA->removeNode(node);
				continue;
			}
			idx++;
			it.next();
		}
	}
	{
		int idx = 0;
		ListIterator<Gcol*> it = _listGcol->iterator();
		while(it.has())
		{
			if(!isUsedGcol[idx])
			{
				ListNode<Gcol*>* node = it.node();
				Gcol* gcol= node->getObject();
				gcol->destroy();
				delete gcol;

				idx++;
				it.next();
				_listGcol->removeNode(node);
				continue;
			}
			idx++;
			it.next();
		}
	}

	// 使用中フラグ用作業配列を削除
	delete[] isUsedShape;
	delete[] isUsedTex;
	delete[] isUsedImg;
	delete[] isUsedTexAnim;
	delete[] isUsedMtrl;
	delete[] isUsedHrAobj;
	delete[] isUsedHrAset;
	delete[] isUsedCamA;
	delete[] isUsedGcol;
	delete[] isUsedSgEnt;
}

/*---------------------------------------------------------------------*//**
	使用しているシェイプを調べる
	removeUnusedSgEntityResource のサブルーチン
**//*---------------------------------------------------------------------*/
void ResourcePool::checkUsedShape(const Shape* shapeUsed, bool* isUsedShape, bool* isUsedMtrl, bool* isUsedTex, bool* isUsedImg, bool* isUsedTexAnim)
{
	// プールリストの中から探し出して使用フラグを立てる
	u32 idxListShape = 0;
	for(ListIterator<Shape*> itListShape = _listShape->iterator(); itListShape.has(); itListShape.next())
	{
		const Shape* shapePl = itListShape.object();
		if(shapeUsed == shapePl)
		{
			ASSERT(idxListShape < _listShape->getCount());
			isUsedShape[idxListShape] = true;	// 使用フラグを立てる

			// 使用中のマテリアルについて調べる
			const Material* mtrlUsed = shapeUsed->getMaterial();
			// プールリストの中から探し出して使用フラグを立てる
			u32 idxListMtrl = 0;
			for(ListIterator<Material*> itListMtrl = _listMtrl->iterator(); itListMtrl.has(); itListMtrl.next())
			{
				Material* mtrlPl = itListMtrl.object();
				if(mtrlUsed == mtrlPl)
				{
					ASSERT(idxListMtrl < _listMtrl->getCount());
					isUsedMtrl[idxListMtrl] = true;	// 使用フラグを立てる

					// 使用中のテクスチャについて調べる
					const Texture* texUsed = mtrlUsed->getTexture();
					// プールリストの中から探し出して使用フラグを立てる
					u32 idxListTex = 0;
					for(ListIterator<Texture*> itListTex = _listTex->iterator(); itListTex.has(); itListTex.next())
					{
						Texture* texPl = itListTex.object();
						if(texUsed == texPl)
						{
							ASSERT(idxListTex < _listTex->getCount());
							isUsedTex[idxListTex] = true;	// 使用フラグを立てる

							// 使用中のイメージについて調べる
							const ImageBase* imgUsed = texUsed->getImage();
							if(imgUsed != 0L)
							{
								// プールリストの中から探し出して使用フラグを立てる
								u32 idxListImg = 0;
								for(ListIterator<ImageBase*> itListImg = _listImg->iterator(); itListImg.has(); itListImg.next())
								{
									ImageBase* imgPl = itListImg.object();
									if(imgUsed == imgPl)
									{
										ASSERT(idxListImg < _listImg->getCount());
										isUsedImg[idxListImg] = true;	// 使用フラグを立てる
										break;
									}
									idxListImg++;
								}
							}

							// 使用中のテクスチャアニメについて調べる
							const TexAnim* texanimUsed = texUsed->getTexAnim();
							if(texanimUsed != 0L)
							{
								// プールリストの中から探し出して使用フラグを立てる
								u32 idxListTexAnim = 0;
								for(ListIterator<TexAnim*> itListTexAnim = _listTexAnim->iterator(); itListTexAnim.has(); itListTexAnim.next())
								{
									TexAnim* texanimPl = itListTexAnim.object();
									if(texanimUsed == texanimPl)
									{
										ASSERT(idxListTexAnim < _listTexAnim->getCount());
										isUsedTexAnim[idxListTexAnim] = true;	// 使用フラグを立てる
										break;
									}
									idxListTexAnim++;
								}
							}

							break;
						}
						idxListTex++;
					}

					break;
				}
				idxListMtrl++;
			}

			break;
		}
		idxListShape++;
	}
}

/*---------------------------------------------------------------------*//**
	使用している階層アニメオブジェを調べる
	removeUnusedSgEntityResource のサブルーチン
**//*---------------------------------------------------------------------*/
void ResourcePool::checkUsedHrAnimObjDive(const HrAnimObj* haobjUsed, bool* isUsedHrAobj)
{
	// プールリストの中から探し出して使用フラグを立てる
	u32 idxListHaobj = 0;
	for(ListIterator<HrAnimObj*> it = _listHrAobj->iterator(); it.has(); it.next())
	{
		HrAnimObj* haobjPl = it.object();
		if(haobjUsed == haobjPl)
		{
			ASSERT(idxListHaobj < _listHrAobj->getCount());
			isUsedHrAobj[idxListHaobj] = true;	// 使用フラグを立てる
			break;
		}
		idxListHaobj++;
	}

	// 子・兄弟に再帰処理
	if(haobjUsed->getChild() != 0L)
	{
		checkUsedHrAnimObjDive(haobjUsed->getChild(), isUsedHrAobj);
	}
	if(haobjUsed->getSibling() != 0L)
	{
		checkUsedHrAnimObjDive(haobjUsed->getSibling(), isUsedHrAobj);
	}
}

/*---------------------------------------------------------------------*//**
	全てのリソースを削除
**//*---------------------------------------------------------------------*/
void ResourcePool::removeAll()
{
	// 最初に一番外側であるシーングラフノードを削除する
	for(ListIterator<SgEntity*> it = _listSgEnt->iterator(); it.has(); it.next())
	{
		SgEntity* sgent= it.object();
		_idmngrRef->releaseId(sgent->getId());
		sgent->destroy();
		delete sgent;
	}
	_listSgEnt->removeAll();

	for(ListIterator<Shape*> it = _listShape->iterator(); it.has(); it.next())
	{
		Shape* shape = it.object();
		_idmngrRef->releaseId(shape->getId());
		shape->destroy();
		delete shape;
	}
	_listShape->removeAll();

	for(ListIterator<Texture*> it = _listTex->iterator(); it.has(); it.next())
	{
		Texture* tex = it.object();
		_idmngrRef->releaseId(tex->getId());
		tex->destroy();
		delete tex;
	}
	_listTex->removeAll();

	for(ListIterator<ImageBase*> it = _listImg->iterator(); it.has(); it.next())
	{
		ImageBase* img = it.object();
		//_idmngrRef->releaseId(img->getId());
		img->destroy();
		delete img;
	}
	_listImg->removeAll();

	for(ListIterator<TexAnim*> it = _listTexAnim->iterator(); it.has(); it.next())
	{
		TexAnim* texanim = it.object();
		//_idmngrRef->releaseId(texanim->getId());
		texanim->destroy();
		delete texanim;
	}
	_listTexAnim->removeAll();

	for(ListIterator<Material*> it = _listMtrl->iterator(); it.has(); it.next())
	{
		Material* mtrl = it.object();
		_idmngrRef->releaseId(mtrl->getId());
		mtrl->destroy();
		delete mtrl;
	}
	_listMtrl->removeAll();

	for(ListIterator<HrAnimObj*> it = _listHrAobj->iterator(); it.has(); it.next())
	{
		HrAnimObj* haobj = it.object();
		delete haobj;
	}
	_listHrAobj->removeAll();

	for(ListIterator<HrAnimSet*> it = _listHrAset->iterator(); it.has(); it.next())
	{
		HrAnimSet* haset = it.object();
		haset->destroy();
		delete haset;
	}
	_listHrAset->removeAll();

	for(ListIterator<Gcol*> it = _listGcol->iterator(); it.has(); it.next())
	{
		Gcol* gcol= it.object();
		gcol->destroy();
		delete gcol;
	}
	_listGcol->removeAll();

	for(ListIterator<CamAnim*> it = _listCamA->iterator(); it.has(); it.next())
	{
		CamAnim* camanim= it.object();
		camanim->destroy();
		delete camanim;
	}
	_listCamA->removeAll();
}

/*---------------------------------------------------------------------*//**
	リソースの状況をデバッグ出力する
**//*---------------------------------------------------------------------*/
void ResourcePool::traceInfo()
{
#if defined(_DEBUG)
	unsigned int numUsedEntity = 0;
	for(ListIterator<SgEntity*> it = _listSgEnt->iterator(); it.has(); it.next())
	{
		SgEntity* sgent= it.object();
		if(sgent->isUsed())
		{
			numUsedEntity++;
		}
	}

	TRACE("{ResourcePool::traceInfo} >>>\n");
	TRACE("    number of SgEntity : %d (used = %d)\n", _listSgEnt->getCount(), numUsedEntity);
	TRACE("    number of Shape : %d\n", _listShape->getCount());
	TRACE("    number of Texture : %d\n", _listTex->getCount());
	TRACE("    number of Image : %d\n", _listImg->getCount());
	TRACE("    number of TexAnim : %d\n", _listTexAnim->getCount());
	TRACE("    number of Material : %d\n", _listMtrl->getCount());
	TRACE("    number of HrAnimObj : %d\n", _listHrAobj->getCount());
	TRACE("    number of HrAnimSet : %d\n", _listHrAset->getCount());
	TRACE("    number of CamAnim : %d\n", _listCamA->getCount());
	TRACE("    number of Gcol : %d\n", _listGcol->getCount());
	TRACE("<<<\n");
#endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ResourcePool::ResourcePool()
	: _listShape(0L)
	, _listTex(0L)
	, _listImg(0L)
	, _listTexAnim(0L)
	, _listMtrl(0L)
	, _listHrAobj(0L)
	, _listHrAset(0L)
	, _listCamA(0L)
	, _listGcol(0L)
	, _listSgEnt(0L)
	, _idmngrRef(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
ResourcePool::~ResourcePool()
{
	// destroy で破棄されているはずのオブジェクト
	ASSERT(_listShape == 0L);
	ASSERT(_listTex == 0L);
	ASSERT(_listImg == 0L);
	ASSERT(_listTexAnim == 0L);
	ASSERT(_listMtrl == 0L);
	ASSERT(_listHrAobj == 0L);
	ASSERT(_listHrAset == 0L);
	ASSERT(_listCamA == 0L);
	ASSERT(_listGcol == 0L);
	ASSERT(_listSgEnt == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool ResourcePool::create(IdManager* idmngrRef)
{
	_listShape = new List<Shape*>;
	_listTex = new List<Texture*>;
	_listImg = new List<ImageBase*>;
	_listTexAnim = new List<TexAnim*>;
	_listMtrl = new List<Material*>;
	_listHrAobj = new List<HrAnimObj*>;
	_listHrAset = new List<HrAnimSet*>;
	_listCamA = new List<CamAnim*>;
	_listGcol = new List<Gcol*>;
	_listSgEnt = new List<SgEntity*>;

	_idmngrRef = idmngrRef;
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void ResourcePool::destroy()
{
	removeAll();

	delete _listShape;
	_listShape = 0L;
	delete _listTex;
	_listTex = 0L;
	delete _listImg;
	_listImg = 0L;
	delete _listTexAnim;
	_listTexAnim = 0L;
	delete _listMtrl;
	_listMtrl = 0L;
	delete _listHrAobj;
	_listHrAobj = 0L;
	delete _listHrAset;
	_listHrAset = 0L;
	delete _listCamA;
	_listCamA = 0L;
	delete _listGcol;
	_listGcol = 0L;
	delete _listSgEnt;
	_listSgEnt = 0L;

	_idmngrRef = 0L;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS