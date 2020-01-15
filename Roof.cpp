#include "Roof.h"
#include "myDirect3D.h"
#include "input.h"
#include "texture.h"
#include "debugproc.h"
#include "enemy.h"
#include "game.h"
#include "sprite.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	VALUE_MOVE		(5.0f)					// �ړ���
#define	VALUE_ROTATE	(D3DX_PI * 0.02f)		// ��]��

#define ENEMY_HEAD		1						// �G�l�~�[�̓����w��
//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffRoof = NULL;	// ���_�o�b�t�@�ւ̃|�C���^
D3DXMATRIX				g_mtxWorldRoof;		// ���[���h�}�g���b�N�X
D3DXVECTOR3				g_posRoof;			// �V��̈ʒu
D3DXVECTOR3				g_rotRoof;			// �V��̌���(��])
D3DXVECTOR3				g_sclRoof;			// �V��̑傫��(�X�P�[��)
XMODEL					*g_enemy;			// �G�l�~�[�̍��W����肷��̂Ɏg�p

//=============================================================================
// ����������
//=============================================================================
HRESULT InitRoof(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();
	g_enemy = GetEnemy();

	// �I�u�W�F�N�g�̒��_�o�b�t�@�𐶐�
	if (FAILED(pDevice->CreateVertexBuffer
	(
		sizeof(VERTEX_3D) * NUM_VERTEX,	// ���_�f�[�^�p�Ɋm�ۂ���o�b�t�@�T�C�Y(�o�C�g�P��)
		D3DUSAGE_WRITEONLY,				// ���_�o�b�t�@�̎g�p�@�@
		FVF_VERTEX_3D,					// �g�p���钸�_�t�H�[�}�b�g
		D3DPOOL_MANAGED,				// ���\�[�X�̃o�b�t�@��ێ����郁�����N���X���w��
		&g_pVtxBuffRoof,				// ���_�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^
		NULL)))							// NULL�ɐݒ�
	{
		return E_FAIL;
	}

	{//���_�o�b�t�@�̒��g�𖄂߂�
		VERTEX_3D *pVtx;

		// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
		g_pVtxBuffRoof->Lock(0, 0, (void**)&pVtx, 0);

		// ���_���W�̐ݒ�
		pVtx[0].pos = D3DXVECTOR3(-50.0f, 50.0f, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(50.0f, 50.0f, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-50.0f, -50.0f, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(50.0f, -50.0f, 0.0f);

		// �@���̐ݒ�
		pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

		// ���_�J���[�̐ݒ�
		pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// �e�N�X�`�����W�̐ݒ�
		pVtx[0].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[1].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(1.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 0.0f);

		// ���_�f�[�^���A�����b�N����
		g_pVtxBuffRoof->Unlock();
	}

	g_posRoof = D3DXVECTOR3(0.0f, ROOF_Y, ROOF_Z);
	g_sclRoof = D3DXVECTOR3(1.0f, 1.0f, 1.5f);

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
	g_enemy = GetEnemy();

	DebugProc_Print((char*)"enemy.x:%f,enemy.y:%f,enemy.z:%f", (g_enemy + ENEMY_HEAD)->Pos.x, (g_enemy + ENEMY_HEAD)->Pos.y, (g_enemy + ENEMY_HEAD)->Pos.z);
}
//=============================================================================
// �`�揈���Ă񂶂傤
//=============================================================================
void DrawRoof(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();
	D3DXMATRIX mtxView, mtxScale, mtxTranslate;

	//�A���t�@�e�X�g��L����
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	//���C�e�B���O�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	//���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&g_mtxWorldRoof);

	//�r���[�}�g���b�N�X���擾
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	//POLYGON�𐳖ʂɌ�����
#if 1
	//�t�s������߂�
	D3DXMatrixInverse(&g_mtxWorldRoof, NULL, &mtxView);
	g_mtxWorldRoof._41 = 0.0f;
	g_mtxWorldRoof._42 = 0.0f;
	g_mtxWorldRoof._43 = 0.0f;
#else
	g_mtxWorldRoof._11 = mtxViex._11;
	g_mtxWorldRoof._12 = mtxViex._21;
	g_mtxWorldRoof._13 = mtxViex._31;
	g_mtxWorldRoof._21 = mtxViex._12;
	g_mtxWorldRoof._22 = mtxViex._22;
	g_mtxWorldRoof._23 = mtxViex._32;
	g_mtxWorldRoof._31 = mtxViex._13;
	g_mtxWorldRoof._32 = mtxViex._23;
	g_mtxWorldRoof._33 = mtxViex._33;
#endif

	//�X�P�[���𔽉f
	D3DXMatrixScaling(&mtxScale, g_sclRoof.x, g_sclRoof.y, g_sclRoof.z);
	D3DXMatrixMultiply(&g_mtxWorldRoof, &g_mtxWorldRoof, &mtxScale);

	//�ړ��𔽉f
	D3DXMatrixTranslation(&mtxTranslate, g_posRoof.x, g_posRoof.y, g_posRoof.z);
	D3DXMatrixMultiply(&g_mtxWorldRoof, &g_mtxWorldRoof, &mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldRoof);

	//���_�o�b�t�@���f�o�C�X�̃f�[�^�X�g���[���Ƀo�C���h
	pDevice->SetStreamSource(0, g_pVtxBuffRoof, 0, sizeof(VERTEX_3D));

	//���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_3D);

	//�e�N�X�`���̐ݒ�
	if ((g_enemy + ENEMY_HEAD)->Pos.y < ROOF_Y)
	{
		pDevice->SetTexture(0, Texture_GetTexture(TEXTURE_INDEX_ROOF_STD));
	}
	if ((g_enemy + ENEMY_HEAD)->Pos.y >= ROOF_Y)
	{
		pDevice->SetTexture(0, Texture_GetTexture(TEXTURE_INDEX_ROOF_BROKE));
	}


	//POLYGON�̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 12);

	//���C�e�B���O��L���ɂ���
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	//�A���t�@�e�X�g�𖳌���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);


	//================================================

}