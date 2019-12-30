#include "Roof.h"
#include "myDirect3D.h"
#include "input.h"
#include "texture.h"
#include "debugproc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

#define	VALUE_MOVE		(5.0f)							// �ړ���
#define	VALUE_ROTATE	(D3DX_PI * 0.02f)				// ��]��

#define	FIELD_WIDTH		(50.0f)						// �n�ʂ̕�(X����)
#define	FIELD_HEIGHT	(20.0f)						// �n�ʂ̕�(X����)
#define	FIELD_DEPTH		(30.0f)					// �n�ʂ̉��s(Z����)


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexRoof(LPDIRECT3DDEVICE9 pDevice);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffRoof = NULL;	// ���_�o�b�t�@�ւ̃|�C���^

//=========================================================================
D3DXMATRIX				g_mtxWorldRoof;		// ���[���h�}�g���b�N�X
D3DXVECTOR3				g_posRoof;			// �n�ʂ̈ʒu
D3DXVECTOR3				g_rotRoof;			// �n�ʂ̌���(��])
D3DXVECTOR3				g_sclRoof;			// �n�ʂ̑傫��(�X�P�[��)

HRESULT InitRoof(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();

	MakeVertexRoof(pDevice);

	// �ʒu�E��]�E�X�P�[���̏����ݒ�
	g_posRoof = D3DXVECTOR3(0.0f, 50.0f, 0.0f);
	g_rotRoof = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_sclRoof = D3DXVECTOR3(8.0f, 1.0f, 20.0f);

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void UninitRoof(void)
{
	if (g_pVtxBuffRoof != NULL)
	{// ���_�o�b�t�@�̊J��
		g_pVtxBuffRoof->Release();
		g_pVtxBuffRoof = NULL;
	}

}
//=============================================================================
// �X�V����
//=============================================================================
void UpdateRoof(void)
{

}
//=============================================================================
// �`�揈���Ă񂶂傤
//=============================================================================
void DrawRoof(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	//�e�N�X�`���𗠕\�`�悷��֐�
	//pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&g_mtxWorldRoof);

	//�X�P�[���𔽉f
	D3DXMatrixScaling(&mtxScl, g_sclRoof.x, g_sclRoof.y, g_sclRoof.z);
	D3DXMatrixMultiply(&g_mtxWorldRoof, &g_mtxWorldRoof, &mtxScl);

	//��]�𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_rotRoof.y, g_rotRoof.z, g_rotRoof.x);
	D3DXMatrixMultiply(&g_mtxWorldRoof, &g_mtxWorldRoof, &mtxRot);

	//�ړ��𔽉f
	D3DXMatrixTranslation(&mtxTranslate, g_posRoof.x, g_posRoof.y, g_posRoof.z);
	D3DXMatrixMultiply(&g_mtxWorldRoof, &g_mtxWorldRoof, &mtxTranslate);

	//���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldRoof);

	//
	pDevice->SetStreamSource(0, g_pVtxBuffRoof, 0, sizeof(VERTEX_3D));

	//
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
	pDevice->SetTexture(NULL,NULL);
	//pDevice->SetTexture(0, Texture_GetTexture(TEXTURE_INDEX_ROOF_BROKE));

	//�|���S���̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 6);

}
//=============================================================================
// ���_�̍쐬�Ă񂶂傤
//=============================================================================
HRESULT MakeVertexRoof(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 6,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffRoof,
		NULL)))
	{
		return E_FAIL;
	}
	{
		//���_�o�b�t�@�̒��g�𖄂߂�
		VERTEX_3D *pVtx;
		//Lock�������VRAM���烁�����̃o�b�t�@���Œ�(���A���^�C���ŏ�������͍̂T���߂ɂ��������悢�j
		//���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�̃|�C���^���Ƃ�
		g_pVtxBuffRoof->Lock(0, 0, (void**)&pVtx, 0);

		//�|�W�V����
		pVtx[0].pos = D3DXVECTOR3(-FIELD_WIDTH, FIELD_HEIGHT, FIELD_DEPTH);
		pVtx[1].pos = D3DXVECTOR3(FIELD_WIDTH, FIELD_HEIGHT, FIELD_DEPTH);
		pVtx[2].pos = D3DXVECTOR3(-FIELD_WIDTH, FIELD_HEIGHT, -FIELD_DEPTH);

		pVtx[3].pos = D3DXVECTOR3(-FIELD_WIDTH, FIELD_HEIGHT, -FIELD_DEPTH);
		pVtx[4].pos = D3DXVECTOR3(FIELD_WIDTH, FIELD_HEIGHT, FIELD_DEPTH);
		pVtx[5].pos = D3DXVECTOR3(FIELD_WIDTH, FIELD_HEIGHT, -FIELD_DEPTH);

		//�@���x�N�g��
		pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[4].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[5].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		//�J���[
		pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[4].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[5].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		//�e�N�X�`��
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);

		pVtx[3].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[4].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[5].tex = D3DXVECTOR2(1.0f, 1.0f);


		//������VRAM�֋A��
		g_pVtxBuffRoof->Unlock();
	}

	return S_OK;

}