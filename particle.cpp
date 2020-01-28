//=============================================================================
//
// �p�[�e�B�N������ [particle.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "debugproc.h"
#include "input.h"
#include "camera.h"
#include "particle.h"
#include "myDirect3D.h"
#include "texture.h"
#include "enemy.h"
#include <time.h>
#include "sky.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	PARTICLE_SIZE_X		(0.5f)							// �r���{�[�h�̕�
#define	PARTICLE_SIZE_Y		(0.5f)							// �r���{�[�h�̍���
#define	VALUE_MOVE_PARTICLE	(5.0f)							// �ړ����x

#define	MAX_CHARGEPARTICLE		(50)						//�`���[�W�r���{�[�h�ő吔
#define	MAX_SMOKEPARTICLE		(50)						//�����r���{�[�h�ő吔
#define	MAX_PARTICLE			(50)						//�r���{�[�h�ő吔


#define CHARGE_IN_SPEED		37								//�`���[�W�C������Ƃ��̑��x
#define CHARGE_OUT_SPEED	20								//�`���[�W�A�E�g����Ƃ��̑��x

#define SMOKE_SPEED		30								//�����̑��x

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;		// �ʒu
	D3DXVECTOR3 rot;		// ��]
	D3DXVECTOR3 scale;		// �X�P�[��
	D3DXVECTOR3 move;		// �ړ���
	D3DXCOLOR col;			// �F
	float fSizeX;			// ��
	float fSizeY;			// ����
	int nIdxShadow;			// �eID
	int nLife;				// ����
	bool bUse;				// �g�p���Ă��邩�ǂ���
} PARTICLE;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexParticle(LPDIRECT3DDEVICE9 pDevice);
void SetVertexParticle(int nIdxParticle, float fSizeX, float fSizeY);
void SetColorParticle(int nIdxParticle, D3DXCOLOR col);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
LPDIRECT3DTEXTURE9		g_pD3DTextureParticle = NULL;	// �e�N�X�`���ւ̃|�C���^
LPDIRECT3DVERTEXBUFFER9 g_pD3DVtxBuffParticle = NULL;	// ���_�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^

D3DXMATRIX				g_mtxWorldParticle;				// ���[���h�}�g���b�N�X
D3DXVECTOR3				g_PosParticle[MAX_PARTICLE];				// �n�ʂ̈ʒu
D3DXVECTOR3				g_RotParticle[MAX_PARTICLE];				// �n�ʂ̌���(��])
D3DXVECTOR3				g_SclParticle[MAX_PARTICLE];				// �n�ʂ̑傫��(�X�P�[��)

PARTICLE				g_aParticle[MAX_PARTICLE];		// �p�[�e�B�N�����[�N
D3DXVECTOR3				g_posBase;						// �r���{�[�h�����ʒu
D3DXVECTOR3				g_posBase2;						// �r���{�[�h2�����ʒu

bool					g_bPause = false;				// �|�[�YON/OFF
float					g_ParticlAlpha;					//�p�[�e�B�N���̃A���t�@�l

//�`���[�W�̊֐�
bool					g_ChargeParticle;					//�`���[�W���Ă��邩�ǂ���
int						g_ChargeCnt;					//�`���[�W����
int						g_ChargePattern;		        //�p�[�e�B�N���̓����̃p�^�[��:0 �Ȃɂ��Ȃ�:1 �p���`�`���[�W�C��:2 �p���`�`���[�W�A�E�g

//�����̊֐�
bool					g_SmokeParticle;					//�`���[�W���Ă��邩�ǂ���
int						g_SmokeCnt;					//�`���[�W����
int						g_SmokePattern;		        //�p�[�e�B�N���̓����̃p�^�[��:0 �Ȃɂ��Ȃ�:1 �p���`�`���[�W�C��:2 �p���`�`���[�W�A�E�g

//=============================================================================
// ����������
//=============================================================================
HRESULT InitParticle(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();

	// ���_���̍쐬
	MakeVertexParticle(pDevice);

	// �e�N�X�`���̓ǂݍ���
	D3DXCreateTextureFromFile(pDevice,						// �f�o�C�X�ւ̃|�C���^
		"asset/texture/effect000.jpg",			// �t�@�C���̖��O
		&g_pD3DTextureParticle);	// �ǂݍ��ރ������[

	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		g_aParticle[nCntParticle].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aParticle[nCntParticle].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aParticle[nCntParticle].scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		g_aParticle[nCntParticle].move = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		g_aParticle[nCntParticle].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		g_aParticle[nCntParticle].fSizeX = PARTICLE_SIZE_X;
		g_aParticle[nCntParticle].fSizeY = PARTICLE_SIZE_Y;
		g_aParticle[nCntParticle].nIdxShadow = -1;
		g_aParticle[nCntParticle].nLife = 0;
		g_aParticle[nCntParticle].bUse = false;
	}

	g_posBase = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_posBase2 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_ChargeParticle = false;
	g_ChargeCnt = 0;
	g_ChargePattern = 0;
	g_ParticlAlpha = 0.0f;
	
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitParticle(void)
{
	if (g_pD3DTextureParticle != NULL)
	{// �e�N�X�`���̊J��
		g_pD3DTextureParticle->Release();
		g_pD3DTextureParticle = NULL;
	}

	if (g_pD3DVtxBuffParticle != NULL)
	{// ���_�o�b�t�@�̊J��
		g_pD3DVtxBuffParticle->Release();
		g_pD3DVtxBuffParticle = NULL;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateParticle(void)
{
	//�����̏�����
	srand((unsigned)time(NULL));

	if (Keyboard_IsTrigger(DIK_6)) {
		SmokeParticle_Start();
	}

	// �J�����̉�]���擾
	CAMERA *cam = GetCamera();

	// �X�V����
	//=============================================================================
	//�`���[�W�p�[�e�B�N��
	//=============================================================================
	ChargeParticle();

	//=============================================================================
	//�����p�[�e�B�N��
	//=============================================================================
	//SandSmokeParticle();
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawParticle(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();
	D3DXMATRIX mtxView, mtxScale, mtxTranslate;
	CAMERA *cam = GetCamera();

	// ���C�e�B���O�𖳌���
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// ���Z�����ɐݒ�
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);			// ���� = �]����(SRC) + �]����(DEST)
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// ���\�[�X�J���[�̎w��
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);			// ���f�X�e�B�l�[�V�����J���[�̎w��
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);				// Z�o�b�t�@�[�̏������݂����Ȃ�
//	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);			// Z��r�Ȃ�

	//pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);	// �A���t�@�u�����f�B���O����
	//pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);	// �ŏ��̃A���t�@����
	//pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);	// �Q�Ԗڂ̃A���t�@����

	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if (g_aParticle[nCntParticle].bUse)
		{
			// ���[���h�}�g���b�N�X�̏�����
			D3DXMatrixIdentity(&g_mtxWorldParticle);

			// �r���[�}�g���b�N�X���擾
			mtxView = cam->mtxView;

			g_mtxWorldParticle._11 = mtxView._11;
			g_mtxWorldParticle._12 = mtxView._21;
			g_mtxWorldParticle._13 = mtxView._31;
			g_mtxWorldParticle._21 = mtxView._12;
			g_mtxWorldParticle._22 = mtxView._22;
			g_mtxWorldParticle._23 = mtxView._32;
			g_mtxWorldParticle._31 = mtxView._13;
			g_mtxWorldParticle._32 = mtxView._23;
			g_mtxWorldParticle._33 = mtxView._33;

			// �X�P�[���𔽉f
			D3DXMatrixScaling(&mtxScale, g_aParticle[nCntParticle].scale.x, g_aParticle[nCntParticle].scale.y, g_aParticle[nCntParticle].scale.z);
			D3DXMatrixMultiply(&g_mtxWorldParticle, &g_mtxWorldParticle, &mtxScale);

			// �ړ��𔽉f
			D3DXMatrixTranslation(&mtxTranslate, g_aParticle[nCntParticle].pos.x, g_aParticle[nCntParticle].pos.y, g_aParticle[nCntParticle].pos.z);
			D3DXMatrixMultiply(&g_mtxWorldParticle, &g_mtxWorldParticle, &mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldParticle);

			// ���_�o�b�t�@���f�o�C�X�̃f�[�^�X�g���[���Ƀo�C���h
			pDevice->SetStreamSource(0, g_pD3DVtxBuffParticle, 0, sizeof(VERTEX_3D));

			// ���_�t�H�[�}�b�g�̐ݒ�
			pDevice->SetFVF(FVF_VERTEX_3D);

			// �e�N�X�`���̐ݒ�
			pDevice->SetTexture(0, g_pD3DTextureParticle);

			// �|���S���̕`��
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, (nCntParticle * 4), NUM_POLYGON);
		}
	}

	// ���C�e�B���O��L����
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// �ʏ�u�����h�ɖ߂�
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);			// ���� = �]����(SRC) + �]����(DEST)
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// ���\�[�X�J���[�̎w��
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// ���f�X�e�B�l�[�V�����J���[�̎w��
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);				// Z�o�b�t�@�[�̏������݂�����
//	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);				// Z��r����

	//pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);	// �A���t�@�u�����f�B���O����
	//pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);		// �ŏ��̃A���t�@����
	//pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);		// �Q�Ԗڂ̃A���t�@����

}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexParticle(LPDIRECT3DDEVICE9 pDevice)
{
	// �I�u�W�F�N�g�̒��_�o�b�t�@�𐶐�
    if( FAILED( pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * NUM_VERTEX * MAX_PARTICLE,	// ���_�f�[�^�p�Ɋm�ۂ���o�b�t�@�T�C�Y(�o�C�g�P��)
												D3DUSAGE_WRITEONLY,							// ���_�o�b�t�@�̎g�p�@�@
												FVF_VERTEX_3D,								// �g�p���钸�_�t�H�[�}�b�g
												D3DPOOL_MANAGED,							// ���\�[�X�̃o�b�t�@��ێ����郁�����N���X���w��
												&g_pD3DVtxBuffParticle,					// ���_�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^
												NULL)))										// NULL�ɐݒ�
	{
        return E_FAIL;
	}

	{//���_�o�b�t�@�̒��g�𖄂߂�
		VERTEX_3D *pVtx;

		// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
		g_pD3DVtxBuffParticle->Lock(0, 0, (void**)&pVtx, 0);

		for(int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++, pVtx += 4)
		{
			// ���_���W�̐ݒ�
			pVtx[0].pos = D3DXVECTOR3(-PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);
			pVtx[1].pos = D3DXVECTOR3(PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(-PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);

			// �@���̐ݒ�
			pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

			// ���ˌ��̐ݒ�
			pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f);
			pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f);
			pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f);
			pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f);

			// �e�N�X�`�����W�̐ݒ�
			pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
			pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
			pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
			pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);
		}

		// ���_�f�[�^���A�����b�N����
		g_pD3DVtxBuffParticle->Unlock();
	}

	return S_OK;
}

//=============================================================================
// ���_���W�̐ݒ�
//=============================================================================
void SetVertexParticle(int nIdxParticle, float fSizeX, float fSizeY)
{
	{//���_�o�b�t�@�̒��g�𖄂߂�
		VERTEX_3D *pVtx;

		// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
		g_pD3DVtxBuffParticle->Lock(0, 0, (void**)&pVtx, 0);

		pVtx += (nIdxParticle * 4);

		// ���_���W�̐ݒ�
		pVtx[0].pos = D3DXVECTOR3(-fSizeX / 2, -fSizeY / 2, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(-fSizeX / 2, fSizeY / 2, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(fSizeX / 2, -fSizeY / 2, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(fSizeX / 2, fSizeY / 2, 0.0f);

		// ���_�f�[�^���A�����b�N����
		g_pD3DVtxBuffParticle->Unlock();
	}
}

//=============================================================================
// ���_�J���[�̐ݒ�
//=============================================================================
void SetColorParticle(int nIdxParticle, D3DXCOLOR col)
{
	{//���_�o�b�t�@�̒��g�𖄂߂�
		VERTEX_3D *pVtx;

		// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
		g_pD3DVtxBuffParticle->Lock(0, 0, (void**)&pVtx, 0);

		pVtx += (nIdxParticle * 4);

		// ���_���W�̐ݒ�
		pVtx[0].col =
		pVtx[1].col =
		pVtx[2].col =
		pVtx[3].col = col;

		// ���_�f�[�^���A�����b�N����
		g_pD3DVtxBuffParticle->Unlock();
	}
}

//=============================================================================
// ���_���̍쐬
//=============================================================================
int SetParticle(D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXCOLOR col, float fSizeX, float fSizeY, int nLife)
{
	int nIdxParticle = -1;

	for(int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if(!g_aParticle[nCntParticle].bUse)
		{
			g_aParticle[nCntParticle].pos = pos;
			g_aParticle[nCntParticle].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			g_aParticle[nCntParticle].scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
			g_aParticle[nCntParticle].move = move;
			g_aParticle[nCntParticle].col = col;
			g_aParticle[nCntParticle].fSizeX = fSizeX;
			g_aParticle[nCntParticle].fSizeY = fSizeY;
			g_aParticle[nCntParticle].nLife = nLife;
			g_aParticle[nCntParticle].bUse = true;

			// ���_���W�̐ݒ�
			SetVertexParticle(nCntParticle, fSizeX, fSizeY);

			nIdxParticle = nCntParticle;


			break;
		}
	}

	return nIdxParticle;
}

//�`���[�W�p�[�e�B�N��
void ChargeParticle() {
	//�x�[�X�̈ʒu���擾
	if (GetPunchIndex() == 0 || GetPunchIndex() == 2 || GetPunchIndex() == 6) {
		g_posBase = (GetEnemy() + 3)->Pos;
	}
	else if (GetPunchIndex() == 1 || GetPunchIndex() == 3 || GetPunchIndex() == 5) {
		g_posBase = (GetEnemy() + 2)->Pos;
	}
	else if (GetPunchIndex() == 4) {
		//g_posBase = (GetEnemy() + 2)->Pos;
		//g_posBase2 = (GetEnemy() + 3)->Pos;
	}
	//�`���[�W���ꂽ��
	if (g_ChargeParticle == true) {
		if (g_bPause == false)
		{
			for (int nCntParticle = 0; nCntParticle < MAX_CHARGEPARTICLE; nCntParticle++)
			{
				if (g_aParticle[nCntParticle].bUse)
				{// �g�p��
					g_aParticle[nCntParticle].pos.x += g_aParticle[nCntParticle].move.x;
					g_aParticle[nCntParticle].pos.y += g_aParticle[nCntParticle].move.y;
					g_aParticle[nCntParticle].pos.z += g_aParticle[nCntParticle].move.z;


					if (GetPunchIndex() == 4) {
						g_aParticle[nCntParticle].pos.y += (GetEnemy() + 3)->Pos.y - g_posBase2.y;
					}
					g_aParticle[nCntParticle].nLife--;

					SetColorParticle(nCntParticle, g_aParticle[nCntParticle].col);
					if (g_ChargePattern == 1) {
						SetColorParticle(nCntParticle, D3DXCOLOR(0.1f, 0.1f, 0.0f, g_ParticlAlpha));
						g_ParticlAlpha += 1.0f / (CHARGE_IN_SPEED* MAX_CHARGEPARTICLE);
					}
					if (g_ChargePattern == 2) {
						SetColorParticle(nCntParticle, D3DXCOLOR(0.1f, 0.1f, 0.0f, g_ParticlAlpha));
						g_ParticlAlpha -= 1.0f / (CHARGE_OUT_SPEED * MAX_CHARGEPARTICLE);
					}
				}
				//���C�t��0�ɂȂ�����
				if (g_aParticle[nCntParticle].nLife <= 0)
				{
					g_aParticle[nCntParticle].bUse = false;
					g_aParticle[nCntParticle].nIdxShadow = -1;
				}

			}
			//����̎�
			if (GetPunchIndex() == 4) {
				g_posBase = (GetEnemy() + 2)->Pos;
				g_posBase2 = (GetEnemy() + 3)->Pos;
			}
			////�W�����s���O�p���`�E�̎�
			//if (GetPunchIndex() == 5) {
			//	g_posBase2.y = (GetEnemy() + 2)->Pos.y;
			//}
			////�W�����s���O�p���`���̎�
			//if (GetPunchIndex() == 6) {
			//	g_posBase2.y = (GetEnemy() + 3)->Pos.y;
			//}

			if (g_ChargePattern == 1) {
				for (int nCntParticle = 0; nCntParticle < MAX_CHARGEPARTICLE; nCntParticle++)
				{

					// �p�[�e�B�N������
					D3DXVECTOR3 pos, pos2;
					D3DXVECTOR3 move, move2;
					int nLife;
					float fSize;

					//�ʒu�ݒ�
					//����
					if (GetPunchIndex() == 0 || GetPunchIndex() == 2 || GetPunchIndex() == 6) {
						pos = (GetEnemy() + 3)->Pos;
					}
					//�E��
					else if (GetPunchIndex() == 1 || GetPunchIndex() == 3 || GetPunchIndex() == 5) {
						pos = (GetEnemy() + 2)->Pos;
					}
					//����p���`
					else if (GetPunchIndex() == 4) {
						pos = (GetEnemy() + 2)->Pos;
						pos2 = (GetEnemy() + 3)->Pos;

						pos2.x += (rand() % MAX_CHARGEPARTICLE - 25);
						pos2.y += (rand() % MAX_CHARGEPARTICLE - 25);


						move2.x = (g_posBase2.x - pos2.x) / CHARGE_IN_SPEED;
						move2.y = (g_posBase2.y - pos2.y) / CHARGE_IN_SPEED;
						move2.z = 0;
					}

					pos.x += (rand() % MAX_CHARGEPARTICLE - 25);
					pos.y += (rand() % MAX_CHARGEPARTICLE - 25);
					pos.z += (rand() % MAX_CHARGEPARTICLE - 25);


					move.x = (g_posBase.x - pos.x) / CHARGE_IN_SPEED;
					move.y = (g_posBase.y - pos.y) / CHARGE_IN_SPEED;
					move.z = (g_posBase.z - pos.z) / CHARGE_IN_SPEED;

					nLife = CHARGE_IN_SPEED;

					fSize = (float)(rand() % 1 + 2);
					//fSize = fSize / 2;
					// �r���{�[�h�̐ݒ�
					SetParticle(pos, move, D3DXCOLOR(0.8f, 0.8f, 0.0f, 1.0f), fSize, fSize, nLife);
					if (GetPunchIndex() == 4) {
						SetParticle(pos2, move2, D3DXCOLOR(0.8f, 0.8f, 0.0f, 1.0f), fSize, fSize, nLife);
					}
				}
			}

			if (g_ChargePattern == 2) {
				for (int nCntParticle = 0; nCntParticle < MAX_CHARGEPARTICLE; nCntParticle++)
				{

					// �p�[�e�B�N������
					D3DXVECTOR3 pos, pos2;
					D3DXVECTOR3 move, move2;
					int nLife;
					float fSize;

					//�ʒu�ݒ�
					if (GetPunchIndex() == 0 || GetPunchIndex() == 2 || GetPunchIndex() == 6) {
						pos = (GetEnemy() + 3)->Pos;
					}
					else if (GetPunchIndex() == 1 || GetPunchIndex() == 3 || GetPunchIndex() == 5) {
						pos = (GetEnemy() + 2)->Pos;
					}
					else if (GetPunchIndex() == 4) {
						pos = (GetEnemy() + 2)->Pos;
						pos2 = (GetEnemy() + 3)->Pos;

						pos2.x += (rand() % MAX_CHARGEPARTICLE - 25);
						pos2.y += (rand() % MAX_CHARGEPARTICLE - 25);

						move2.x = (rand() % MAX_CHARGEPARTICLE - 25);
						move2.y = (rand() % MAX_CHARGEPARTICLE - 25);
						move2.z = (rand() % MAX_CHARGEPARTICLE - 25);

						move2.x = move2.x / CHARGE_OUT_SPEED;
						move2.y = move2.y / CHARGE_OUT_SPEED;
						move2.z = 0;
					}

					move.x = (rand() % MAX_CHARGEPARTICLE - 25);
					move.y = (rand() % MAX_CHARGEPARTICLE - 25);
					move.z = (rand() % MAX_CHARGEPARTICLE - 25);

					move.x = move.x / CHARGE_OUT_SPEED;
					move.y = move.y / CHARGE_OUT_SPEED;
					move.z = move.z / CHARGE_OUT_SPEED;

					nLife = CHARGE_OUT_SPEED * 2;

					fSize = (float)(rand() % 1 + 2);
					//fSize = fSize / 2;
					// �r���{�[�h�̐ݒ�
					SetParticle(pos, move, D3DXCOLOR(0.8f, 0.8f, 0.0f, 1.0f), fSize, fSize, nLife);
					if (GetPunchIndex() == 4) {
						SetParticle(pos2, move2, D3DXCOLOR(0.8f, 0.8f, 0.0f, 1.0f), fSize, fSize, nLife);
					}
				}
			}

			g_ChargeCnt++;
			if (g_ChargeCnt >= CHARGE_IN_SPEED) {
				g_ChargePattern = 2;
			}
			if (g_ChargeCnt >= CHARGE_IN_SPEED + CHARGE_OUT_SPEED * 2) {
				for (int nCntParticle = 0; nCntParticle < MAX_CHARGEPARTICLE; nCntParticle++)
				{
					g_aParticle[nCntParticle].bUse = false;
					g_aParticle[nCntParticle].nIdxShadow = -1;
				}
				g_ChargeParticle = false;
			}
		}
	}
	else {
		g_ParticlAlpha = 0.0f;
		g_ChargeCnt = 0;
	}
}
//�`���[�W��true�ɂ���֐�
void Charge_Start(){
	g_ChargeParticle = true;
	g_ChargePattern = 1;
}
//=============================================================================
//�����p�[�e�B�N��
//=============================================================================
void SandSmokeParticle() {
	if (g_SmokeParticle == true) {
		g_posBase = (GetEnemy() + 2)->Pos;

		for (int nCntParticle = 0; nCntParticle < MAX_SMOKEPARTICLE; nCntParticle++)
		{
			if (g_aParticle[nCntParticle].bUse)
			{// �g�p��
				g_aParticle[nCntParticle].pos.x += g_aParticle[nCntParticle].move.x;
				g_aParticle[nCntParticle].pos.y += g_aParticle[nCntParticle].move.y;
				g_aParticle[nCntParticle].pos.z += g_aParticle[nCntParticle].move.z;

				g_aParticle[nCntParticle].nLife--;

				SetColorParticle(nCntParticle, g_aParticle[nCntParticle].col);
				SetColorParticle(nCntParticle, D3DXCOLOR(0.1f, 0.1f, 0.0f, g_ParticlAlpha));
				g_ParticlAlpha += 1.0f / (SMOKE_SPEED* MAX_SMOKEPARTICLE);

			}
		}
		for (int nCntParticle = 0; nCntParticle < MAX_SMOKEPARTICLE; nCntParticle++)
		{

			// �p�[�e�B�N������
			D3DXVECTOR3 pos;
			D3DXVECTOR3 move;
			int nLife;
			float fSize;

			//�ʒu�ݒ�
			pos = (GetEnemy() + 2)->Pos;
	
			move.x = (rand() % 50 - 25);
			move.y = (rand() % 25 - 25);
			move.z = 0;

			move.x = move.x / SMOKE_SPEED;
			move.y = move.y / SMOKE_SPEED;
			move.z = move.z / SMOKE_SPEED;

			nLife = SMOKE_SPEED * 2;

			fSize = (float)(rand() % 1 + 2) * 0.1f;
			//fSize = fSize / 2;
			// �r���{�[�h�̐ݒ�
			SetParticle(pos, move, D3DXCOLOR(0.8f, 0.8f, 0.0f, 1.0f), fSize, fSize, nLife);

		}
		g_ChargeCnt++;

		//���C�t��0�ɂȂ�����
		if (g_SmokeCnt >= SMOKE_SPEED) {
			for (int nCntParticle = 0; nCntParticle < MAX_SMOKEPARTICLE; nCntParticle++)
			{
				g_aParticle[nCntParticle].bUse = false;
				g_aParticle[nCntParticle].nIdxShadow = -1;
			}
			g_SmokeParticle = false;
		}
	}
	else {
		g_ParticlAlpha = 0.0f;
		g_SmokeCnt = 0;
	}
}

//�����p�[�e�B�N����true�ɂ���֐�
void SmokeParticle_Start() {
	g_SmokeParticle = true;
	g_SmokePattern = 1;
}