#include "Ring.h"
#include "field.h"
#include "myDirect3D.h"
#include "input.h"
#include "texture.h"
#include "debugproc.h"
#include "Xfile.h"
#include "sprite.h"

//モデルの数
#define RING_MONEL_NUM		(3)

// 移動量
#define	MODEL_MOVE		(1.0f)

//モデルの数分用意
XMODEL g_Ring_model[1] = {};

//ワールドマトリクスは基本1個、状況に応じて(体と手を"完全に"別々に動かしたい等)増やして使ってね
D3DXMATRIXA16 g_mtxRingWorld;

HRESULT InitRing()
{
	//リングのモデルを読み込んでみる
	if (FAILED(LoadXFile(&g_Ring_model[0], XFILE_INDEX_RING)))
	{
		return E_FAIL;
	}

	//座標、回転、拡縮　初期化
	RingPosReset();

	return S_OK;
}

//終了時解放処理
void UninitRing()
{
	//メッシュ情報を解放
	for (int i = 0; i < 1; i++)
	{
		SAFE_RELEASE(g_Ring_model[i].pMesh);
	}
}

void UpdateRing()
{

}

void DrawRing()
{
	//Direct3DDeviceの取得
	LPDIRECT3DDEVICE9 p_D3DDevice = GetD3DDevice();

	//ワールドトランスフォーム(絶対座標変換)
	D3DXMatrixIdentity(&g_mtxRingWorld);

	//モデル分繰り返す
	for (int j = 0; j < RING_MONEL_NUM; j++)
	{
		//ワールドマトリクス作って適用
		p_D3DDevice->SetTransform(D3DTS_WORLD, MakeWorldMatrix(&g_Ring_model[j], &g_mtxRingWorld));

		//マテリアルの数だけ繰り返し
		for (DWORD i = 0; i < g_Ring_model[j].dwNumMaterials; i++)
		{
			//マテリアルセット
			p_D3DDevice->SetMaterial(&g_Ring_model[j].pMaterials[i]);
			//テクスチャセット
			p_D3DDevice->SetTexture(0, g_Ring_model[j].pTextures[i]);
			//透過度設定
			g_Ring_model[j].pMaterials[i].Diffuse.a = 1.0f;
			//いわゆる描画
			g_Ring_model[j].pMesh->DrawSubset(i);
		}
	}
}

//座標とかリセット
void RingPosReset()
{
	//リングA
	g_Ring_model[0].Pos = D3DXVECTOR3(2.0f, 0.0f, 10.0f);
	g_Ring_model[0].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));
	g_Ring_model[0].Scale = D3DXVECTOR3(1.0f,1.0f, 1.0f);
}

