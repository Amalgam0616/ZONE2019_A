#include "field_yoko.h"
#include "myDirect3D.h"
#include "input.h"
#include "texture.h"
#include "debugproc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

#define	VALUE_MOVE		(5.0f)							// �ړ���
#define	VALUE_ROTATE	(D3DX_PI * 0.02f)				// ��]��

#define	FIELD_WIDTH		(800.0f)						// �n�ʂ̕�(X����)
#define	FIELD_HEIGHT	(300.0f)						// �n�ʂ̍���(Y����)
#define	FIELD_DEPTH		(300.0f)					// �n�ʂ̉��s(Z����)

#define VERTEX 6 * 2 * 5	//�|���P�����̂�LIST���ƒ��_�U���K�v	* ���E�i�Q�j���@Z���{�����ɉ����o����
#define POLYGON_KAZU VERTEX / 3
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexField_Yoko(LPDIRECT3DDEVICE9 pDevice);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffField_yoko = NULL;	// ���_�o�b�t�@�ւ̃|�C���^

D3DXMATRIX				g_mtxWorldField_yoko;			// ���[���h�}�g���b�N�X
D3DXVECTOR3				g_posField_yoko;				// �n�ʂ̈ʒu
D3DXVECTOR3				g_rotField_yoko;				// �n�ʂ̌���(��])
D3DXVECTOR3				g_sclField_yoko;				// �n�ʂ̑傫��(�X�P�[��)
//=========================================================================

HRESULT InitField_Yoko(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();

	// ���_���̍쐬
	MakeVertexField_Yoko(pDevice);

	// �ʒu�E��]�E�X�P�[���̏����ݒ�
	g_posField_yoko = D3DXVECTOR3(0.0f, 0.0f, 200.0f);
	g_rotField_yoko = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_sclField_yoko = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitField_Yoko(void)
{
	if (g_pVtxBuffField_yoko != NULL)
	{// ���_�o�b�t�@�̊J��
		g_pVtxBuffField_yoko->Release();
		g_pVtxBuffField_yoko = NULL;
	}
}
//=============================================================================
// �X�V����
//=============================================================================
void UpdateField_Yoko(void)
{

}
//=============================================================================
// �`�揈��
//=============================================================================
void DrawField_Yoko(void)
{
	LPD3DXBUFFER pD3DXMtrlBuffer = NULL;

	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	//���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&g_mtxWorldField_yoko);

	//�X�P�[���𔽉f
	D3DXMatrixScaling(&mtxScl, g_sclField_yoko.x, g_sclField_yoko.y, g_sclField_yoko.z);
	D3DXMatrixMultiply(&g_mtxWorldField_yoko, &g_mtxWorldField_yoko, &mtxScl);

	//��]�𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_rotField_yoko.y, g_rotField_yoko.z, g_rotField_yoko.x);
	D3DXMatrixMultiply(&g_mtxWorldField_yoko, &g_mtxWorldField_yoko, &mtxRot);

	//�ړ��𔽉f
	D3DXMatrixTranslation(&mtxTranslate, g_posField_yoko.x, g_posField_yoko.y, g_posField_yoko.z);
	D3DXMatrixMultiply(&g_mtxWorldField_yoko, &g_mtxWorldField_yoko, &mtxTranslate);

//	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);


	//���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldField_yoko);

	//
	pDevice->SetStreamSource(0, g_pVtxBuffField_yoko, 0, sizeof(VERTEX_3D));

	pDevice->SetFVF(FVF_VERTEX_3D);

	D3DMATERIAL9 mat = {};

	mat.Diffuse.r = 1.0f;
	mat.Diffuse.g = 1.0f;
	mat.Diffuse.b = 1.0f;
	mat.Diffuse.a = 1.0f;

	mat.Ambient.r = 1.0f;
	mat.Ambient.g = 1.0f;
	mat.Ambient.b = 1.0f;
	mat.Ambient.a = 1.0f;

	//�}�e���A���Z�b�g
	pDevice->SetMaterial(&mat);

	//�e�N�X�`���̃Z�b�g
	pDevice->SetTexture(0, Texture_GetTexture(TEXTURE_INDEX_AUDIENCE));
	
	//�|���S���̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, POLYGON_KAZU);

	////pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}
//=============================================================================
// ���_�̍쐬
//=============================================================================
HRESULT MakeVertexField_Yoko(LPDIRECT3DDEVICE9 pDevice)
{
	//for�����񂷂̂ɗ��p,pos�̑����
	int cnt = 0;

	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D)* VERTEX,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffField_yoko,
		NULL)))
	{
		return E_FAIL;
	}
	{
		//���_�o�b�t�@�̒��g�𖄂߂�
		VERTEX_3D *pVtx;
		//Lock�������VRAM���烁�����̃o�b�t�@���Œ�(���A���^�C���ŏ�������͍̂T���߂ɂ��������悢�j
		//���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�̃|�C���^���Ƃ�
		g_pVtxBuffField_yoko->Lock(0, 0, (void**)&pVtx, 0);

		/*
			�ق�Ƃ͂���ȏ������������Ȃ����ǎd���Ȃ���
		*/
		//�|�W�V����
		for (int i = 0; i < VERTEX; i = i + 12)
		{
			//========
			//  �E��
			//========
			//�|�W�V����
			pVtx[i].pos = D3DXVECTOR3(FIELD_WIDTH + 200.0f, FIELD_HEIGHT, FIELD_DEPTH + FIELD_DEPTH * cnt);
			pVtx[i + 1].pos = D3DXVECTOR3(FIELD_WIDTH + 200.0f, FIELD_HEIGHT, -FIELD_DEPTH + FIELD_DEPTH * cnt);
			pVtx[i + 2].pos = D3DXVECTOR3(FIELD_WIDTH, -FIELD_HEIGHT, FIELD_DEPTH + FIELD_DEPTH * cnt);

			pVtx[i + 3].pos = D3DXVECTOR3(FIELD_WIDTH, -FIELD_HEIGHT, FIELD_DEPTH + FIELD_DEPTH * cnt);
			pVtx[i + 4].pos = D3DXVECTOR3(FIELD_WIDTH + 200.0f, FIELD_HEIGHT, -FIELD_DEPTH + FIELD_DEPTH * cnt);
			pVtx[i + 5].pos = D3DXVECTOR3(FIELD_WIDTH, -FIELD_HEIGHT, -FIELD_DEPTH + FIELD_DEPTH * cnt);

			//�@���x�N�g��
			pVtx[i].nor = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
			pVtx[i + 1].nor = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
			pVtx[i + 2].nor = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

			pVtx[i + 3].nor = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
			pVtx[i + 4].nor = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
			pVtx[i + 5].nor = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

			//���_�J���[
			pVtx[i].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[i + 1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[i + 2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

			pVtx[i + 3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[i + 4].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[i + 5].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

			//�e�N�X�`��
			pVtx[i].tex = D3DXVECTOR2(0.0f, 0.0f);
			pVtx[i + 1].tex = D3DXVECTOR2(1.0f, 0.0f);
			pVtx[i + 2].tex = D3DXVECTOR2(0.0f, 0.5f);

			pVtx[i + 3].tex = D3DXVECTOR2(0.0f, 0.5f);
			pVtx[i + 4].tex = D3DXVECTOR2(1.0f, 0.0f);
			pVtx[i + 5].tex = D3DXVECTOR2(1.0f, 0.5f);

			//========
			//  ����
			//========
			//�|�W�V����
			pVtx[i + 6].pos = D3DXVECTOR3(-FIELD_WIDTH - 200.0f, FIELD_HEIGHT, -FIELD_DEPTH + FIELD_DEPTH * cnt);
			pVtx[i + 7].pos = D3DXVECTOR3(-FIELD_WIDTH - 200.0f, FIELD_HEIGHT, FIELD_DEPTH + FIELD_DEPTH * cnt);
			pVtx[i + 8].pos = D3DXVECTOR3(-FIELD_WIDTH, -FIELD_HEIGHT, -FIELD_DEPTH + FIELD_DEPTH * cnt);

			pVtx[i + 9].pos = D3DXVECTOR3(-FIELD_WIDTH, -FIELD_HEIGHT, -FIELD_DEPTH + FIELD_DEPTH * cnt);
			pVtx[i + 10].pos = D3DXVECTOR3(-FIELD_WIDTH - 200.0f, FIELD_HEIGHT, FIELD_DEPTH + FIELD_DEPTH * cnt);
			pVtx[i + 11].pos = D3DXVECTOR3(-FIELD_WIDTH, -FIELD_HEIGHT, FIELD_DEPTH + FIELD_DEPTH * cnt);

			//�@���x�N�g��
			pVtx[i + 6].nor = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
			pVtx[i + 7].nor = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
			pVtx[i + 8].nor = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);

			pVtx[i + 9].nor = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
			pVtx[i + 10].nor = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
			pVtx[i + 11].nor = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);

			//���_�J���[
			pVtx[i + 6].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[i + 7].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[i + 8].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

			pVtx[i + 9].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[i + 10].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[i + 11].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

			//�e�N�X�`��
			pVtx[i + 6].tex = D3DXVECTOR2(0.0f, 0.0f);
			pVtx[i + 7].tex = D3DXVECTOR2(1.0f, 0.0f);
			pVtx[i + 8].tex = D3DXVECTOR2(0.0f, 0.5f);

			pVtx[i + 9].tex = D3DXVECTOR2(0.0f, 0.5f);
			pVtx[i + 10].tex = D3DXVECTOR2(1.0f, 0.0f);
			pVtx[i + 11].tex = D3DXVECTOR2(1.0f, 0.5f);

			cnt++;
		}

		//������VRAM�֋A��
		g_pVtxBuffField_yoko->Unlock();
	}
	
	return S_OK;
}

