#include "main.h"
#include "field.h"
#include "myDirect3D.h"
#include "input.h"
#include "texture.h"
#include "debugproc.h"
#include "sprite.h"
#include "player.h"
#include "enemy.h"
#include "Effect.h"	
//*****************************************************************************
// �}�N����`
//*****************************************************************************

#define	VALUE_MOVE		(5.0f)							// �ړ���
#define	VALUE_ROTATE	(D3DX_PI * 0.02f)				// ��]��

#define	HIT_EFFECT_WIDTH	(20.0f)						// �G�t�F�N�g�̕�(X����)
#define	HIT_EFFECT_HEIGHT	(20.0f)						// �G�t�F�N�g�̕�(X����)
														 
#define	WIND_EFFECT_WIDTH	(60.0f)						// �G�t�F�N�g�̕�(X����)
#define	WIND_EFFECT_HEIGHT	(60.0f)						// �G�t�F�N�g�̕�(X����)

#define	EFFECT_DEPTH	(10.0f)						// �n�ʂ̉��s(Z����)
#define HIT_EFFECT_POSX (-10.0f)					//�q�b�g�G�t�F�N�g��X�ʒu
#define HIT_EFFECT_POSZ (-20.0f)					//�q�b�g�G�t�F�N�g��Z�ʒu

#define EFFECT_SIZEX (1.0f)
#define EFFECT_SIZEY (1.0f)

#define EFFECT_MAX			3						//�G�t�F�N�g�̐��A�����ƕύX����
#define EFFECT_SHRINK_SPEED 1						//�G�t�F�N�g�̏k�܂�X�s�[�h
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

//****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffEffect[EFFECT_MAX] = {};// ���_�o�b�t�@�ւ̃|�C���^

D3DXMATRIX				g_mtxWorldEffect[EFFECT_MAX];		// ���[���h�}�g���b�N�X
D3DXVECTOR3				g_PosEffect[EFFECT_MAX];				// �n�ʂ̈ʒu
D3DXVECTOR3				g_RotEffect[EFFECT_MAX];				// �n�ʂ̌���(��])
D3DXVECTOR3				g_SclEffect[EFFECT_MAX];				// �n�ʂ̑傫��(�X�P�[��)

//�`���[�W����Ƃ��̕ϐ�
float g_Charge_Animation_3d_X;		 //�`���[�W�A�j���[�V������U�l
float g_Charge_Animation_3d_Y;		 //�`���[�W�A�j���[�V������V�l
float g_Charge_Animation_U;			 //�`���[�W�A�j���[�V������U�̈ړ�����l
float g_Charge_Animation_V;			 //�`���[�W�A�j���[�V������V�̈ړ�����l
bool g_Charge_Animation_Flg;		 //�`���[�W�A�j���[�V�������I���ɂ���t���O
int	  g_Charge_Animation_Rag_Cnt;	 //�G�t�F�N�g��x������J�E���g
bool  g_AnimationShrink_Flg;		 //�t�G�t�F�N�g�A�j���[�V�������I������t���O
bool   g_NumberResetFlg;			 //�A�j���[�V�������Ƃɒl��ς���t���O

//�p���`�̋��؂�G�t�F�N�g
float g_Wind_Animation_3d_X;		 //�p���`�����؂�A�j���[�V������U�l
float g_Wind_Animation_3d_Y;		 //�p���`�����؂�A�j���[�V������V�l
float g_Wind_Animation_U;			 //�p���`�����؂�A�j���[�V������U�̈ړ�����l
float g_Wind_Animation_V;			 //�p���`�����؂�A�j���[�V������V�̈ړ�����l
bool  g_Wind_Animation_Flg;		 //�`���[�W�A�j���[�V�������I���ɂ���t���O

//�p���`�̃q�b�g�G�t�F�N�g
float g_Hit_Animation_3d_X;		 //�p���`�����؂�A�j���[�V������U�l
float g_Hit_Animation_3d_Y;		 //�p���`�����؂�A�j���[�V������V�l
float g_Hit_Animation_U;		 //�p���`�����؂�A�j���[�V������U�̈ړ�����l
float g_Hit_Animation_V;		 //�p���`�����؂�A�j���[�V������V�̈ړ�����l
bool  g_Hit_Animation_Flg;		 //�`���[�W�A�j���[�V�������I���ɂ���t���O


float AnimationX = 0.0f;
float AnimationY = 0.0f;

//�p���`�̎��
typedef enum {
	PUNCH_HIT_EFFECT = 0,
	PUNCH_CUT_THE_SKY,

	MAX_EFFECT,
}EFFECT;

int   g_Punch_Val;					 //�p���`�̃G�t�F�N�g���

//�X���[�̎��̕ϐ�
int g_SlowEffectCnt;
bool g_SlowFlg;
int g_Player_y;
//=============================================================================
// �\���̐錾
//=============================================================================
typedef struct
{
	D3DXVECTOR3 Effect_pos;	// ���_���W
	D3DXVECTOR3 Effect_nor;	// �@���x�N�g��
	D3DCOLOR	Effect_col;	// ���_�J���[
	D3DXVECTOR2 Effect_tex;	// �e�N�X�`�����W
}EFFECT_3D;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitEffect(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();

	MakeVertexChargeEffect(pDevice);
	MakeVertexWindEffect(pDevice);
	MakeVertexHitEffect(pDevice);

	// �ʒu�E��]�E�X�P�[���̏����ݒ�
	g_PosEffect[0] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_RotEffect[0] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_SclEffect[0] = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	//�q�b�g�G�t�F�N�g�̃A�j���[�V�����̏����l
	g_Charge_Animation_U = 0.25f;
	g_Charge_Animation_V = 0.125f;
	g_Charge_Animation_3d_X = 0.75f;
	g_Charge_Animation_3d_Y = 0.875f;
	g_Charge_Animation_Rag_Cnt = 0;
	g_AnimationShrink_Flg = false;
	g_NumberResetFlg = true;
	g_Punch_Val = 0;

	//�p���`�����؂�G�t�F�N�g�̏�����
	g_Wind_Animation_3d_X = 0.0f;
	g_Wind_Animation_3d_Y = 0.0f;
	g_Wind_Animation_U = 0.25f;
	g_Wind_Animation_V = 0.125f;
	g_Wind_Animation_Flg = false;

	//�p���`�̃q�b�g�G�t�F�N�g�̏�����
	g_Hit_Animation_3d_X = 0.0f;
	g_Hit_Animation_3d_Y = 0.0f;
	g_Hit_Animation_U = 0.25f;
	g_Hit_Animation_V = 0.125f;
	g_Hit_Animation_Flg = false;

	//

	//for (int i = 0; i < EFFECT_MAX; i++) {
	//	g_Charge_Animation_3d_X[i] = 0.25f *(rand() % 3);
	//	g_Charge_Animation_3d_Y[i] = 0.125f*(rand() % 7);
	//}



	g_SlowEffectCnt = 0;
	g_SlowFlg = false;
	g_Charge_Animation_Flg = false;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEffect(void)
{
	for (int i = 0; i < EFFECT_MAX; i++) {
		if (g_pVtxBuffEffect[i] != NULL)
		{// ���_�o�b�t�@�̊J��
			g_pVtxBuffEffect[i]->Release();
			g_pVtxBuffEffect[i] = NULL;
		}
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateEffect(void)
{
	//�G�t�F�N�g��`�悳���鏈��
	//if (Keyboard_IsTrigger(DIK_1) || g_Charge_Animation_Flg == true) {
	//	g_Charge_Animation_Flg = true;
	//	g_Punch_Val = 0;
	//}

	//�G�t�F�N�g��`�悳���鏈��
	if (Keyboard_IsTrigger(DIK_2) || g_Wind_Animation_Flg == true) {
		g_Wind_Animation_Flg = true;
		g_Punch_Val = 2;
	}

	//�G�t�F�N�g��`�悳���鏈��
	if (Keyboard_IsTrigger(DIK_4) || g_Wind_Animation_Flg == true) {
		g_Hit_Animation_Flg = true;
		g_Punch_Val = 0;
	}

	Punch_Charge_Effect_Animation();
	Punch_Wind_Effect_Animation();
	Punch_Hit_Effect_Animation();
	
	//�X���[�̃f�o�b�O
	if (Keyboard_IsPress(DIK_5)) {
		g_SlowFlg = true;
		g_SlowEffectCnt += 20;
		if (g_SlowEffectCnt >= 250) {
			g_SlowEffectCnt = 250;
		}
	}


	else {
		g_SlowFlg = false;
		g_SlowEffectCnt = 0;
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEffect(void)
{
	if (g_Charge_Animation_Flg == true || g_AnimationShrink_Flg == true || g_Wind_Animation_Flg == true || g_Hit_Animation_Flg == true) {
		LPD3DXBUFFER pD3DXMtrlBuffer = NULL;

		LPDIRECT3DDEVICE9 pEffectDevice = GetD3DDevice();
		D3DXMATRIX EffectMtxScl[EFFECT_MAX], EffectMtxRot[EFFECT_MAX], EffectMtxTranslate[EFFECT_MAX];

		//�e�N�X�`���𗠕\�`�悷��֐�
		//��������Ƃ��낢�남�������Ȃ�
		//pEffectDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

		//���e�X�g��L����
		pEffectDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		pEffectDevice->SetRenderState(D3DRS_ALPHAREF, 0);
		pEffectDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

		for (int j = 0; j < EFFECT_MAX; j++) {
			//���[���h�}�g���b�N�X�̏�����
			D3DXMatrixIdentity(&g_mtxWorldEffect[j]);

			//�X�P�[���𔽉f
			D3DXMatrixScaling(&EffectMtxScl[j], g_SclEffect[j].x, g_SclEffect[j].y, g_SclEffect[j].z);
			D3DXMatrixMultiply(&g_mtxWorldEffect[j], &g_mtxWorldEffect[j], &EffectMtxScl[j]);

			//��]�𔽉f
			D3DXMatrixRotationYawPitchRoll(&EffectMtxRot[j], g_RotEffect[j].x, g_RotEffect[j].y, g_RotEffect[j].z);
			D3DXMatrixMultiply(&g_mtxWorldEffect[j], &g_mtxWorldEffect[j], &EffectMtxRot[j]);

			//�ړ��𔽉f
			D3DXMatrixTranslation(&EffectMtxTranslate[j], g_PosEffect[j].x, g_PosEffect[j].y, g_PosEffect[j].z);
			D3DXMatrixMultiply(&g_mtxWorldEffect[j], &g_mtxWorldEffect[j], &EffectMtxTranslate[j]);

			//���[���h�}�g���b�N�X�̐ݒ�
			pEffectDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldEffect[j]);

			//
			pEffectDevice->SetStreamSource(0, g_pVtxBuffEffect[j], 0, sizeof(VERTEX_3D));

			//
			pEffectDevice->SetFVF(FVF_VERTEX_3D);
			//�}�e���A���̐ݒ�
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
			pEffectDevice->SetMaterial(&mat);

			if (g_Punch_Val == 0) {
				//�e�N�X�`���̐ݒ�
				pEffectDevice->SetTexture(0, Texture_GetTexture(TEXTURE_INDEX_HIT_EFFECT));
			}
			else if (g_Punch_Val == 1) {
				pEffectDevice->SetTexture(0, Texture_GetTexture(TEXTURE_INDEX_CUT_THE_SKY_SIDE));
			}
			else if (g_Punch_Val == 2) {
				pEffectDevice->SetTexture(0, Texture_GetTexture(TEXTURE_INDEX_CUT_THE_SKY_FRONT));
			}
			//�|���S���̕`��
			pEffectDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, NUM_POLYGON);
		}
		//���e�X�g��L����
		pEffectDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	}
}



void SlowEffect() {
	if (g_SlowFlg == true) {
		Sprite_Draw(TEXTURE_INDEX_SLOW_EFFECT,
			SCREEN_WIDTH / 2,
			SCREEN_HEIGHT / 2,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			SCREEN_WIDTH / 2,
			SCREEN_HEIGHT / 2,
			1.0f,
			1.0f,
			0.0f,
			g_SlowEffectCnt);
	}
}

void DamageEffect() {
	Sprite_Draw(TEXTURE_INDEX_DAMAGE_EFFECT,
		SCREEN_WIDTH / 2,
		SCREEN_HEIGHT / 2,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		SCREEN_WIDTH / 2,
		SCREEN_HEIGHT / 2,
		1.0f,
		1.0f,
		0.0f,
		255);
}


//============================================================================
//�p���`�̃`���[�W����Ƃ��̃G�t�F�N�g
//============================================================================
//=============================================================================
// ���_�̍쐬
//=============================================================================
HRESULT MakeVertexChargeEffect(LPDIRECT3DDEVICE9 pDevice)
{
	for (int i = 0; i < EFFECT_MAX; i++) {
		if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D)*NUM_VERTEX,
			D3DUSAGE_WRITEONLY,
			FVF_VERTEX_3D,
			D3DPOOL_MANAGED,
			&g_pVtxBuffEffect[i],
			NULL)))
		{
			return E_FAIL;
		}
		{
			//���_�o�b�t�@�̔��𖄂߂�
			//VERTEX_3D *pVtx;
			EFFECT_3D *pVtx;
			//VRAM��̏��������Ă��邩��A���_���������悤�ɂȂ�
			g_pVtxBuffEffect[i]->Lock(0, 0, (void**)&pVtx, 0);

			//�ʒu
			pVtx[0].Effect_pos = D3DXVECTOR3(0, HIT_EFFECT_HEIGHT, 0);
			pVtx[1].Effect_pos = D3DXVECTOR3(HIT_EFFECT_WIDTH, HIT_EFFECT_HEIGHT, 0);
			pVtx[2].Effect_pos = D3DXVECTOR3(0, 0, 0);
			pVtx[3].Effect_pos = D3DXVECTOR3(HIT_EFFECT_WIDTH, 0, 0);

			//�@���A�F
			for (int i = 0; i < 4; i++) {
				pVtx[i].Effect_nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
				pVtx[i].Effect_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			}

			//�e�N�X�`��
			pVtx[0].Effect_tex = D3DXVECTOR2(g_Charge_Animation_3d_X, g_Charge_Animation_3d_Y);
			pVtx[1].Effect_tex = D3DXVECTOR2(g_Charge_Animation_3d_X + g_Charge_Animation_U, g_Charge_Animation_3d_Y);
			pVtx[2].Effect_tex = D3DXVECTOR2(g_Charge_Animation_3d_X, g_Charge_Animation_3d_Y + g_Charge_Animation_V);
			pVtx[3].Effect_tex = D3DXVECTOR2(g_Charge_Animation_3d_X + g_Charge_Animation_U, g_Charge_Animation_3d_Y + g_Charge_Animation_V);

			//��]
			g_RotEffect[0] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

			//������VRAM�֋A��
			g_pVtxBuffEffect[i]->Unlock();
		}
	}
	return S_OK;
}

void Punch_Charge_Effect_Animation() {
	//�t�ɕ`�悷�鏈��
	if (g_Charge_Animation_Flg == true) {
		if (g_NumberResetFlg == true) {
			g_Charge_Animation_3d_X = 0.75f;
			g_Charge_Animation_3d_Y = 0.875f;
			g_NumberResetFlg = false;
		}
		g_Charge_Animation_Rag_Cnt++;
		if (g_Charge_Animation_Rag_Cnt >= EFFECT_SHRINK_SPEED) {
			//��[�܂ōs���ĂȂ��Ƃ�
			if (g_Charge_Animation_3d_Y >= 0.0f) {
				//U�l�����炷
				g_Charge_Animation_3d_X -= g_Charge_Animation_U;

				//U�l���E�[�֍s�����Ƃ�
				if (g_Charge_Animation_3d_X < 0.0f) {
					g_Charge_Animation_3d_Y -= g_Charge_Animation_V;	//V�l��1���炷
					g_Charge_Animation_3d_X = 0.75f;	//U�l�����[��
				}
			}
			//V�l����[�֍s�����Ƃ�
			if (g_Charge_Animation_3d_Y < 0.0f) {
				g_Charge_Animation_Rag_Cnt++;
				//20F�̃��O�����
				if (g_Charge_Animation_Rag_Cnt >= 10) {
					g_Charge_Animation_3d_X = 0.0f;
					g_Charge_Animation_3d_Y = 0.0f;
					g_AnimationShrink_Flg = true;
					g_NumberResetFlg = true;
					g_Charge_Animation_Flg = false;
				}
			}
			else {
				g_Charge_Animation_Rag_Cnt = 0;
			}
			//�`��
			for (int i = 0; i < EFFECT_MAX; i++) {
				//���_�o�b�t�@�̔��𖄂߂�
				//VERTEX_3D *pVtx;
				EFFECT_3D *pVtx;

				//VRAM��̏��������Ă��邩��A���_���������悤�ɂȂ�
				g_pVtxBuffEffect[i]->Lock(0, 0, (void**)&pVtx, 0);
				//�ʒu
				pVtx[0].Effect_pos = D3DXVECTOR3(0, HIT_EFFECT_HEIGHT, 0);
				pVtx[1].Effect_pos = D3DXVECTOR3(HIT_EFFECT_WIDTH, HIT_EFFECT_HEIGHT, 0);
				pVtx[2].Effect_pos = D3DXVECTOR3(0, 0, 0);
				pVtx[3].Effect_pos = D3DXVECTOR3(HIT_EFFECT_WIDTH, 0, 0);

				//�ʒu(����)
				if (GetPunchIndex() == 0|| GetPunchIndex() == 2) {
					g_PosEffect[0] = D3DXVECTOR3((GetEnemy() + 3)->Pos.x - 10, (GetEnemy() + 3)->Pos.y - 9.0, (GetEnemy() + 3)->Pos.z);
					g_SclEffect[1] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
				}
				//�E��
				else if (GetPunchIndex() == 1|| GetPunchIndex()==3) {
					g_PosEffect[0] = D3DXVECTOR3((GetEnemy() + 2)->Pos.x - 10, (GetEnemy() + 2)->Pos.y - 9.0, (GetEnemy() + 2)->Pos.z);
					g_SclEffect[1] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
				}
				//����
				else if (GetPunchIndex() == 4) {
					g_PosEffect[0] = D3DXVECTOR3((GetEnemy() + 2)->Pos.x - 10, (GetEnemy() + 2)->Pos.y - 9.0, (GetEnemy() + 2)->Pos.z);
					g_PosEffect[1] = D3DXVECTOR3((GetEnemy() + 3)->Pos.x - 10, (GetEnemy() + 3)->Pos.y - 9.0, (GetEnemy() + 3)->Pos.z);
					g_RotEffect[1] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
					g_SclEffect[1] = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
				}
				//�e�N�X�`��
				pVtx[0].Effect_tex = D3DXVECTOR2(g_Charge_Animation_3d_X, g_Charge_Animation_3d_Y);
				pVtx[1].Effect_tex = D3DXVECTOR2(g_Charge_Animation_3d_X + g_Charge_Animation_U, g_Charge_Animation_3d_Y);
				pVtx[2].Effect_tex = D3DXVECTOR2(g_Charge_Animation_3d_X, g_Charge_Animation_3d_Y + g_Charge_Animation_V);
				pVtx[3].Effect_tex = D3DXVECTOR2(g_Charge_Animation_3d_X + g_Charge_Animation_U, g_Charge_Animation_3d_Y + g_Charge_Animation_V);

				//��]
				g_RotEffect[0] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

				//������VRAM�֋A��
				g_pVtxBuffEffect[i]->Unlock();
			}

		}
	}

	if (g_AnimationShrink_Flg == true) {

		//�l��ύX
		if (g_NumberResetFlg == true) {
			g_Charge_Animation_3d_X = 0.0f;
			g_Charge_Animation_3d_Y = 0.0f;
			g_NumberResetFlg = false;
		}
		//U�l�𑝂₷
		g_Charge_Animation_3d_X += g_Charge_Animation_U;

		//U�l���E�[�֍s�����Ƃ�
		if (g_Charge_Animation_3d_X >= 1.0f) {
			g_Charge_Animation_3d_Y += g_Charge_Animation_V;	//V�l��1���炷
			g_Charge_Animation_3d_X = 0.0f;	//U�l�����[��
		}
		//V�l�����[�֍s�����Ƃ�
		if (g_Charge_Animation_3d_Y >= 1.0f) {
			g_NumberResetFlg = true;
			g_AnimationShrink_Flg = false;
		}

		//�`��
		for (int i = 0; i < EFFECT_MAX; i++) {
			//���_�o�b�t�@�̔��𖄂߂�
			//VERTEX_3D *pVtx;
			EFFECT_3D *pVtx;

			//VRAM��̏��������Ă��邩��A���_���������悤�ɂȂ�
			g_pVtxBuffEffect[i]->Lock(0, 0, (void**)&pVtx, 0);

			//�ʒu
			pVtx[0].Effect_pos = D3DXVECTOR3(0, HIT_EFFECT_HEIGHT, 0);
			pVtx[1].Effect_pos = D3DXVECTOR3(HIT_EFFECT_WIDTH, HIT_EFFECT_HEIGHT, 0);
			pVtx[2].Effect_pos = D3DXVECTOR3(0, 0, 0);
			pVtx[3].Effect_pos = D3DXVECTOR3(HIT_EFFECT_WIDTH, 0, 0);

			//�ʒu(����)
			if (GetPunchIndex() == 0 || GetPunchIndex() == 2) {
				g_PosEffect[0] = D3DXVECTOR3((GetEnemy() + 3)->Pos.x - 10, (GetEnemy() + 3)->Pos.y - 9.0, (GetEnemy() + 3)->Pos.z);
				g_SclEffect[1] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			}
			//�E��
			else if (GetPunchIndex() == 1 || GetPunchIndex() == 3) {
				g_PosEffect[0] = D3DXVECTOR3((GetEnemy() + 2)->Pos.x - 10, (GetEnemy() + 2)->Pos.y - 9.0, (GetEnemy() + 2)->Pos.z);
				g_SclEffect[1] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			}
			//����
			else if (GetPunchIndex() == 4) {
				g_PosEffect[0] = D3DXVECTOR3((GetEnemy() + 2)->Pos.x - 10, (GetEnemy() + 2)->Pos.y - 9.0, (GetEnemy() + 2)->Pos.z);
				g_PosEffect[1] = D3DXVECTOR3((GetEnemy() + 3)->Pos.x - 10, (GetEnemy() + 3)->Pos.y - 9.0, (GetEnemy() + 3)->Pos.z);
				g_RotEffect[1] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
				g_SclEffect[1] = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
			}

			//�e�N�X�`��
			pVtx[0].Effect_tex = D3DXVECTOR2(g_Charge_Animation_3d_X, g_Charge_Animation_3d_Y);
			pVtx[1].Effect_tex = D3DXVECTOR2(g_Charge_Animation_3d_X + g_Charge_Animation_U, g_Charge_Animation_3d_Y);
			pVtx[2].Effect_tex = D3DXVECTOR2(g_Charge_Animation_3d_X, g_Charge_Animation_3d_Y + g_Charge_Animation_V);
			pVtx[3].Effect_tex = D3DXVECTOR2(g_Charge_Animation_3d_X + g_Charge_Animation_U, g_Charge_Animation_3d_Y + g_Charge_Animation_V);

			//��]
			g_RotEffect[0] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

			//������VRAM�֋A��
			g_pVtxBuffEffect[i]->Unlock();
		}
	}
}

void Punch_Charge_Effect() {
	g_Charge_Animation_Flg = true;
}


//=====================================================================================
//�p���`�̋��؂�Ƃ��̃G�t�F�N�g
//=====================================================================================
HRESULT MakeVertexWindEffect(LPDIRECT3DDEVICE9 pDevice)
{
	for (int i = 0; i < EFFECT_MAX; i++) {
		if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D)*NUM_VERTEX,
			D3DUSAGE_WRITEONLY,
			FVF_VERTEX_3D,
			D3DPOOL_MANAGED,
			&g_pVtxBuffEffect[i],
			NULL)))
		{
			return E_FAIL;
		}
		{
			//���_�o�b�t�@�̔��𖄂߂�
			//VERTEX_3D *pVtx;
			EFFECT_3D *pVtx;
			//VRAM��̏��������Ă��邩��A���_���������悤�ɂȂ�
			g_pVtxBuffEffect[i]->Lock(0, 0, (void**)&pVtx, 0);

			//�ʒu
			pVtx[0].Effect_pos = D3DXVECTOR3(0, WIND_EFFECT_HEIGHT, 0);
			pVtx[1].Effect_pos = D3DXVECTOR3(WIND_EFFECT_WIDTH, WIND_EFFECT_HEIGHT, 0);
			pVtx[2].Effect_pos = D3DXVECTOR3(0, 0, 0);
			pVtx[3].Effect_pos = D3DXVECTOR3(WIND_EFFECT_WIDTH, 0, 0);

			//�@���A�F
			for (int i = 0; i < 4; i++) {
				pVtx[i].Effect_nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
				pVtx[i].Effect_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			}

			//�e�N�X�`��
			pVtx[0].Effect_tex = D3DXVECTOR2(g_Wind_Animation_3d_X, g_Wind_Animation_3d_Y);
			pVtx[1].Effect_tex = D3DXVECTOR2(g_Wind_Animation_3d_X + g_Wind_Animation_U, g_Wind_Animation_3d_Y);
			pVtx[2].Effect_tex = D3DXVECTOR2(g_Wind_Animation_3d_X, g_Wind_Animation_3d_Y + g_Wind_Animation_V);
			pVtx[3].Effect_tex = D3DXVECTOR2(g_Wind_Animation_3d_X + g_Wind_Animation_U, g_Wind_Animation_3d_Y + g_Wind_Animation_V);

			//��]
			g_RotEffect[0] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

			
			//������VRAM�֋A��
			g_pVtxBuffEffect[i]->Unlock();
		}
	}
	return S_OK;
}

void Punch_Wind_Effect_Animation() {
	if (g_Wind_Animation_Flg == true) {
		//U�l�𑝂₷
		g_Wind_Animation_3d_X += g_Wind_Animation_U;

		//U�l���E�[�֍s�����Ƃ�
		if (g_Wind_Animation_3d_X >= 1.0f) {
			g_Wind_Animation_3d_Y += g_Wind_Animation_V;	//V�l��1���炷
			g_Wind_Animation_3d_X = 0.0f;	//U�l�����[��
		}
		//V�l�����[�֍s�����Ƃ�
		if (g_Wind_Animation_3d_Y >= 1.0f) {
			g_Wind_Animation_3d_X = 0.0f;
			g_Wind_Animation_3d_Y = 0.0f;
			g_Wind_Animation_Flg = false;
		}

		//�`��
		for (int i = 0; i < EFFECT_MAX; i++) {
			//���_�o�b�t�@�̔��𖄂߂�
			//VERTEX_3D *pVtx;
			EFFECT_3D *pVtx;

			//VRAM��̏��������Ă��邩��A���_���������悤�ɂȂ�
			g_pVtxBuffEffect[i]->Lock(0, 0, (void**)&pVtx, 0);

			//�ʒu
			pVtx[0].Effect_pos = D3DXVECTOR3(0, WIND_EFFECT_HEIGHT, 0);
			pVtx[1].Effect_pos = D3DXVECTOR3(WIND_EFFECT_WIDTH, WIND_EFFECT_HEIGHT, 0);
			pVtx[2].Effect_pos = D3DXVECTOR3(0, 0, 0);
			pVtx[3].Effect_pos = D3DXVECTOR3(WIND_EFFECT_WIDTH, 0, 0);

			//�ʒu(����)
			if (GetPunchLR() == true) {
				g_PosEffect[0] = D3DXVECTOR3((GetEnemy() + 3)->Pos.x - 34,(GetEnemy() + 3)->Pos.y - 24.0, (GetEnemy() + 3)->Pos.z);
			}
			//�E��
			else {
				g_PosEffect[0] = D3DXVECTOR3((GetEnemy() + 2)->Pos.x - 34, (GetEnemy() + 2)->Pos.y - 24.0, (GetEnemy() + 2)->Pos.z);
			}
			//�e�N�X�`��
			pVtx[0].Effect_tex = D3DXVECTOR2(g_Wind_Animation_3d_X, g_Wind_Animation_3d_Y);
			pVtx[1].Effect_tex = D3DXVECTOR2(g_Wind_Animation_3d_X + g_Wind_Animation_U, g_Wind_Animation_3d_Y);
			pVtx[2].Effect_tex = D3DXVECTOR2(g_Wind_Animation_3d_X, g_Wind_Animation_3d_Y + g_Wind_Animation_V);
			pVtx[3].Effect_tex = D3DXVECTOR2(g_Wind_Animation_3d_X + g_Wind_Animation_U, g_Wind_Animation_3d_Y + g_Wind_Animation_V);

			//��]
			g_RotEffect[0] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			
			//������VRAM�֋A��
			g_pVtxBuffEffect[i]->Unlock();
		}
	}
}


void Punch_Wind_Effect() {
	g_Wind_Animation_Flg = true;
}


//=====================================================================================
//�p���`�̃q�b�g�G�t�F�N�g
//=====================================================================================
HRESULT MakeVertexHitEffect(LPDIRECT3DDEVICE9 pDevice)
{
	for (int i = 0; i < EFFECT_MAX; i++) {
		if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D)*NUM_VERTEX,
			D3DUSAGE_WRITEONLY,
			FVF_VERTEX_3D,
			D3DPOOL_MANAGED,
			&g_pVtxBuffEffect[i],
			NULL)))
		{
			return E_FAIL;
		}
		{
			//���_�o�b�t�@�̔��𖄂߂�
			//VERTEX_3D *pVtx;
			EFFECT_3D *pVtx;
			//VRAM��̏��������Ă��邩��A���_���������悤�ɂȂ�
			g_pVtxBuffEffect[i]->Lock(0, 0, (void**)&pVtx, 0);

			//�ʒu
			pVtx[0].Effect_pos = D3DXVECTOR3(0, WIND_EFFECT_HEIGHT, 0);
			pVtx[1].Effect_pos = D3DXVECTOR3(WIND_EFFECT_WIDTH, WIND_EFFECT_HEIGHT, 0);
			pVtx[2].Effect_pos = D3DXVECTOR3(0, 0, 0);
			pVtx[3].Effect_pos = D3DXVECTOR3(WIND_EFFECT_WIDTH, 0, 0);

			//�@���A�F
			for (int i = 0; i < 4; i++) {
				pVtx[i].Effect_nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
				pVtx[i].Effect_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			}

			//�e�N�X�`��
			pVtx[0].Effect_tex = D3DXVECTOR2(g_Wind_Animation_3d_X, g_Wind_Animation_3d_Y);
			pVtx[1].Effect_tex = D3DXVECTOR2(g_Wind_Animation_3d_X + g_Wind_Animation_U, g_Wind_Animation_3d_Y);
			pVtx[2].Effect_tex = D3DXVECTOR2(g_Wind_Animation_3d_X, g_Wind_Animation_3d_Y + g_Wind_Animation_V);
			pVtx[3].Effect_tex = D3DXVECTOR2(g_Wind_Animation_3d_X + g_Wind_Animation_U, g_Wind_Animation_3d_Y + g_Wind_Animation_V);

			//��]
			g_RotEffect[0] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);


			//������VRAM�֋A��
			g_pVtxBuffEffect[i]->Unlock();
		}
	}
	return S_OK;
}

void Punch_Hit_Effect_Animation() {
	if (g_Hit_Animation_Flg == true) {
		//U�l�𑝂₷
		g_Hit_Animation_3d_X += g_Hit_Animation_U;

		//U�l���E�[�֍s�����Ƃ�
		if (g_Hit_Animation_3d_X >= 1.0f) {
			g_Hit_Animation_3d_Y += g_Hit_Animation_V;	//V�l��1���炷
			g_Hit_Animation_3d_X = 0.0f;	//U�l�����[��
		}
		//V�l�����[�֍s�����Ƃ�
		if (g_Hit_Animation_3d_Y >= 1.0f) {
			g_Hit_Animation_3d_X = 0.0f;
			g_Hit_Animation_3d_Y = 0.0f;
			g_Hit_Animation_Flg = false;
		}

		//�`��
		for (int i = 0; i < EFFECT_MAX; i++) {
			//���_�o�b�t�@�̔��𖄂߂�
			//VERTEX_3D *pVtx;
			EFFECT_3D *pVtx;

			//VRAM��̏��������Ă��邩��A���_���������悤�ɂȂ�
			g_pVtxBuffEffect[i]->Lock(0, 0, (void**)&pVtx, 0);

			//�ʒu
			pVtx[0].Effect_pos = D3DXVECTOR3(0, HIT_EFFECT_HEIGHT, 0);
			pVtx[1].Effect_pos = D3DXVECTOR3(HIT_EFFECT_WIDTH, HIT_EFFECT_HEIGHT, 0);
			pVtx[2].Effect_pos = D3DXVECTOR3(0, 0, 0);
			pVtx[3].Effect_pos = D3DXVECTOR3(HIT_EFFECT_WIDTH, 0, 0);

			//�ʒu
			g_PosEffect[0] = D3DXVECTOR3(GetEnemy()->Pos.x, GetEnemy()->Pos.y, GetEnemy()->Pos.z -4);
			g_PosEffect[1] = D3DXVECTOR3(GetEnemy()->Pos.x + 10, GetEnemy()->Pos.y + 10, GetEnemy()->Pos.z);
			g_PosEffect[2] = D3DXVECTOR3(GetEnemy()->Pos.x - 10, GetEnemy()->Pos.y - 10, GetEnemy()->Pos.z);


			//�傫��
			g_SclEffect[0] = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
			g_SclEffect[1] = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
			g_SclEffect[2] = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
			//�e�N�X�`��
			pVtx[0].Effect_tex = D3DXVECTOR2(g_Hit_Animation_3d_X, g_Hit_Animation_3d_Y);
			pVtx[1].Effect_tex = D3DXVECTOR2(g_Hit_Animation_3d_X + g_Hit_Animation_U, g_Hit_Animation_3d_Y);
			pVtx[2].Effect_tex = D3DXVECTOR2(g_Hit_Animation_3d_X, g_Hit_Animation_3d_Y + g_Hit_Animation_V);
			pVtx[3].Effect_tex = D3DXVECTOR2(g_Hit_Animation_3d_X + g_Hit_Animation_U, g_Hit_Animation_3d_Y + g_Hit_Animation_V);

			//��]
			g_RotEffect[0] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

			//������VRAM�֋A��
			g_pVtxBuffEffect[i]->Unlock();
		}
	}
}


void Punch_Hit_Effect() {
	g_Hit_Animation_Flg = true;
}

