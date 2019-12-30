#include "field.h"
#include "myDirect3D.h"
#include "input.h"
#include "texture.h"
#include "debugproc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

#define	VALUE_MOVE		(5.0f)							// �ړ���
#define	VALUE_ROTATE	(D3DX_PI * 0.02f)				// ��]��

#define	FIELD_WIDTH		(300.0f)			// �n�ʂ̕�(X����)
#define	FIELD_HEIGHT	(300.0f)			// �n�ʂ̍���(Y����)
#define	FIELD_DEPTH		(0.0f)			// �n�ʂ̉��s(Z����)

#define VERTEX 6 * 1 * 9	//�|���P�����̂�LIST���ƒ��_�U���K�v	* ���̂݁i�P�j���@X���{�����ɉ����o����
#define POLYGON_KAZU VERTEX / 3
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexField(LPDIRECT3DDEVICE9 pDevice);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffField = NULL;	// ���_�o�b�t�@�ւ̃|�C���^

D3DXMATRIX				g_mtxWorldField;		// ���[���h�}�g���b�N�X
D3DXVECTOR3				g_posField;				// �n�ʂ̈ʒu
D3DXVECTOR3				g_rotField;				// �n�ʂ̌���(��])
D3DXVECTOR3				g_sclField;				// �n�ʂ̑傫��(�X�P�[��)
//=========================================================================

HRESULT InitField(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();

	// ���_���̍쐬
	MakeVertexField(pDevice);

	//�ʒu�E��]�E�X�P�[���̏����ݒ�
	g_posField = D3DXVECTOR3(-1200.0f, 0.0f, 1500.0f);
	g_rotField = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_sclField = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitField(void)
{
	if (g_pVtxBuffField != NULL)
	{// ���_�o�b�t�@�̊J��
		g_pVtxBuffField->Release();
		g_pVtxBuffField = NULL;
	}
}
//=============================================================================
// �X�V����
//=============================================================================
void UpdateField(void)
{
}
//=============================================================================
// �`�揈��
//=============================================================================
void DrawField(void)
{
	LPD3DXBUFFER pD3DXMtrlBuffer = NULL;

	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	//���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&g_mtxWorldField);

	//�X�P�[���𔽉f
	D3DXMatrixScaling(&mtxScl, g_sclField.x, g_sclField.y, g_sclField.z);
	D3DXMatrixMultiply(&g_mtxWorldField, &g_mtxWorldField, &mtxScl);

	//��]�𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_rotField.y, g_rotField.z, g_rotField.x);
	D3DXMatrixMultiply(&g_mtxWorldField, &g_mtxWorldField, &mtxRot);

	//�ړ��𔽉f
	D3DXMatrixTranslation(&mtxTranslate, g_posField.x, g_posField.y, g_posField.z);
	D3DXMatrixMultiply(&g_mtxWorldField, &g_mtxWorldField, &mtxTranslate);

//	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);


	//���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldField);

	//
	pDevice->SetStreamSource(0, g_pVtxBuffField, 0, sizeof(VERTEX_3D));

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
}
//=============================================================================
// ���_�̍쐬
//=============================================================================
HRESULT MakeVertexField(LPDIRECT3DDEVICE9 pDevice)
{
	//for�����񂷂Ƃ��ɍ��W��ς���̂ɕK�v
	int cnt = 0;

	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D)* VERTEX,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffField,
		NULL)))
	{
		return E_FAIL;
	}
	{
		//���_�o�b�t�@�̒��g�𖄂߂�
		VERTEX_3D *pVtx;
		//Lock�������VRAM���烁�����̃o�b�t�@���Œ�(���A���^�C���ŏ�������͍̂T���߂ɂ��������悢�j
		//���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�̃|�C���^���Ƃ�
		g_pVtxBuffField->Lock(0, 0, (void**)&pVtx, 0);

		for (int i = 0; i < VERTEX; i = i + 6)
		{
			//==============
			//  �w�ʕ�
			//==============
			//�|�W�V����
			pVtx[i].pos = D3DXVECTOR3(-FIELD_WIDTH + FIELD_WIDTH * cnt, FIELD_HEIGHT, FIELD_DEPTH + 200.0f);
			pVtx[i + 1].pos = D3DXVECTOR3(FIELD_WIDTH + FIELD_WIDTH * cnt, FIELD_HEIGHT, FIELD_DEPTH + 200.0f);
			pVtx[i + 2].pos = D3DXVECTOR3(-FIELD_WIDTH + FIELD_WIDTH * cnt, -FIELD_HEIGHT, FIELD_DEPTH);

			pVtx[i + 3].pos = D3DXVECTOR3(-FIELD_WIDTH + FIELD_WIDTH * cnt, -FIELD_HEIGHT, FIELD_DEPTH);
			pVtx[i + 4].pos = D3DXVECTOR3(FIELD_WIDTH + FIELD_WIDTH * cnt, FIELD_HEIGHT, FIELD_DEPTH + 200.0f);
			pVtx[i + 5].pos = D3DXVECTOR3(FIELD_WIDTH + FIELD_WIDTH * cnt, -FIELD_HEIGHT, FIELD_DEPTH);

			//�@���x�N�g��
			pVtx[i].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			pVtx[i + 1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			pVtx[i + 2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

			pVtx[i + 3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			pVtx[i + 4].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			pVtx[i + 5].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

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

			cnt++;
		}
	}
		
	//������VRAM�֋A��
	g_pVtxBuffField->Unlock();

	return S_OK;
}

