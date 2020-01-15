#pragma once

/*==================================================================================
製作者：Amalgam
概要：Xfileを使いやすく1個の関数で読み込みできるようにしてみました
　　　大体はGMで使ったLoadTexture関数と同じ感じで、DXのTexture扱う機構を盛り込んでみました。
　　　アドレス指定よりインデックスの方が分かりやすいでしょ？多分
==================================================================================*/

#include "main.h"

//XFileの3Dモデル情報を扱う構造体
typedef struct
{
	LPD3DXMESH				pMesh;			//メッシュ
	D3DMATERIAL9			*pMaterials;	//マテリアル
	LPDIRECT3DTEXTURE9		*pTextures;		//テクスチャ

	DWORD dwNumMaterials;		//3Dモデルに使われているマテリアルの数

	D3DXVECTOR3 Pos, Rot, Scale;	//座標、回転角度、拡大縮小
}XMODEL;

//XFileのインデックス　追加する場合はここと、cppの方のg_XFileListに書いてください
enum XFILE_INDEX
{
	//プレイヤー
	XFILE_INDEX_P_BODY_001 = 0,
	XFILE_INDEX_P_HEAD_001,
	XFILE_INDEX_P_GROBE_L_001,
	XFILE_INDEX_P_GROBE_R_001,
	XFILE_INDEX_P_LEG_L_001,
	XFILE_INDEX_P_LEG_R_001,
	//エネミー
	XFILE_INDEX_E_BODY_001,
	XFILE_INDEX_E_HEAD_001,
	XFILE_INDEX_E_GROBE_L_001,
	XFILE_INDEX_E_GROBE_R_001,
	XFILE_INDEX_E_LEG_L_001,
	XFILE_INDEX_E_LEG_R_001,
	XFILE_INDEX_E_DMGBODY_001,

	//リング
	XFILE_INDEX_RING,
	//月
	XFILE_INDEX_MOON,
	//飛行機
	XFILE_INDEX_AIRPLANE,
	//UFO
	XFILE_INDEX_UFO,

	XFILE_INDEX_MAX,

};

//XFileを指定して読み込む関数
//引数１：XMODEL：Xfileを読み込む変数のアドレス
//引数２：XFILE_INDEX：読み込むXfileのインデックス
HRESULT LoadXFile(XMODEL *model, XFILE_INDEX index);

//各種行列を纏めて計算してくれるやつ
//引数１：XMODEL：対象のXMODEL変数のアドレス
//引数２：D3DXMATRIXA16：計算結果を入れるワールドマトリクス
D3DXMATRIXA16 *MakeWorldMatrix(XMODEL *model, D3DXMATRIXA16 *mtxWorld);

