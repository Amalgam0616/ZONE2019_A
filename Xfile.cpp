
#include "myDirect3D.h"
#include "Xfile.h"

//Xfileのリスト　追加する場合はここと、hの方のXFILE_INDEXに書いてください
static const LPCSTR g_XFileList[XFILE_INDEX_MAX] =
{
	//プレイヤー
	"asset/Xfile/P_Body.x",
	"asset/Xfile/P_Head.x",
	"asset/Xfile/P_Grobe_L.x",
	"asset/Xfile/P_Grobe_R.x",
	"asset/Xfile/P_Leg_L.x",
	"asset/Xfile/P_Leg_R.x",
	//エネミー
	"asset/Xfile/E_Body_2.x",
	"asset/Xfile/E_Head.x",
	"asset/Xfile/E_Globe_L_ex.x",
	"asset/Xfile/E_Globe_R_ex.x",
	"asset/Xfile/E_Leg_L_ex.x",
	"asset/Xfile/E_Leg_R_ex.x",
	"asset/Xfile/E_Body_ex.x",

	//月
	"asset/Xfile/moon.x",

	//リング
	"asset/Xfile/Ling.x",
};

//XFileを指定して読み込む関数
HRESULT LoadXFile(XMODEL *model, XFILE_INDEX index)
{
	//Direct3DDeviceの取得
	LPDIRECT3DDEVICE9 g_pD3DDevice = GetD3DDevice();

	//============================================================================================
	// Xファイルからメッシュをロードする 
	//============================================================================================
	LPD3DXBUFFER pD3DXMtrlBuffer = NULL;

	HRESULT hr = D3DXLoadMeshFromX(g_XFileList[index],
		D3DXMESH_SYSTEMMEM,
		g_pD3DDevice,
		NULL,
		&pD3DXMtrlBuffer,
		NULL,
		&model->dwNumMaterials,
		&model->pMesh);

	//エラーコードチェック
	if (hr == D3DERR_INVALIDCALL)
	{
		MessageBox(NULL, "D3DERR_INVALIDCALL", "エラー", MB_OK);
		return E_FAIL;
	}
	else if (hr == E_OUTOFMEMORY)
	{
		MessageBox(NULL, "E_OUTOFMEMORY", "エラー", MB_OK);
		return E_FAIL;
	}

	//バッファのポインタ取得

	D3DXMATERIAL *D3DXMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();

	//マテリアルの数だけ D3DMATERIAL9 を取得
	model->pMaterials = new D3DMATERIAL9[model->dwNumMaterials];

	//マテリアルの数だけ LPDIRECT3DTEXTURE9 を取得
	model->pTextures = new LPDIRECT3DTEXTURE9[model->dwNumMaterials];

	//マテリアルの数だけ繰り返す
	for (DWORD i = 0; i < model->dwNumMaterials; i++)
	{
		//マテリアル情報読み込み(多分)
		model->pMaterials[i] = D3DXMaterials[i].MatD3D;
		//Ambientと鏡面反射の値をコピー　Ambient = 色？
		model->pMaterials[i].Ambient = model->pMaterials[i].Diffuse;
		//テクスチャ　NULL初期化
		model->pTextures[i] = NULL;
		//テクスチャネームが存在している且つ、ちゃんと名前がついている
		if (D3DXMaterials[i].pTextureFilename != NULL &&
			lstrlen(D3DXMaterials[i].pTextureFilename) > 0)
		{
			//テクスチャ生成
			if (FAILED(D3DXCreateTextureFromFile(g_pD3DDevice,
				D3DXMaterials[i].pTextureFilename,
				&model->pTextures[i])))
			{
				//エラー
				MessageBox(NULL, "テクスチャの読み込みに失敗しました", NULL, MB_OK);
			}
		}
	}

	//バッファ解放
	pD3DXMtrlBuffer->Release();

	return S_OK;
}
//各種行列を纏めて計算してくれるやつ
D3DXMATRIXA16 *MakeWorldMatrix(XMODEL *model, D3DXMATRIXA16 *mtxWorld)
{
	//移動、拡縮、回転行列
	D3DXMATRIXA16 mtxPos, mtxScl, mtxRot;

	//移動行列
	D3DXMatrixTranslation(&mtxPos, model->Pos.x, model->Pos.y, model->Pos.z);

	//拡縮行列
	D3DXMatrixScaling(&mtxScl, model->Scale.x, model->Scale.y, model->Scale.z);

	//3軸回転行列　ジンバルロックに気を付けてね
	D3DXMatrixRotationYawPitchRoll(&mtxRot, model->Rot.y, model->Rot.x, model->Rot.z);

	//１軸回転
	//D3DXMatrixRotationX(&matRotate[0], g_Rot[0].x);							//X軸回転
	//D3DXMatrixRotationY(&matRotate[0], g_Rot[0].y);							//Y軸回転
	//D3DXMatrixRotationZ(&matRotate[0], g_Rot[0].z);							//Z軸回転

	//拡縮行列と回転行列を合成してMulにいれる
	D3DXMatrixMultiply(&mtxScl, &mtxScl, &mtxRot);

	//Mul(拡縮行列＊回転行列)と移動行列を合成してワールドマトリクスにいれる
	D3DXMatrixMultiply(mtxWorld, &mtxScl, &mtxPos);

	return mtxWorld;
}