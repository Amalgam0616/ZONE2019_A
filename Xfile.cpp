
#include "myDirect3D.h"
#include "Xfile.h"

//Xfile�̃��X�g�@�ǉ�����ꍇ�͂����ƁAh�̕���XFILE_INDEX�ɏ����Ă�������
static const LPCSTR g_XFileList[XFILE_INDEX_MAX] =
{
	//�v���C���[
	"asset/Xfile/P_Body.x",
	"asset/Xfile/P_Head.x",
	"asset/Xfile/P_Grobe_L.x",
	"asset/Xfile/P_Grobe_R.x",
	"asset/Xfile/P_Leg_L.x",
	"asset/Xfile/P_Leg_R.x",
	//�G�l�~�[
	"asset/Xfile/E_Body_2.x",
	"asset/Xfile/E_Head.x",
	"asset/Xfile/E_Globe_L_ex.x",
	"asset/Xfile/E_Globe_R_ex.x",
	"asset/Xfile/E_Leg_L_ex.x",
	"asset/Xfile/E_Leg_R_ex.x",
	"asset/Xfile/E_Body_ex.x",

	//��
	"asset/Xfile/moon.x",

	//�����O
	"asset/Xfile/Ling.x",
};

//XFile���w�肵�ēǂݍ��ފ֐�
HRESULT LoadXFile(XMODEL *model, XFILE_INDEX index)
{
	//Direct3DDevice�̎擾
	LPDIRECT3DDEVICE9 g_pD3DDevice = GetD3DDevice();

	//============================================================================================
	// X�t�@�C�����烁�b�V�������[�h���� 
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

	//�G���[�R�[�h�`�F�b�N
	if (hr == D3DERR_INVALIDCALL)
	{
		MessageBox(NULL, "D3DERR_INVALIDCALL", "�G���[", MB_OK);
		return E_FAIL;
	}
	else if (hr == E_OUTOFMEMORY)
	{
		MessageBox(NULL, "E_OUTOFMEMORY", "�G���[", MB_OK);
		return E_FAIL;
	}

	//�o�b�t�@�̃|�C���^�擾

	D3DXMATERIAL *D3DXMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();

	//�}�e���A���̐����� D3DMATERIAL9 ���擾
	model->pMaterials = new D3DMATERIAL9[model->dwNumMaterials];

	//�}�e���A���̐����� LPDIRECT3DTEXTURE9 ���擾
	model->pTextures = new LPDIRECT3DTEXTURE9[model->dwNumMaterials];

	//�}�e���A���̐������J��Ԃ�
	for (DWORD i = 0; i < model->dwNumMaterials; i++)
	{
		//�}�e���A�����ǂݍ���(����)
		model->pMaterials[i] = D3DXMaterials[i].MatD3D;
		//Ambient�Ƌ��ʔ��˂̒l���R�s�[�@Ambient = �F�H
		model->pMaterials[i].Ambient = model->pMaterials[i].Diffuse;
		//�e�N�X�`���@NULL������
		model->pTextures[i] = NULL;
		//�e�N�X�`���l�[�������݂��Ă��銎�A�����Ɩ��O�����Ă���
		if (D3DXMaterials[i].pTextureFilename != NULL &&
			lstrlen(D3DXMaterials[i].pTextureFilename) > 0)
		{
			//�e�N�X�`������
			if (FAILED(D3DXCreateTextureFromFile(g_pD3DDevice,
				D3DXMaterials[i].pTextureFilename,
				&model->pTextures[i])))
			{
				//�G���[
				MessageBox(NULL, "�e�N�X�`���̓ǂݍ��݂Ɏ��s���܂���", NULL, MB_OK);
			}
		}
	}

	//�o�b�t�@���
	pD3DXMtrlBuffer->Release();

	return S_OK;
}
//�e��s���Z�߂Čv�Z���Ă������
D3DXMATRIXA16 *MakeWorldMatrix(XMODEL *model, D3DXMATRIXA16 *mtxWorld)
{
	//�ړ��A�g�k�A��]�s��
	D3DXMATRIXA16 mtxPos, mtxScl, mtxRot;

	//�ړ��s��
	D3DXMatrixTranslation(&mtxPos, model->Pos.x, model->Pos.y, model->Pos.z);

	//�g�k�s��
	D3DXMatrixScaling(&mtxScl, model->Scale.x, model->Scale.y, model->Scale.z);

	//3����]�s��@�W���o�����b�N�ɋC��t���Ă�
	D3DXMatrixRotationYawPitchRoll(&mtxRot, model->Rot.y, model->Rot.x, model->Rot.z);

	//�P����]
	//D3DXMatrixRotationX(&matRotate[0], g_Rot[0].x);							//X����]
	//D3DXMatrixRotationY(&matRotate[0], g_Rot[0].y);							//Y����]
	//D3DXMatrixRotationZ(&matRotate[0], g_Rot[0].z);							//Z����]

	//�g�k�s��Ɖ�]�s�����������Mul�ɂ����
	D3DXMatrixMultiply(&mtxScl, &mtxScl, &mtxRot);

	//Mul(�g�k�s�񁖉�]�s��)�ƈړ��s����������ă��[���h�}�g���N�X�ɂ����
	D3DXMatrixMultiply(mtxWorld, &mtxScl, &mtxPos);

	return mtxWorld;
}