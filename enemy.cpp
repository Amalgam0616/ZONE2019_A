
#include "myDirect3D.h"
#include "input.h"
#include "Xfile.h"
#include "enemy.h"
#include "debugproc.h"
#include "Effect.h"
#include <time.h>	//�����V�[�h���悤�Ȃ̂ŁA�����Ȃ炱���
#include "texture.h"
#include "player.h"

#include "sky.h"
#include "game.h"
#include "Roof.h"
#include "result.h"
#include "particle.h"
#include "score.h"

//�萔��` ==========================================================================

//�v���C���[�Ɏg�����f����
#define		ENEMY_MODEL_NUM	(7)

//�G�̏������WZ
#define ENEMY_POS_Z (26.0f)

//�A�b�p�[�Ő�����ԃX�s�[�h
#define UPPERED_SPEED (15.0f)

//�X���[�G���A���W
#define		SLOW_IN		(23.0f)
#define		SLOW_OUT	(15.0f)

//1�Q�[��������̃p���`��
#define		NUM_PUNCH		(10)

//�p���`�̎�ނ��Ƃ�START���珀�������܂ł̃t���[������ݒ�
#define		PPI_R_P_START_FRAME		(74)	//�E�ʏ�p���`
#define		PPI_L_P_START_FRAME		(74)	//���ʏ�p���`
#define		FLASH_P_START_FRAME		(74)	//�u�Ԉړ��p���`
#define		PPI_D_P_START_FRAME		(74)	//���ʏ�p���`
#define		PPI_JP_START_FRAME		(120)	//�W�����v�p���`(���E���)


//�u�Ԉړ��p���`�̃X���[�܂ł̃t���[��
#define		FLASH_P_SLOW_FRAME		(74)

//�O���[�o���ϐ���` =================================================================

bool g_Huttobi;

//�v���C���[��XMODEL�錾������
//0:���@1:���@2:����@3:�E��@4:�����@5:�E��
XMODEL g_Enemy[ENEMY_MODEL_NUM] = {};

//���[���h���W
static D3DXMATRIXA16 g_mtxWorld;

//�ҋ@���[�V������点��̂ł���p�̃��c(�قڃv���C���[�Ɠ�����)
//�A�j���[�V�����̃��X�g
static PL_ANIME g_EnmAnime =
{
	//ANIME_INDEX_MORMAL�@�ҋ@���[�V����
	15, 15, D3DXVECTOR3(0.0f, D3DXToRadian(0.0f), 0.0f),D3DXVECTOR3(0.0f, 1.5f, 0.0f)
};

//�A�j���[�V�����֘A
//�t���[���J�E���^
int g_EnmAnimFrameCnt;
//�ҋ@��Ԃ��ۂ�
bool isTaiki;
//1�t���[��������̊p�x�ω���
D3DXVECTOR3 g_EnmAnglePerFrame;
//1�t���[��������̍��W�ω���
D3DXVECTOR3 g_EnmMovePerFrame;
//���𓮂����n�̃��c(���Ǝv��)
bool EnmLeg_Flg;


//�p���`�p�^�[��
//�O���E�X�g���[�g�@�P�����p���`�@�Q���u�Ԉړ��p���`
int g_PunchPattern[NUM_PUNCH];

//�p���`�����ۂ�
bool Punch_Flg;
//�p���`�̃t�F�[�Y�Ǘ�
PUNCH_PHASE g_PunchPhase;

//�ǉ�����1===============================
LAST_PUNCH_PHASE g_Last_Punch_Phase;
//�ǉ�����1===============================

//�����Ă���p���`�̎�ނ̊Ǘ�
PUNCH_PATTERN_INDEX g_Punch_Pattern_Index;
//�p���`�̃t���[���J�E���^
int g_PunchFrameCnt;
//�p���`�̃X�s�[�h
float Punch_Speed;
//�p���`�I��Z���W
float g_PunchEndLine;

//�ǉ�����2===============================
//������΂����Ƃ��Ɏg���J�E���g
int g_FLYAWAY_Cnt;
//�ǉ�����2===============================

//�u�Ԉړ����̑��x�{��
static float g_Slow_Multi;

//�p���`�J�nZ���W
//0= �E�@1= ��
float g_PunchStartLine[2];
//�������Ă���肪�ǂ����Ȃ̂�
//L=true R=false
bool PunchLR;
//���̃p���`�̉�
static int g_PunchCnt;
//�ҋ@����
int g_WaitTime;
//���f���̏����ʒu
static D3DXVECTOR3 g_ModelStaPos[ENEMY_MODEL_NUM - 1] = {};
static D3DXVECTOR3 g_ModelStaRot[ENEMY_MODEL_NUM - 1] = {};
//�p���`�A��x�N�g��
static D3DXVECTOR3 g_PunchEndVec[ENEMY_MODEL_NUM - 1] = {};
//�p���`�A��p�x
static D3DXVECTOR3 g_PunchEndRot[ENEMY_MODEL_NUM - 1] = {};

//�p���`�I�����猳�̈ʒu�ɖ߂�܂ł̃t���[�����̃��X�g
static const int g_EndFrameList[PPI_MAX] =
{
	15,	//�E�ʏ�p���`
	15,	//���ʏ�p���`
	0,	//�E�u�Ԉړ��p���`
	0,	//���u�Ԉړ��p���`
	10,	//����o�[���Ă���p���`
	15,	//�W�����v�p���`��
	15,	//�W�����v�p���`�E
};



//�X���[�t���O(����)
bool SlowFlg;

//��{�֐���` ======================================================================
void ENEMY_OUTFLYAWAY();

//������
HRESULT InitEnemy()
{
	//�����V�[�h����(���̂Ƃ��ł��Ȃ炱���̂͏����Ă�������)
	srand((unsigned int)time(NULL));

	//�ǂݍ��݁E�����l�ݒ�
	{
		//XFile����3D���f�������[�h
		if (FAILED(LoadXFile(&g_Enemy[0], XFILE_INDEX_E_BODY_001)) ||		//����
			FAILED(LoadXFile(&g_Enemy[1], XFILE_INDEX_E_HEAD_001)) ||		//��
			FAILED(LoadXFile(&g_Enemy[2], XFILE_INDEX_E_GROBE_L_001)) ||	//����(�O����݂ĉE)
			FAILED(LoadXFile(&g_Enemy[3], XFILE_INDEX_E_GROBE_R_001)) ||	//�E��(�O����݂č�)
			
			FAILED(LoadXFile(&g_Enemy[4], XFILE_INDEX_E_LEG_L_001)) ||		//����(�O����݂ĉE)
			FAILED(LoadXFile(&g_Enemy[5], XFILE_INDEX_E_LEG_R_001)) ||		//�E��(�O����݂č�)
			FAILED(LoadXFile(&g_Enemy[6], XFILE_INDEX_E_DMGBODY_001)))		//���̎��̓���
		{
			//���s�����炱��
			return E_FAIL;
		}

		//���W�E��]�E�g�k�@�����l�ݒ�
		EnemyPosReset();

		for (int i = 0; i < ENEMY_MODEL_NUM; i++)
		{
			g_Enemy[i].Scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		}
	}

	//�X���[�t���O
	SlowFlg = false;

	//�ǉ�����3===============================
	//�p���`�p�^�[���쐬
	for (int i = 0; i < NUM_PUNCH; i++)
	{
		g_PunchPattern[i] = rand() % (PPI_MAX - 2);
	}
	//�Ō�̃p���`�͕K��
	g_PunchPattern[NUM_PUNCH - 1] = LAST_PUNCH;

	//�p���`�t���[���J�E���g�̏�����
	g_PunchFrameCnt = 0;

	//�ǉ�����3===============================

	//�p���`�����ۂ�
	Punch_Flg = false;
	//�p���`�X�s�[�h
	Punch_Speed = 3.0f;

	//�ǉ�����4===============================
	//������΂����Ƃ��Ɏg���J�E���g
	g_FLYAWAY_Cnt = 0;
	//�ǉ�����4===============================

	//�u�Ԉړ��̑��x�{��
	g_Slow_Multi = 1.0f;

	//�p���`�̃t�F�[�Y�Ǘ�
	g_PunchPhase = PUNCH_PHASE_CHARGE;
	//�����Ă���p���`�̎�ނ̊Ǘ�
	g_Punch_Pattern_Index = PUNCH_NULL;

	//�ǉ�����5===============================
	//�Ō�̃p���`�̃t�F�[�Y�̏�����
	g_Last_Punch_Phase = PUNCH_PHASE_LCHARGE;
	//�ǉ�����5===============================

	//�p���`�I��Z���W(���ꂪ����̂ŁA�G��Init�̓v���C���[����ɍs���Ă�������)
	g_PunchEndLine = (GetPlayer() + 1)->Pos.z;

	//�p���`�X�^�[�g�ʒu�L�^
	g_PunchStartLine[0] = g_Enemy[3].Pos.z;
	g_PunchStartLine[1] = g_Enemy[2].Pos.z;

	//�p���`�񐔏�����
	g_PunchCnt = 0;

	//���f�������ʒu�ۑ�
	for (int i = 0; i < ENEMY_MODEL_NUM - 1; i++)
	{
		g_ModelStaPos[i] = g_Enemy[i].Pos;
		g_ModelStaRot[i] = g_Enemy[i].Rot;
	}

	//�ҋ@���ԏ�����
	g_WaitTime = rand() % 60 + 120;

	//�A�b�p�[�ҋ@��Ԃ��ۂ�
	g_Huttobi = false;

	//�A�j���[�V�����֘A
	g_EnmAnimFrameCnt = 0;
	isTaiki = true;
	g_EnmAnglePerFrame = {};
	g_EnmMovePerFrame = {};
	EnmLeg_Flg = false;

	return S_OK;
}

//�I��
void UninitEnemy()
{
	//���b�V���̉��
	for (int i = 0; i < ENEMY_MODEL_NUM; i++)
	{
		SAFE_RELEASE(g_Enemy[i].pMesh);
	}
}

//�X�V
void UpdateEnemy()
{
	static int i = 0;

	//����F�X���[�֘A
	{
		//��������͂���Ă��Ȃ����A�p���`����������p���`�p�^�[�����Ƃ�Z���W�����āA�X���[�G���A����������X���[
		if (!GetDodgeFlg())
		{
			//�p���`�{�̒��̂�
			if (g_PunchPhase == PUNCH_PHASE_SWING)
			{
				//�p���`�񐔖ڂ̃p���`�p�^�[�������āAif������
				if (g_PunchPattern[g_PunchCnt] == PPI_RIGHT_PUNCH)
				{//�E�ʏ�p���`
					if (g_Enemy[3].Pos.z < SLOW_IN && g_Enemy[3].Pos.z > SLOW_OUT && !SlowFlg)
					{
						SlowFlg = true;
					}
					else if (g_Enemy[3].Pos.z < SLOW_OUT && SlowFlg)
					{
						SlowFlg = false;
					}
				}
				else if (g_PunchPattern[g_PunchCnt] == PPI_LEFT_PUNCH)
				{//���ʏ�p���`
					if (g_Enemy[2].Pos.z < SLOW_IN && g_Enemy[2].Pos.z > SLOW_OUT && !SlowFlg)
					{
						SlowFlg = true;
					}
					else if (g_Enemy[2].Pos.z < SLOW_OUT&& SlowFlg)
					{
						SlowFlg = false;
					}
				}
				else if (g_PunchPattern[g_PunchCnt] == R_FLASH_PUNCH)
				{//�u�Ԉړ��p���`
				}
				else if (g_PunchPattern[g_PunchCnt] == PPI_DUNK_PUNCH)
				{//����o�[���Ă���p���`
					if (g_Enemy[2].Pos.z < (SLOW_IN - 3.0f) && g_Enemy[2].Pos.z >(SLOW_OUT + 2.0f) && !SlowFlg)
					{
						SlowFlg = true;
					}
					else if (g_Enemy[2].Pos.z < (SLOW_OUT + 2.0f) && SlowFlg)
					{
						SlowFlg = false;
					}
				}
				else if (g_PunchPattern[g_PunchCnt] == PPI_L_JUMP_PUNCH)
				{//�W�����v�p���`��
					if (g_Enemy[2].Pos.z < SLOW_IN && g_Enemy[2].Pos.z >(SLOW_OUT - 2.0f) && !SlowFlg)
					{
						SlowFlg = true;
					}
					else if (g_Enemy[2].Pos.z < (SLOW_OUT - 2.0f) && SlowFlg)
					{
						SlowFlg = false;
					}
				}
				else if (g_PunchPattern[g_PunchCnt] == PPI_R_JUMP_PUNCH)
				{//�W�����v�p���`�E
					if (g_Enemy[3].Pos.z < SLOW_IN && g_Enemy[3].Pos.z >(SLOW_OUT - 2.0f) && !SlowFlg)
					{
						SlowFlg = true;
					}
					else if (g_Enemy[3].Pos.z < (SLOW_OUT - 2.0f) && SlowFlg)
					{
						SlowFlg = false;
					}
				}
			}
		}

		//�����������̉�������͂��ꂽ��X���[����
		if (GetDodgeFlg())
		{
			SlowFlg = false;
		}
	}

	//�ҋ@����������ҋ@���[�V���������
	if (isTaiki)
	{
		//�A�j���\�V�����i�s
		EnmAnimation();
		//�t���[���i�s
		g_EnmAnimFrameCnt++;	//�����Ł@1�@�ȏ�ɂȂ�
	}

	//�p���`�֘A
	{
		//�A�b�p�[�ҋ@���͓����Ȃ�
		if (!g_Huttobi)
		{
			//������������A�j���[�V�������I������PosReset�A����̃p���`
			if (!Punch_Flg && g_WaitTime <= 0)
			{
				//�A�j���I���F�����l�ɃZ�b�g
				isTaiki = false;
				EnemyPosReset();
				//�p���`���ɂ���
				Punch_Flg = true;
			}
			else
			{//�ҋ@���Ԑi�s
				g_WaitTime--;
			}

			if (Punch_Flg)
			{
				//���X�g�̃p���`�����ڂŌ������ꍇ�͉��̃R�����g���������Ă�������
				//g_PunchPattern[g_PunchCnt] = LAST_PUNCH;

				//�p���`�񐔖ڂ̃p���`�p�^�[�������āAif������
				if (g_PunchPattern[g_PunchCnt] == PPI_RIGHT_PUNCH)
				{//�E�ʏ�p���`
					g_Punch_Pattern_Index = PPI_RIGHT_PUNCH;
					RightPunch();
				}
				else if (g_PunchPattern[g_PunchCnt] == PPI_LEFT_PUNCH)
				{//���ʏ�p���`
					g_Punch_Pattern_Index = PPI_LEFT_PUNCH;
					LeftPunch();
				}
				else if (g_PunchPattern[g_PunchCnt] == R_FLASH_PUNCH)
				{//�u�Ԉړ��p���`

				//��������S�̓I�ɉ���
					if (GetDodgeFlg())
					{
						g_Slow_Multi = 3.0f;
						g_PunchFrameCnt++;
					}

					g_Punch_Pattern_Index = R_FLASH_PUNCH;
					Right_R_FLASH_PUNCH();
				}
				else if (g_PunchPattern[g_PunchCnt] == L_FLASH_PUNCH)
				{//�u�Ԉړ��p���`

				//��������S�̓I�ɉ���
					if (GetDodgeFlg())
					{
						g_Slow_Multi = 3.0f;
						g_PunchFrameCnt++;
					}

					g_Punch_Pattern_Index = L_FLASH_PUNCH;
					Left_R_FLASH_PUNCH();
				}
				else if (g_PunchPattern[g_PunchCnt] == PPI_DUNK_PUNCH)
				{//����o�[���Ă���p���`
					g_Punch_Pattern_Index = PPI_DUNK_PUNCH;
					DunkPunch();
				}
				else if (g_PunchPattern[g_PunchCnt] == PPI_L_JUMP_PUNCH)
				{//�W�����v�p���`��
					g_Punch_Pattern_Index = PPI_L_JUMP_PUNCH;
					JumpPunch_L();
				}
				else if (g_PunchPattern[g_PunchCnt] == PPI_R_JUMP_PUNCH)
				{//�W�����v�p���`�E
					g_Punch_Pattern_Index = PPI_R_JUMP_PUNCH;
					JumpPunch_R();
				}
				//�ǉ�����6==========================================
				else if (g_PunchPattern[g_PunchCnt] == LAST_PUNCH)
				{
					g_Punch_Pattern_Index = LAST_PUNCH;
					Last_Punch();
				}
				//�ǉ�����6==========================================
			}
		}
	}




	//�f�o�b�O�p
	DebugProc_Print((char *)"�Ō�̃p���`�̃t�F�[�Y�Ǘ��F[%d]\n", g_Last_Punch_Phase);
	DebugProc_Print((char *)"i�F[%d]\n", i);
	DebugProc_Print((char *)"���W�F[X:%f Y:%f Z:%f]\n", g_Enemy[i].Pos.x, g_Enemy[i].Pos.y, g_Enemy[i].Pos.z);
	DebugProc_Print((char *)"�p�x�F[X:%f Y:%f Z:%f]\n", D3DXToDegree(g_Enemy[i].Rot.x), D3DXToDegree(g_Enemy[i].Rot.y), D3DXToDegree(g_Enemy[i].Rot.z));
	DebugProc_Print((char *)"SlowFlg�F[%d]\n", SlowFlg);
	DebugProc_Print((char *)"�p���`��ށF[%d]\n", g_PunchPattern[g_PunchCnt]);
	DebugProc_Print((char *)"�p���`��(�f�o�b�O���̓��[�v)�F[%d]\n", g_PunchCnt);
	DebugProc_Print((char *)"����̃p���`��ވꗗ�F[%d,%d,%d,%d,%d,%d,%d,%d,%d,%d]\n", g_PunchPattern[0], g_PunchPattern[1], g_PunchPattern[2], g_PunchPattern[3], g_PunchPattern[4], g_PunchPattern[5], g_PunchPattern[6], g_PunchPattern[7], g_PunchPattern[8], g_PunchPattern[9]);

	//�ȉ��f�o�b�N�p
	static int a = 0;
	if (Keyboard_IsTrigger(DIK_O))
	{

	}

	if (Keyboard_IsPress(DIK_I))
	{

	}

	//�f�o�b�N�p�i��]/�V�t�g�����Ȃ��炾�ƈړ��j
	if (Keyboard_IsPress(DIK_LEFT))
	{
		if (Keyboard_IsPress(DIK_LSHIFT))
		{
			g_Enemy[i].Pos.x -= 0.3f;
		}
		else
		{
			g_Enemy[i].Rot.y += D3DXToRadian(0.3f);
		}
	}

	if (Keyboard_IsPress(DIK_RIGHT))
	{
		if (Keyboard_IsPress(DIK_LSHIFT))
		{
			g_Enemy[i].Pos.x += 0.3f;
		}
		else
		{
			g_Enemy[i].Rot.y -= D3DXToRadian(0.3f);
		}
	}

	if (Keyboard_IsPress(DIK_UP))
	{
		if (Keyboard_IsPress(DIK_LSHIFT))
		{
			g_Enemy[i].Pos.y += 0.3f;
		}
		else if (Keyboard_IsPress(DIK_Z))
		{
			g_Enemy[i].Pos.z += 0.3f;
		}
		else
		{
			g_Enemy[i].Rot.x += D3DXToRadian(0.3f);
		}
	}

	if (Keyboard_IsPress(DIK_DOWN))
	{
		if (Keyboard_IsPress(DIK_LSHIFT))
		{
			g_Enemy[i].Pos.y -= 0.3f;
		}
		else if (Keyboard_IsPress(DIK_Z))
		{
			g_Enemy[i].Pos.z -= 0.3f;
		}
		else
		{
			g_Enemy[i].Rot.x -= D3DXToRadian(0.3f);
		}
	}

	if (Keyboard_IsTrigger(DIK_N))
	{
		i++;
	}

	if (i > 6)
	{
		i = 0;
	}
}

//�`��
void DrawEnemy()
{
	//Direct3DDevice�̎擾
	LPDIRECT3DDEVICE9 p_D3DDevice = GetD3DDevice();

	//���[���h�g�����X�t�H�[��(��΍��W�ϊ�)
	D3DXMatrixIdentity(&g_mtxWorld);

	//�ǉ�����7==========================================
	if (g_Last_Punch_Phase < PUNCH_PHASE_FLYAWAY)
		//�ǉ�����7==========================================
	{
		//���f�����J��Ԃ�
		for (int j = 0; j < ENEMY_MODEL_NUM - 1; j++)
		{
			//���[���h�}�g���N�X����ēK�p
			p_D3DDevice->SetTransform(D3DTS_WORLD, MakeWorldMatrix(&g_Enemy[j], &g_mtxWorld));

			//�}�e���A���̐������J��Ԃ�
			for (DWORD i = 0; i < g_Enemy[j].dwNumMaterials; i++)
			{
				//�}�e���A���Z�b�g
				p_D3DDevice->SetMaterial(&g_Enemy[j].pMaterials[i]);
				//�e�N�X�`���Z�b�g
				p_D3DDevice->SetTexture(0, g_Enemy[j].pTextures[i]);
				//���ߓx�ݒ�
				g_Enemy[j].pMaterials[i].Diffuse.a = 1.0f;
				//������`��
				g_Enemy[j].pMesh->DrawSubset(i);
			}
		}
	}
	else
	{
		//���f�����J��Ԃ�
		for (int j = 1; j < ENEMY_MODEL_NUM; j++)
		{
			//���[���h�}�g���N�X����ēK�p
			p_D3DDevice->SetTransform(D3DTS_WORLD, MakeWorldMatrix(&g_Enemy[j], &g_mtxWorld));

			//�}�e���A���̐������J��Ԃ�
			for (DWORD i = 0; i < g_Enemy[j].dwNumMaterials; i++)
			{
				//�}�e���A���Z�b�g
				p_D3DDevice->SetMaterial(&g_Enemy[j].pMaterials[i]);
				//�e�N�X�`���Z�b�g
				p_D3DDevice->SetTexture(0, g_Enemy[j].pTextures[i]);
				//���ߓx�ݒ�
				g_Enemy[j].pMaterials[i].Diffuse.a = 1.0f;
				//������`��
				g_Enemy[j].pMesh->DrawSubset(i);
			}
		}
	}
}

//���̑������Ȋ֐���` =============================================================

//������A�j���[�V����������֐�
void EnmAnimation()
{
	//g_AnimFrame�@���@0�@�ȏ㊎�@StrFrame�@�ȉ��������ꍇ�́A�A�j���X�e�[�g���ɓ��������ʂ����߂ĂP�t���[����������
	if ((g_EnmAnimFrameCnt >= 0 + 1) && (g_EnmAnimFrameCnt <= g_EnmAnime.StrFrame + 1))
	{
		//g_AnimFrameCnt�@���@0�@�������ꍇ�́A���f���̍��W�E��]�������ʒu�ɖ߂��āA�A�j���X�e�[�g���ɂP�t���[��������̓������v�Z
		if (g_EnmAnimFrameCnt == 0 + 1)
		{
			EnemyPosReset();
			EnmLeg_Flg = true;

			//1�t���[��������̊p�x
			g_EnmAnglePerFrame = D3DXVECTOR3(g_EnmAnime.Angle.x / g_EnmAnime.StrFrame,
				g_EnmAnime.Angle.y / g_EnmAnime.StrFrame,
				g_EnmAnime.Angle.z / g_EnmAnime.StrFrame);

			//1�t���[��������̈ړ�
			g_EnmMovePerFrame = D3DXVECTOR3(g_EnmAnime.Move.x / g_EnmAnime.StrFrame,
				g_EnmAnime.Move.y / g_EnmAnime.StrFrame,
				g_EnmAnime.Move.z / g_EnmAnime.StrFrame);
		}

		//�ҋ@���[�V�����@���A���A���r�@�𓮂���
		for (int i = 0; i < ENEMY_MODEL_NUM - 3; i++)
		{
			//���W�ړ�
			g_Enemy[i].Pos += g_EnmMovePerFrame;
			//�p�x�ω�
			g_Enemy[i].Rot += g_EnmAnglePerFrame;
		}
	}
	//g_AnimFrameCnt�@���@StrFrame + 1�@�ȏ㊎�@StrFrame + EndFrame�@�ȉ��������ꍇ�́A�A�j���X�e�[�g���ɓ��������ʂ����߂ĂP�t���[����������
	else if ((g_EnmAnimFrameCnt >= g_EnmAnime.StrFrame + 2) && (g_EnmAnimFrameCnt <= (g_EnmAnime.StrFrame + g_EnmAnime.EndFrame + 1)))
	{
		//g_AnimFrameCnt�@���@StrFrame�@���@1�@�傫���ꍇ�́A1�t���[��������̈ړ�������߂�p�ɍČv�Z����
		if (g_EnmAnimFrameCnt == (g_EnmAnime.StrFrame + 2))
		{
			EnmLeg_Flg = false;
			//�p�x�@�S�̂̊p�x�ɖ߂��āA�A��̃t���[�����Ŋ����āA���]
			g_EnmAnglePerFrame = D3DXVECTOR3((g_EnmAnime.Angle.x / g_EnmAnime.EndFrame) * -1.0f,
				(g_EnmAnime.Angle.y / g_EnmAnime.EndFrame) * -1.0f,
				(g_EnmAnime.Angle.z / g_EnmAnime.EndFrame) * -1.0f);

			//�ړ��ʁ@�S�̂̈ړ��ʂɖ߂��āA�A��̃t���[�����Ŋ����āA���]
			g_EnmMovePerFrame = D3DXVECTOR3((g_EnmAnime.Move.x / g_EnmAnime.EndFrame) * -1.0f,
				(g_EnmAnime.Move.y / g_EnmAnime.EndFrame) * -1.0f,
				(g_EnmAnime.Move.z / g_EnmAnime.EndFrame) * -1.0f);
		}

		//�ҋ@���[�V�����@���A���A���r�@�𓮂���
		for (int i = 0; i < ENEMY_MODEL_NUM - 3; i++)
		{
			//���W�ړ�
			g_Enemy[i].Pos += g_EnmMovePerFrame;
			//�p�x�ω�
			g_Enemy[i].Rot += g_EnmAnglePerFrame;
		}
	}
	//g_AnimFrameCnt�@���@StrFrame + EndFrame + 1�@�ȏゾ�����ꍇ�́A�A�j���[�V�������I�����đҋ@���[�V�����ɖ߂�
	else if (g_EnmAnimFrameCnt >= (g_EnmAnime.StrFrame + g_EnmAnime.EndFrame + 2))
	{
		//g_AnimFrameCnt��0�ɖ߂�
		g_EnmAnimFrameCnt = 0;
		//�t���[��������̈ړ���]��0�ɖ߂�
		g_EnmAnglePerFrame = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_EnmMovePerFrame = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}
}

//�G�l�~�[�̏����ʒu�ɖ߂��֐�
void EnemyPosReset()
{
	//����
	g_Enemy[0].Pos = D3DXVECTOR3(0.0f, 8.0f, ENEMY_POS_Z);
	g_Enemy[0].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));
	//��
	g_Enemy[1].Pos = D3DXVECTOR3(0.0f, 23.0f, ENEMY_POS_Z);
	g_Enemy[1].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));
	//���r
	g_Enemy[2].Pos = D3DXVECTOR3(7.0f, 12.0f, ENEMY_POS_Z - 4.0f);
	g_Enemy[2].Rot = D3DXVECTOR3(D3DXToRadian(15.0f), D3DXToRadian(180.0f), D3DXToRadian(90.0f));
	//�E�r
	g_Enemy[3].Pos = D3DXVECTOR3(-7.0f, 15.0f, 26.0f);
	g_Enemy[3].Rot = D3DXVECTOR3(D3DXToRadian(15.0f), D3DXToRadian(180.0f), D3DXToRadian(-90.0f));
	//����
	g_Enemy[4].Pos = D3DXVECTOR3(8.0f, -6.0f, ENEMY_POS_Z);
	g_Enemy[4].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));
	//�E��
	g_Enemy[5].Pos = D3DXVECTOR3(-6.0f, -5.85f, ENEMY_POS_Z);
	g_Enemy[5].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));
}

//�G�l�~�[���A�b�p�[�󂯂�ʒu�ɕς���֐�
void EnemyUpperPos()
{
	//����
	g_Enemy[0].Pos = D3DXVECTOR3(0.0f, 5.0f, ENEMY_POS_Z);
	g_Enemy[0].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));
	//��
	g_Enemy[1].Pos = D3DXVECTOR3(g_Enemy[1].Pos.x, g_Enemy[1].Pos.y - 7, g_Enemy[1].Pos.z - 5.0f);
	g_Enemy[1].Rot = D3DXVECTOR3(D3DXToRadian(340.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));
	//���r
	g_Enemy[2].Pos = D3DXVECTOR3(g_Enemy[2].Pos.x, g_Enemy[2].Pos.y, g_Enemy[2].Pos.z - 8.0f);
	g_Enemy[2].Rot = D3DXVECTOR3(D3DXToRadian(90.0f), D3DXToRadian(90.0f), D3DXToRadian(0.0f));
	//�E�r
	g_Enemy[3].Pos = D3DXVECTOR3(g_Enemy[3].Pos.x, g_Enemy[3].Pos.y, g_Enemy[3].Pos.z - 8.0f);
	g_Enemy[3].Rot = D3DXVECTOR3(D3DXToRadian(90.0f), D3DXToRadian(270.0f), D3DXToRadian(0.0f));
	//����
	g_Enemy[4].Pos = D3DXVECTOR3(g_Enemy[4].Pos.x, g_Enemy[4].Pos.y, g_Enemy[4].Pos.z - 6.0f);
	g_Enemy[4].Rot = D3DXVECTOR3(D3DXToRadian(20.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));
	//�E��
	g_Enemy[5].Pos = D3DXVECTOR3(g_Enemy[5].Pos.x, g_Enemy[5].Pos.y, g_Enemy[5].Pos.z - 6.0f);
	g_Enemy[5].Rot = D3DXVECTOR3(D3DXToRadian(20.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));

	//�A�b�p�[���ꂽ���̓���
	g_Enemy[6].Pos = D3DXVECTOR3(0.0f, 5.0f, ENEMY_POS_Z);
	g_Enemy[6].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));
}

//�u�Ԉړ��p���`(�E)�̃|�W�V����
void R_FLASH_PUNCH_R()
{
	//����
	g_Enemy[0].Pos = R_FLASH_PUNCH_R_POS0;
	g_Enemy[0].Rot = R_FLASH_PUNCH_R_ROT0;
	//��
	g_Enemy[1].Pos = R_FLASH_PUNCH_R_POS1;
	g_Enemy[1].Rot = R_FLASH_PUNCH_R_ROT1;
	//���r
	g_Enemy[2].Pos = R_FLASH_PUNCH_R_POS2;
	g_Enemy[2].Rot = R_FLASH_PUNCH_R_ROT2;
	//�E�r
	g_Enemy[3].Pos = R_FLASH_PUNCH_R_POS3;
	g_Enemy[3].Rot = R_FLASH_PUNCH_R_ROT3;
	//����
	g_Enemy[4].Pos = R_FLASH_PUNCH_R_POS4;
	g_Enemy[4].Rot = R_FLASH_PUNCH_R_ROT4;
	//�E��
	g_Enemy[5].Pos = R_FLASH_PUNCH_R_POS5;
	g_Enemy[5].Rot = R_FLASH_PUNCH_R_ROT5;
}

//�u�Ԉړ��p���`(��)�̃|�W�V����
void L_FLASH_PUNCH_R()
{
	//����
	g_Enemy[0].Pos = L_FLASH_PUNCH_POS0;
	g_Enemy[0].Rot = L_FLASH_PUNCH_ROT0;
	//��
	g_Enemy[1].Pos = L_FLASH_PUNCH_POS1;
	g_Enemy[1].Rot = L_FLASH_PUNCH_ROT1;
	//���r
	g_Enemy[2].Pos = L_FLASH_PUNCH_POS2;
	g_Enemy[2].Rot = L_FLASH_PUNCH_ROT2;
	//�E�r
	g_Enemy[3].Pos = L_FLASH_PUNCH_POS3;
	g_Enemy[3].Rot = L_FLASH_PUNCH_ROT3;
	//����
	g_Enemy[4].Pos = L_FLASH_PUNCH_POS4;
	g_Enemy[4].Rot = L_FLASH_PUNCH_ROT4;
	//�E��
	g_Enemy[5].Pos = L_FLASH_PUNCH_POS5;
	g_Enemy[5].Rot = L_FLASH_PUNCH_ROT5;
}

//�G�̃p���`��ݒ肷��֐��Q
//No.01�̂����{�ł��B��{�͂���ɏ]���΍��܂�(����)

//�E�ʏ�p���`(PPI_RIGHT_PUNCH)(�쐬�ҁFAmalgam�ANo.01�@���̃R�����g�̓p���`�֐��Q�����ꍇ�ɓ��P���Ă�������)
void RightPunch()
{
	//�ڕW�n�_�ւ̃x�N�g��
	static D3DXVECTOR2 dir;

	//�t���[���i�s
	g_PunchFrameCnt++;

	if (g_PunchPhase == PUNCH_PHASE_CHARGE)
	{//�\������
		if (g_PunchFrameCnt < PPI_R_P_START_FRAME)
		{
			if (g_PunchFrameCnt < 30)
			{
				//�E��������\������
				g_Enemy[3].Pos.z += 8.0f / PPI_R_P_START_FRAME;
			}

			if (g_PunchFrameCnt == 1)
			{
				//Punch_Charge_Effect();
				Charge_Start();
				g_Enemy[3].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(285.0f), D3DXToRadian(10.0f));
				PunchLR = true;
			}
		}
		else
		{
			//�v���C���[�̓��ւ̃x�N�g�����o��
			dir.x = 0.0f - g_Enemy[3].Pos.x;
			dir.y = 8.0f - g_Enemy[3].Pos.y;
			D3DXVec2Normalize(&dir, &dir);

			//�t�F�[�Y�i�s
			g_PunchPhase = PUNCH_PHASE_SWING;
		}
	}
	else if (g_PunchPhase == PUNCH_PHASE_SWING)
	{//�p���`�{��
		if (g_Enemy[3].Pos.z >= g_PunchEndLine)
		{//�G���h���C���ɓ��B���ĂȂ���Γ�����(�v���C���[�͓G����݂�Z�}�C�i�X�����ɂ���)
			if (SlowFlg)
			{//�X���[�G���A���������瑬�x�ɉ����␳
				g_Enemy[3].Pos += D3DXVECTOR3((dir.x / 2.0f) * 0.1f, (dir.y / 2.0f) * 0.1f, -(Punch_Speed / 10.0f));
			}
			else
			{
				g_Enemy[3].Pos += D3DXVECTOR3(dir.x / 2.0f, dir.y / 1.5f, -(Punch_Speed));
			}
		}
		else if (g_Enemy[3].Pos.z < g_PunchEndLine)
		{//���B������I���
			//��X�����̂łO�ɂ���
			g_PunchFrameCnt = 0;
			//�t�F�[�Y�i�s
			g_PunchPhase = PUNCH_PHASE_RETURN;
		}
	}
	else if (g_PunchPhase == PUNCH_PHASE_RETURN)
	{
		if (g_PunchFrameCnt == 1)
		{//�ŏ��̃t���[���ɖ߂�p�̃x�N�g�������
			CreatePunchEndVec();
		}
		else if (g_PunchFrameCnt >= 3)
		{//���t���[���܂��Ă��猳�̈ʒu�ɖ߂�
			if (g_PunchFrameCnt < g_EndFrameList[g_PunchPattern[g_PunchCnt]])
			{
				for (int i = 0; i < ENEMY_MODEL_NUM - 1; i++)
				{
					g_Enemy[i].Pos -= g_PunchEndVec[i];
					g_Enemy[i].Rot -= g_PunchEndRot[i];
				}
			}
			else
			{//�X�^�[�g�ʒu�ɖ߂�����p���`�������I������
				//�t���[���F�t�F�[�Y��������
				g_PunchFrameCnt = 0;
				g_PunchPhase = PUNCH_PHASE_CHARGE;
				//�����ʒu�ɖ߂�
				EnemyPosReset();

				//�p���`�I��
				Punch_Flg = false;
				isTaiki = true;
				g_WaitTime = rand() % 60 + 120;

				//�p���`�񐔉��Z(�f�o�b�O�p�ɖ������[�v����)
				if (g_PunchCnt < NUM_PUNCH - 1)
				{
					g_PunchCnt++;
				}
				else
				{
					g_PunchCnt = 0;
				}
			}
		}
	}
}
//���ʏ�p���`(PPI_LEFT_PUNCH)(�쐬�ҁFAmalgam�ANo.02)
void LeftPunch()
{
	//�ڕW�n�_�ւ̃x�N�g��
	static D3DXVECTOR2 dir;

	//�t���[���i�s
	g_PunchFrameCnt++;

	if (g_PunchPhase == PUNCH_PHASE_CHARGE)
	{//�\������
		if (g_PunchFrameCnt < PPI_L_P_START_FRAME)
		{
			if (g_PunchFrameCnt < 30)
			{
				//�E��������\������
				g_Enemy[2].Pos.z += 8.0f / PPI_R_P_START_FRAME;
			}

			if (g_PunchFrameCnt == 1)
			{
				//Punch_Charge_Effect();
				Charge_Start();
				g_Enemy[2].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(75.0f), D3DXToRadian(10.0f));
				PunchLR = false;
			}
		}
		else
		{
			//�v���C���[�̓��ւ̃x�N�g�����o��
			dir.x = 0.0f - g_Enemy[2].Pos.x;
			dir.y = 8.0f - g_Enemy[2].Pos.y;
			D3DXVec2Normalize(&dir, &dir);

			//�t�F�[�Y�i�s
			g_PunchPhase = PUNCH_PHASE_SWING;
		}
	}
	else if (g_PunchPhase == PUNCH_PHASE_SWING)
	{//�p���`�{��
		if (g_Enemy[2].Pos.z >= g_PunchEndLine)
		{//�G���h���C���ɓ��B���ĂȂ���Γ�����(�v���C���[�͓G����݂�Z�}�C�i�X�����ɂ���)
			if (SlowFlg)
			{//�X���[�G���A���������瑬�x�ɉ����␳
				g_Enemy[2].Pos += D3DXVECTOR3((dir.x / 2.0f) * 0.1f, (dir.y / 2.0f) * 0.1f, -(Punch_Speed / 10.0f));
			}
			else
			{
				g_Enemy[2].Pos += D3DXVECTOR3(dir.x / 2.0f, dir.y / 1.5f, -(Punch_Speed));
			}
		}
		else if (g_Enemy[2].Pos.z < g_PunchEndLine)
		{//���B������I���
			//��X�����̂łO�ɂ���
			g_PunchFrameCnt = 0;
			//�t�F�[�Y�i�s
			g_PunchPhase = PUNCH_PHASE_RETURN;
		}
	}
	else if (g_PunchPhase == PUNCH_PHASE_RETURN)
	{
		if (g_PunchFrameCnt == 1)
		{//�ŏ��̃t���[���ɖ߂�p�̃x�N�g�������
			CreatePunchEndVec();
		}
		else if (g_PunchFrameCnt >= 3)
		{//���t���[���܂��Ă��猳�̈ʒu�ɖ߂�
			if (g_PunchFrameCnt < g_EndFrameList[g_PunchPattern[g_PunchCnt]])
			{
				for (int i = 0; i < ENEMY_MODEL_NUM - 1; i++)
				{
					g_Enemy[i].Pos -= g_PunchEndVec[i];
					g_Enemy[i].Rot -= g_PunchEndRot[i];
				}
			}
			else
			{//�X�^�[�g�ʒu�ɖ߂�����p���`�������I������
				//�t���[���F�t�F�[�Y��������
				g_PunchFrameCnt = 0;
				g_PunchPhase = PUNCH_PHASE_CHARGE;
				//�����ʒu�ɖ߂�
				EnemyPosReset();

				//�p���`�I��
				Punch_Flg = false;
				isTaiki = true;
				g_WaitTime = rand() % 60 + 120;

				//�p���`�񐔉��Z(�f�o�b�O�p�ɖ������[�v����)
				if (g_PunchCnt < NUM_PUNCH - 1)
				{
					g_PunchCnt++;
				}
				else
				{
					g_PunchCnt = 0;
				}
			}
		}
	}
}
//�E�u�Ԉړ��p���`(R_FLASH_PUNCH)(�쐬�ҁF�ʁANo.01)
void Right_R_FLASH_PUNCH()
{
	//�t���[���i�s
	g_PunchFrameCnt++;

	if (g_PunchPhase == PUNCH_PHASE_CHARGE)
	{//�\������
		if (g_PunchFrameCnt < FLASH_P_START_FRAME)
		{

		}
		else
		{
			R_FLASH_PUNCH_R();
			g_PunchPhase = PUNCH_PHASE_SWING;
			g_PunchFrameCnt = 0;
			g_Slow_Multi = 1.0f;
		}
		if (g_PunchFrameCnt == 1)
		{
			PunchLR = true;
			Charge_Start();
			//Punch_Charge_Effect();
		}

	}
	else if (g_PunchPhase == PUNCH_PHASE_SWING)
	{//�p���`�{��
		if (!(g_PunchFrameCnt < 20))
		{
			if (g_PunchFrameCnt < FLASH_P_SLOW_FRAME + 20)
			{
				//�����
				for (int j = 0; j < ENEMY_MODEL_NUM - 1; j++)
				{
					for (DWORD i = 0; i < g_Enemy[j].dwNumMaterials; i++)
					{
						g_Enemy[j].pMaterials[i].Diffuse.a = 1.0f;
					}
				}


				//�ړ�
				g_Enemy[0].Rot.y -= D3DXToRadian(35.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi);		//35.0f�͍ŏI�I�ɉ�]���鑍�p�x
				g_Enemy[2].Pos += D3DXVECTOR3(1.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi, 2.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi, 8.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi);
				g_Enemy[3].Pos += D3DXVECTOR3(28.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi, -20.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi, -35.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi);
				g_Enemy[3].Rot += D3DXVECTOR3(D3DXToRadian(50.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi), D3DXToRadian(-75.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi), 0.0f);
			}
			else
			{//���B������I���
				//��X�����̂łO�ɂ���
				g_PunchFrameCnt = 0;
				//�t�F�[�Y�i�s
				g_PunchPhase = PUNCH_PHASE_RETURN;
			}
		}
		else
		{
			//������
			for (int j = 0; j < ENEMY_MODEL_NUM - 1; j++)
			{
				for (DWORD i = 0; i < g_Enemy[j].dwNumMaterials; i++)
				{
					g_Enemy[j].pMaterials[i].Diffuse.a = 0.0f;
				}
			}
		}
	}
	else if (g_PunchPhase == PUNCH_PHASE_RETURN)
	{
		if (g_PunchFrameCnt > 3)
		{
			{//�X�^�[�g�ʒu�ɖ߂�����p���`�������I������
				//�t���[���F�t�F�[�Y��������
				g_PunchFrameCnt = 0;
				g_PunchPhase = PUNCH_PHASE_CHARGE;
				//�����ʒu�ɖ߂�
				EnemyPosReset();

				//�p���`�I��
				Punch_Flg = false;
				isTaiki = true;
				//�p���`�������Ă��Ȃ���Ԃɂ��ǂ�
				g_Punch_Pattern_Index = PUNCH_NULL;

				g_WaitTime = rand() % 60 + 120;

				//�p���`�񐔉��Z(�f�o�b�O�p�ɖ������[�v����)
				if (g_PunchCnt < NUM_PUNCH - 1)
				{
					g_PunchCnt++;
				}
				else
				{
					g_PunchCnt = 0;
				}
			}
		}
	}
}
//���u�Ԉړ��p���`(L_FLASH_PUNCH)(�쐬��:�ʁANo.02)
void  Left_R_FLASH_PUNCH()
{
	//�t���[���i�s
	g_PunchFrameCnt++;

	if (g_PunchPhase == PUNCH_PHASE_CHARGE)
	{//�\������
		if (g_PunchFrameCnt < FLASH_P_START_FRAME)
		{

		}
		else
		{
			L_FLASH_PUNCH_R();
			g_PunchPhase = PUNCH_PHASE_SWING;
			g_PunchFrameCnt = 0;
			g_Slow_Multi = 1.0f;
		}
		if (g_PunchFrameCnt == 1)
		{
			PunchLR = false;
			Charge_Start();
			//Punch_Charge_Effect();
		}
	}
	else if (g_PunchPhase == PUNCH_PHASE_SWING)
	{//�p���`�{��
		if (!(g_PunchFrameCnt < 20))
		{
			if (g_PunchFrameCnt < FLASH_P_SLOW_FRAME + 20)
			{
				//�����
				for (int j = 0; j < ENEMY_MODEL_NUM - 1; j++)
				{
					for (DWORD i = 0; i < g_Enemy[j].dwNumMaterials; i++)
					{
						g_Enemy[j].pMaterials[i].Diffuse.a = 1.0f;
					}
				}

				//�ړ�
				g_Enemy[0].Rot.y -= D3DXToRadian(-35.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi);  //35.0f�͍ŏI�I�ɉ�]���鑍�p�x
				g_Enemy[3].Pos += D3DXVECTOR3(-1.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi, 2.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi, 8.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi);
				g_Enemy[2].Pos += D3DXVECTOR3(-23.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi, -20.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi, -36.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi);
				g_Enemy[2].Rot += D3DXVECTOR3(D3DXToRadian(-85.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi), D3DXToRadian(0.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi), D3DXToRadian(-28.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi));

			}
			else
			{//���B������I���
			 //��X�����̂łO�ɂ���
				g_PunchFrameCnt = 0;
				//�t�F�[�Y�i�s
				g_PunchPhase = PUNCH_PHASE_RETURN;
			}
		}
		else
		{
			//������
			for (int j = 0; j < ENEMY_MODEL_NUM - 1; j++)
			{
				for (DWORD i = 0; i < g_Enemy[j].dwNumMaterials; i++)
				{
					g_Enemy[j].pMaterials[i].Diffuse.a = 0.0f;
				}
			}
		}
	}
	else if (g_PunchPhase == PUNCH_PHASE_RETURN)
	{
		if (g_PunchFrameCnt > 3)
		{
			{//�X�^�[�g�ʒu�ɖ߂�����p���`�������I������
			 //�t���[���F�t�F�[�Y��������
				g_PunchFrameCnt = 0;
				g_PunchPhase = PUNCH_PHASE_CHARGE;
				//�����ʒu�ɖ߂�
				EnemyPosReset();

				//�p���`�I��
				Punch_Flg = false;
				isTaiki = true;
				//�p���`�������Ă��Ȃ���Ԃɂ��ǂ�
				g_Punch_Pattern_Index = PUNCH_NULL;

				g_WaitTime = rand() % 60 + 120;

				//�p���`�񐔉��Z(�f�o�b�O�p�ɖ������[�v����)
				if (g_PunchCnt < NUM_PUNCH - 1)
				{
					g_PunchCnt++;
				}
				else
				{
					g_PunchCnt = 0;
				}
			}
		}
	}
}
//����o�[���Ă���p���`(PPI_DUNK_PUNCH)(�쐬�ҁFAmalgam�ANo.03)
void DunkPunch()
{
	//�ڕW�n�_�ւ̃x�N�g��
	static D3DXVECTOR3 dir;

	//�t���[���i�s
	g_PunchFrameCnt++;

	if (g_PunchPhase == PUNCH_PHASE_CHARGE)
	{//�\������
		if (g_PunchFrameCnt < PPI_D_P_START_FRAME)
		{
			if (g_PunchFrameCnt < 30)
			{
				//�\������

				//��
				g_Enemy[0].Pos.y += 2.0f / 30;
				g_Enemy[0].Rot.x += D3DXToRadian(15.0f) / 30;
				//��
				g_Enemy[1].Pos.y += 2.0f / 30;
				g_Enemy[1].Pos.z += 2.0f / 30;
				g_Enemy[1].Rot.x += D3DXToRadian(15.0f) / 30;

				//���r
				g_Enemy[2].Pos += D3DXVECTOR3(3.0f, 20.0f, 4.0f) / 30;
				g_Enemy[2].Rot.x += D3DXToRadian(-20.0f) / 30;
				//�E�r
				g_Enemy[3].Pos += D3DXVECTOR3(-3.0f, 20.0f, 4.0f) / 30;
				g_Enemy[3].Rot.x += D3DXToRadian(-20.0f) / 30;
			}

			if (g_PunchFrameCnt == 1)
			{
				//���r
				g_Enemy[2].Pos = D3DXVECTOR3(7.0f, 15.0f, ENEMY_POS_Z);
				//�E�r
				g_Enemy[3].Pos = D3DXVECTOR3(-7.0f, 15.0f, ENEMY_POS_Z);
				Charge_Start();
				//Punch_Charge_Effect();
				PunchLR = true;
			}
		}
		else
		{
			//�v���C���[�̑����ւ̃x�N�g�����o��
			dir.y = 0.0f - g_Enemy[3].Pos.y;
			dir.z = g_PunchEndLine - g_Enemy[3].Pos.z;
			D3DXVec3Normalize(&dir, &dir);

			//��X�����̂łO�ɂ���
			g_PunchFrameCnt = 0;
			//�t�F�[�Y�i�s
			g_PunchPhase = PUNCH_PHASE_SWING;
		}
	}
	else if (g_PunchPhase == PUNCH_PHASE_SWING)
	{//�p���`�{��
		if (g_Enemy[3].Pos.z >= g_PunchEndLine)
		{//�G���h���C���ɓ��B���ĂȂ���Γ�����(�v���C���[�͓G����݂�Z�}�C�i�X�����ɂ���)
			if (SlowFlg)
			{//�X���[�G���A���������瑬�x�ɉ����␳
				//���A���̈ړ��E��]
				g_Enemy[0].Pos.y -= 0.05f * 0.2f;
				g_Enemy[0].Rot.x -= D3DXToRadian(0.5f) * 0.2f;

				g_Enemy[1].Pos.y -= 0.08f * 0.2f;
				g_Enemy[1].Pos.z -= 0.1f * 0.2f;
				g_Enemy[1].Rot.x -= D3DXToRadian(0.5f) * 0.2f;

				//���r
				g_Enemy[2].Pos.x += 0.0f;
				g_Enemy[2].Pos.y += dir.y * 0.2f;
				g_Enemy[2].Pos.z += dir.z * 0.2f;
				//�E�r
				g_Enemy[3].Pos.x += 0.0f;
				g_Enemy[3].Pos.y += dir.y * 0.2f;
				g_Enemy[3].Pos.z += dir.z * 0.2f;
			}
			else
			{
				//���A���̈ړ��E��]
				g_Enemy[0].Pos.y -= 0.05f * 1.5f;
				g_Enemy[0].Rot.x -= D3DXToRadian(0.5f) * 1.5f;

				g_Enemy[1].Pos.y -= 0.08f * 1.5f;
				g_Enemy[1].Pos.z -= 0.1f * 1.5f;
				g_Enemy[1].Rot.x -= D3DXToRadian(0.5f) * 1.5f;

				//���r
				g_Enemy[2].Pos.x += 0.0f;
				g_Enemy[2].Pos.y += dir.y * 3.0f;
				g_Enemy[2].Pos.z += dir.z * 3.0f;
				//�E�r
				g_Enemy[3].Pos.x += 0.0f;
				g_Enemy[3].Pos.y += dir.y * 3.0f;
				g_Enemy[3].Pos.z += dir.z * 3.0f;
			}

			//�r�̉�]
			if (g_PunchFrameCnt < 10)
			{
				g_Enemy[3].Rot.x += D3DXToRadian(90.0f) / 10;
				g_Enemy[2].Rot.x += D3DXToRadian(90.0f) / 10;
			}
		}
		else
		{//���B������I���
			//��X�����̂łO�ɂ���
			g_PunchFrameCnt = 0;
			//�t�F�[�Y�i�s
			g_PunchPhase = PUNCH_PHASE_RETURN;
		}
	}
	else if (g_PunchPhase == PUNCH_PHASE_RETURN)
	{
		if (g_PunchFrameCnt == 1)
		{//�ŏ��̃t���[���ɖ߂�p�̃x�N�g�������
			CreatePunchEndVec();
		}
		else if (g_PunchFrameCnt >= 3)
		{//���t���[���܂��Ă��猳�̈ʒu�ɖ߂�
			if (g_PunchFrameCnt < g_EndFrameList[g_PunchPattern[g_PunchCnt]])
			{
				for (int i = 0; i < ENEMY_MODEL_NUM - 1; i++)
				{
					g_Enemy[i].Pos -= g_PunchEndVec[i];
					g_Enemy[i].Rot -= g_PunchEndRot[i];
				}
			}
			else
			{//�X�^�[�g�ʒu�ɖ߂�����p���`�������I������
				//�t���[���F�t�F�[�Y��������
				g_PunchFrameCnt = 0;
				g_PunchPhase = PUNCH_PHASE_CHARGE;
				//�����ʒu�ɖ߂�
				EnemyPosReset();

				//�p���`�I��
				Punch_Flg = false;
				isTaiki = true;
				g_WaitTime = rand() % 60 + 120;

				//�p���`�񐔉��Z(�f�o�b�O�p�ɖ������[�v����)
				if (g_PunchCnt < NUM_PUNCH - 1)
				{
					g_PunchCnt++;
				}
				else
				{
					g_PunchCnt = 0;
				}
			}
		}
	}
}

//�W�����s���O�p���`��(PPI_L_JUMP_PUNCH)(�쐬�ҁFAmalgam�ANo.04)
void JumpPunch_L()
{
	//�ڕW�n�_�ւ̃x�N�g��
	static D3DXVECTOR3 dir;

	//�t���[���i�s
	g_PunchFrameCnt++;

	if (g_PunchPhase == PUNCH_PHASE_CHARGE)
	{//�\������
		if (g_PunchFrameCnt < PPI_JP_START_FRAME)
		{
			if (g_PunchFrameCnt == 1)
			{
				Charge_Start();
				//Punch_Charge_Effect();
				PunchLR = true;
			}
			//�O�b
			if (g_PunchFrameCnt < 10)
			{
				//����
				g_Enemy[0].Pos.y -= 0.2f;

				//��
				g_Enemy[1].Pos.y -= 0.3f;
				g_Enemy[1].Rot.x -= D3DXToRadian(1.5f);

				//���r
				g_Enemy[2].Pos.x -= 0.2f;
				g_Enemy[2].Pos.y -= 0.3f;

				//�E�r
				g_Enemy[3].Pos.x += 0.2f;
				g_Enemy[3].Pos.y -= 0.3f;
			}
			//�O2
			else if (g_PunchFrameCnt > 10 && g_PunchFrameCnt < 15)
			{
				//����
				g_Enemy[0].Pos.y -= 0.4f;

				//��
				g_Enemy[1].Pos.y -= 0.6f;
				g_Enemy[1].Rot.x -= D3DXToRadian(3.0f);

				//���r
				g_Enemy[2].Pos.x -= 0.4f;
				g_Enemy[2].Pos.y -= 0.5f;

				//�E�r
				g_Enemy[3].Pos.x += 0.4f;
				g_Enemy[3].Pos.y -= 0.6f;
			}
			//�s����1
			else if (g_PunchFrameCnt > 45 && g_PunchFrameCnt < 50)
			{
				//��
				g_Enemy[0].Pos.y += 1.0f;
				g_Enemy[0].Rot.x += D3DXToRadian(1.0f);

				//��
				g_Enemy[1].Pos.y += 1.0f;
				g_Enemy[1].Rot.x += D3DXToRadian(10.0f);

				//���r
				g_Enemy[2].Pos.x += 1.0f;
				g_Enemy[2].Pos.y += 4.0f;

				//�E�r
				g_Enemy[3].Pos.x -= 0.6f;
				g_Enemy[3].Pos.y += 0.1f;

				//����
				g_Enemy[4].Pos.y += 0.5f;
				g_Enemy[4].Rot.x -= D3DXToRadian(20.0f);

				//�E��
				g_Enemy[5].Pos.y += 0.5f;
				g_Enemy[5].Rot.x -= D3DXToRadian(20.0f);
			}
			//�s����2
			else if (g_PunchFrameCnt > 50 && g_PunchFrameCnt < 65)
			{
				for (int i = 0; i < ENEMY_MODEL_NUM - 1; i++)
				{
					g_Enemy[i].Pos.y += 1.0f;
					g_Enemy[i].Pos.z += 0.5f;
				}
			}
			//�O�[�b1
			else if (g_PunchFrameCnt == 66)
			{
				//��
				g_Enemy[0].Rot.x = D3DXToRadian(-20.0f);

				//��
				g_Enemy[1].Pos.y -= 2.0f;
				g_Enemy[1].Pos.z -= 5.0f;
				g_Enemy[1].Rot.x = D3DXToRadian(-20.0f);

				//���r
				g_Enemy[2].Pos.y -= 8.0f;
				g_Enemy[2].Rot = D3DXVECTOR3(D3DXToRadian(10.0f), D3DXToRadian(120.0f), D3DXToRadian(0.0f));
			}
			//�O�[�b2(�G�t�F�N�g�����Ȃ炱��)
			else if (g_PunchFrameCnt > 70 && g_PunchFrameCnt < 85)
			{
				//���r
				g_Enemy[2].Pos.x += 0.09f;
				g_Enemy[2].Pos.y += 0.09f;
				g_Enemy[2].Pos.z += 0.25f;

				PunchLR = false;
			}

		}
		else
		{
			//�v���C���[�̓��ւ̃x�N�g�����o��
			dir.x = 0.0f - g_Enemy[2].Pos.x;
			dir.y = 8.0f - g_Enemy[2].Pos.y;
			dir.z = g_PunchEndLine - g_Enemy[2].Pos.z;
			D3DXVec3Normalize(&dir, &dir);

			//�t�F�[�Y�i�s
			g_PunchPhase = PUNCH_PHASE_SWING;
			g_PunchFrameCnt = 0;
		}
	}
	else if (g_PunchPhase == PUNCH_PHASE_SWING)
	{//�p���`�{��
		if (g_Enemy[2].Pos.z >= g_PunchEndLine)
		{//�G���h���C���ɓ��B���ĂȂ���Γ�����(�v���C���[�͓G����݂�Z�}�C�i�X�����ɂ���)
			if (SlowFlg)
			{
				//����
				g_Enemy[0].Pos.y += dir.y * 0.25f;
				g_Enemy[0].Pos.z += dir.z * 0.25f;
				g_Enemy[0].Rot.x -= D3DXToRadian(0.5f);
				g_Enemy[0].Rot.y += D3DXToRadian(0.5f);

				//��
				g_Enemy[1].Pos.x -= 0.05f;
				g_Enemy[1].Pos.y += dir.y * 0.25f;
				g_Enemy[1].Pos.z += dir.z * 0.25f - 0.05f;
				g_Enemy[1].Rot.x -= D3DXToRadian(0.5f);
				g_Enemy[1].Rot.y += D3DXToRadian(0.5f);

				//�E�r
				g_Enemy[2].Pos.x += dir.x * 0.5f;
				g_Enemy[2].Pos.y += dir.y * 0.5f;
				g_Enemy[2].Pos.z += dir.z * 0.5f;
			}
			else
			{
				//����
				g_Enemy[0].Pos.y += dir.y * 0.5f;
				g_Enemy[0].Pos.z += dir.z * 0.5f;
				g_Enemy[0].Rot.x += D3DXToRadian(1.0f);
				g_Enemy[0].Rot.y += D3DXToRadian(1.0f);

				//��
				g_Enemy[1].Pos.x -= 0.1f;
				g_Enemy[1].Pos.y += dir.y * 0.5f;
				g_Enemy[1].Pos.z += dir.z * 0.5f - 0.1f;
				g_Enemy[1].Rot.x += D3DXToRadian(1.0f);
				g_Enemy[1].Rot.y += D3DXToRadian(1.0f);

				//�E�r
				g_Enemy[2].Pos.x += dir.x * 3.0f;
				g_Enemy[2].Pos.y += dir.y * 3.0f;
				g_Enemy[2].Pos.z += dir.z * 3.0f;
			}
		}
		else if (g_Enemy[2].Pos.z < g_PunchEndLine)
		{//���B������I���
			//��X�����̂łO�ɂ���
			g_PunchFrameCnt = 0;
			//�t�F�[�Y�i�s
			g_PunchPhase = PUNCH_PHASE_RETURN;
		}
	}
	else if (g_PunchPhase == PUNCH_PHASE_RETURN)
	{
		if (g_PunchFrameCnt == 1)
		{//�ŏ��̃t���[���ɖ߂�p�̃x�N�g�������
			CreatePunchEndVec();
		}
		else if (g_PunchFrameCnt >= 3)
		{//���t���[���܂��Ă��猳�̈ʒu�ɖ߂�
			if (g_PunchFrameCnt < g_EndFrameList[g_PunchPattern[g_PunchCnt]])
			{
				for (int i = 0; i < ENEMY_MODEL_NUM - 1; i++)
				{
					g_Enemy[i].Pos -= g_PunchEndVec[i];
					g_Enemy[i].Rot -= g_PunchEndRot[i];
				}
			}
			else
			{//�X�^�[�g�ʒu�ɖ߂�����p���`�������I������
				//�t���[���F�t�F�[�Y��������
				g_PunchFrameCnt = 0;
				g_PunchPhase = PUNCH_PHASE_CHARGE;
				//�����ʒu�ɖ߂�
				EnemyPosReset();

				//�p���`�I��
				Punch_Flg = false;
				isTaiki = true;
				g_WaitTime = rand() % 60 + 120;

				//�p���`�񐔉��Z(�f�o�b�O�p�ɖ������[�v����)
				if (g_PunchCnt < NUM_PUNCH - 1)
				{
					g_PunchCnt++;
				}
				else
				{
					g_PunchCnt = 0;
				}
			}
		}
	}
}
//�W�����s���O�p���`�E(PPI_R_JUMP_PUNCH)(�쐬�ҁFAmalgam�ANo.05)
void JumpPunch_R()
{
	//�ڕW�n�_�ւ̃x�N�g��
	static D3DXVECTOR3 dir;

	//�t���[���i�s
	g_PunchFrameCnt++;

	if (g_PunchPhase == PUNCH_PHASE_CHARGE)
	{//�\������
		if (g_PunchFrameCnt < PPI_JP_START_FRAME)
		{
			if (g_PunchFrameCnt == 1)
			{
				Charge_Start();
				//Punch_Charge_Effect();
				PunchLR = false;
			}

			//�O�b
			if (g_PunchFrameCnt < 10)
			{
				//����
				g_Enemy[0].Pos.y -= 0.2f;

				//��
				g_Enemy[1].Pos.y -= 0.3f;
				g_Enemy[1].Rot.x -= D3DXToRadian(1.5f);

				//���r
				g_Enemy[2].Pos.x -= 0.2f;
				g_Enemy[2].Pos.y -= 0.3f;

				//�E�r
				g_Enemy[3].Pos.x += 0.2f;
				g_Enemy[3].Pos.y -= 0.3f;
			}
			//�O2
			else if (g_PunchFrameCnt > 10 && g_PunchFrameCnt < 15)
			{
				//����
				g_Enemy[0].Pos.y -= 0.4f;

				//��
				g_Enemy[1].Pos.y -= 0.6f;
				g_Enemy[1].Rot.x -= D3DXToRadian(3.0f);

				//���r
				g_Enemy[2].Pos.x -= 0.4f;
				g_Enemy[2].Pos.y -= 0.5f;

				//�E�r
				g_Enemy[3].Pos.x += 0.4f;
				g_Enemy[3].Pos.y -= 0.6f;
			}
			//�s����1
			else if (g_PunchFrameCnt > 45 && g_PunchFrameCnt < 50)
			{
				//��
				g_Enemy[0].Pos.y += 1.0f;
				g_Enemy[0].Rot.x += D3DXToRadian(1.0f);

				//��
				g_Enemy[1].Pos.y += 1.0f;
				g_Enemy[1].Rot.x += D3DXToRadian(10.0f);

				//���r
				g_Enemy[2].Pos.x += 0.6f;
				g_Enemy[2].Pos.y += 0.1f;

				//�E�r
				g_Enemy[3].Pos.x -= 1.0f;
				g_Enemy[3].Pos.y += 4.0f;

				//����
				g_Enemy[4].Pos.y += 0.5f;
				g_Enemy[4].Rot.x -= D3DXToRadian(20.0f);

				//�E��
				g_Enemy[5].Pos.y += 0.5f;
				g_Enemy[5].Rot.x -= D3DXToRadian(20.0f);
			}
			//�s����2
			else if (g_PunchFrameCnt > 50 && g_PunchFrameCnt < 65)
			{
				for (int i = 0; i < ENEMY_MODEL_NUM - 1; i++)
				{
					g_Enemy[i].Pos.y += 1.0f;
					g_Enemy[i].Pos.z += 0.5f;
				}
			}
			//�O�[�b1
			else if (g_PunchFrameCnt == 66)
			{
				//��
				g_Enemy[0].Rot.x = D3DXToRadian(-20.0f);

				//��
				g_Enemy[1].Pos.y -= 2.0f;
				g_Enemy[1].Pos.z -= 5.0f;
				g_Enemy[1].Rot.x = D3DXToRadian(-20.0f);

				//�E�r
				g_Enemy[3].Pos.y -= 8.0f;
				g_Enemy[3].Rot = D3DXVECTOR3(D3DXToRadian(10.0f), D3DXToRadian(-90.0f), D3DXToRadian(0.0f));
			}
			//�O�[�b2(�G�t�F�N�g�����Ȃ炱��)
			else if (g_PunchFrameCnt > 70 && g_PunchFrameCnt < 85)
			{
				//���r
				g_Enemy[3].Pos.x -= 0.09f;
				g_Enemy[3].Pos.y += 0.09f;
				g_Enemy[3].Pos.z += 0.25f;

				PunchLR = true;
			}
		}
		else
		{
			//�v���C���[�̓��ւ̃x�N�g�����o��
			dir.x = 0.0f - g_Enemy[3].Pos.x;
			dir.y = 8.0f - g_Enemy[3].Pos.y;
			dir.z = g_PunchEndLine - g_Enemy[3].Pos.z;
			D3DXVec3Normalize(&dir, &dir);

			//�t�F�[�Y�i�s
			g_PunchPhase = PUNCH_PHASE_SWING;
			g_PunchFrameCnt = 0;
		}
	}
	else if (g_PunchPhase == PUNCH_PHASE_SWING)
	{//�p���`�{��
		if (g_Enemy[3].Pos.z >= g_PunchEndLine)
		{//�G���h���C���ɓ��B���ĂȂ���Γ�����(�v���C���[�͓G����݂�Z�}�C�i�X�����ɂ���)
			if (SlowFlg)
			{
				//����
				g_Enemy[0].Pos.y += dir.y * 0.25f;
				g_Enemy[0].Pos.z += dir.z * 0.25f;
				g_Enemy[0].Rot.x -= D3DXToRadian(0.5f);
				g_Enemy[0].Rot.y -= D3DXToRadian(0.5f);

				//��
				g_Enemy[1].Pos.x += 0.05f;
				g_Enemy[1].Pos.y += dir.y * 0.25f;
				g_Enemy[1].Pos.z += dir.z * 0.25f - 0.05f;
				g_Enemy[1].Rot.x -= D3DXToRadian(0.5f);
				g_Enemy[1].Rot.y -= D3DXToRadian(0.5f);

				//�E�r
				g_Enemy[3].Pos.x += dir.x * 0.5f;
				g_Enemy[3].Pos.y += dir.y * 0.5f;
				g_Enemy[3].Pos.z += dir.z * 0.5f;
			}
			else
			{
				//����
				g_Enemy[0].Pos.y += dir.y * 0.5f;
				g_Enemy[0].Pos.z += dir.z * 0.5f;
				g_Enemy[0].Rot.x -= D3DXToRadian(1.0f);
				g_Enemy[0].Rot.y -= D3DXToRadian(1.0f);

				//��
				g_Enemy[1].Pos.x += 0.1f;
				g_Enemy[1].Pos.y += dir.y * 0.5f;
				g_Enemy[1].Pos.z += dir.z * 0.5f - 0.1f;
				g_Enemy[1].Rot.x -= D3DXToRadian(1.0f);
				g_Enemy[1].Rot.y -= D3DXToRadian(1.0f);

				//�E�r
				g_Enemy[3].Pos.x += dir.x * 3.0f;
				g_Enemy[3].Pos.y += dir.y * 3.0f;
				g_Enemy[3].Pos.z += dir.z * 3.0f;
			}
		}
		else if (g_Enemy[3].Pos.z < g_PunchEndLine)
		{//���B������I���
			//��X�����̂łO�ɂ���
			g_PunchFrameCnt = 0;
			//�t�F�[�Y�i�s
			g_PunchPhase = PUNCH_PHASE_RETURN;
		}
	}
	else if (g_PunchPhase == PUNCH_PHASE_RETURN)
	{
		if (g_PunchFrameCnt == 1)
		{//�ŏ��̃t���[���ɖ߂�p�̃x�N�g�������
			CreatePunchEndVec();
		}
		else if (g_PunchFrameCnt >= 3)
		{//���t���[���܂��Ă��猳�̈ʒu�ɖ߂�
			if (g_PunchFrameCnt < g_EndFrameList[g_PunchPattern[g_PunchCnt]])
			{
				for (int i = 0; i < ENEMY_MODEL_NUM - 1; i++)
				{
					g_Enemy[i].Pos -= g_PunchEndVec[i];
					g_Enemy[i].Rot -= g_PunchEndRot[i];
				}
			}
			else
			{//�X�^�[�g�ʒu�ɖ߂�����p���`�������I������
				//�t���[���F�t�F�[�Y��������
				g_PunchFrameCnt = 0;
				g_PunchPhase = PUNCH_PHASE_CHARGE;
				//�����ʒu�ɖ߂�
				EnemyPosReset();

				//�p���`�I��
				Punch_Flg = false;
				isTaiki = true;
				g_WaitTime = rand() % 60 + 120;

				//�p���`�񐔉��Z(�f�o�b�O�p�ɖ������[�v����)
				if (g_PunchCnt < NUM_PUNCH - 1)
				{
					g_PunchCnt++;
				}
				else
				{
					g_PunchCnt = 0;
				}
			}
		}
	}
}


//�ǉ�����8===============================
//�Ō�̃p���`
void Last_Punch()
{
	//�t���[���i�s
	g_PunchFrameCnt++;

	if (g_Last_Punch_Phase == PUNCH_PHASE_LCHARGE)
	{//�\������
		//���߃|�[�Y
		if (g_PunchFrameCnt < LAST_PUNCH_CHAGE_FRAME + 60)
		{
			//���߂����邽�߂ɐ�ɓ������~�܂�
			if (g_PunchFrameCnt < LAST_PUNCH_CHAGE_FRAME)
			{
				g_Enemy[1].Rot.x += D3DXToRadian(20.0f) / LAST_PUNCH_CHAGE_FRAME;
				g_Enemy[2].Pos += D3DXVECTOR3(-7.0f / LAST_PUNCH_CHAGE_FRAME, 15.0f / LAST_PUNCH_CHAGE_FRAME, -5.0f / LAST_PUNCH_CHAGE_FRAME);
				g_Enemy[3].Pos += D3DXVECTOR3(7.0f / LAST_PUNCH_CHAGE_FRAME, 15.0f / LAST_PUNCH_CHAGE_FRAME, -5.0f / LAST_PUNCH_CHAGE_FRAME);
			}
		}
		else
		{
			g_PunchFrameCnt = 0;
			//�t�F�[�Y�i�s
			g_Last_Punch_Phase = PUNCH_PHASE_WAVE;
		}
	}
	else if (g_Last_Punch_Phase == PUNCH_PHASE_WAVE)
	{//�v���C���[��������΂����
		if (g_PunchFrameCnt < LAST_PUNCH_WAVE_FRAME)
		{
			//�g�����[�V����
			g_Enemy[1].Rot = D3DXVECTOR3(D3DXToRadian(22.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));
			g_Enemy[2].Pos = D3DXVECTOR3(10.0f, 14.0f, 22.6f);
			g_Enemy[2].Rot = D3DXVECTOR3(D3DXToRadian(10.0f), D3DXToRadian(192.0f), D3DXToRadian(90.0f));
			g_Enemy[3].Pos = D3DXVECTOR3(-11.0f, 15.6f, 26.0f);
			g_Enemy[3].Rot = D3DXVECTOR3(D3DXToRadian(15.0f), D3DXToRadian(180.0f), D3DXToRadian(270.0f));

			//�v���C���[��������΂����
			for (int i = 0; i < 6; i++)
			{
				(GetPlayer() + i)->Pos.z -= 20.0f / g_PunchFrameCnt;
			}
		}
		else
		{
			g_PunchFrameCnt = 0;
			//�t�F�[�Y�i�s
			g_Last_Punch_Phase = PUNCH_PHASE_SET;
		}
	}
	else if (g_Last_Punch_Phase == PUNCH_PHASE_SET)
	{
		if (g_PunchFrameCnt < LAST_PUNCH_SET_FRAME)
		{
			//�\�����Ƃ�
			g_Enemy[0].Rot += D3DXVECTOR3(0, D3DXToRadian(60.0f) / LAST_PUNCH_SET_FRAME, 0);
			g_Enemy[1].Pos += D3DXVECTOR3(0, -1.0f / LAST_PUNCH_SET_FRAME, 0);
			g_Enemy[1].Rot += D3DXVECTOR3(D3DXToRadian(-20.0f) / LAST_PUNCH_SET_FRAME, 0, 0);
			g_Enemy[2].Pos += D3DXVECTOR3(-10.0f / LAST_PUNCH_SET_FRAME, 3.4f / LAST_PUNCH_SET_FRAME, 0);
			g_Enemy[2].Rot += D3DXVECTOR3(D3DXToRadian(5.0f) / LAST_PUNCH_SET_FRAME, D3DXToRadian(50.0f) / LAST_PUNCH_SET_FRAME, 0);
			g_Enemy[3].Pos += D3DXVECTOR3(4.0f / LAST_PUNCH_SET_FRAME, 2.4f / LAST_PUNCH_SET_FRAME, 6.0f / LAST_PUNCH_SET_FRAME);
			g_Enemy[3].Rot += D3DXVECTOR3(D3DXToRadian(-5.0f) / LAST_PUNCH_SET_FRAME, D3DXToRadian(100.0f) / LAST_PUNCH_SET_FRAME, D3DXToRadian(100.0f) / LAST_PUNCH_SET_FRAME);
			g_Enemy[4].Pos += D3DXVECTOR3(-7.0f / LAST_PUNCH_SET_FRAME, 0.0f / LAST_PUNCH_SET_FRAME, -10.0f / LAST_PUNCH_SET_FRAME);
			g_Enemy[4].Rot += D3DXVECTOR3(0, D3DXToRadian(12.0f) / LAST_PUNCH_SET_FRAME, 0);
			g_Enemy[5].Pos += D3DXVECTOR3(0, 0, 10.0f / LAST_PUNCH_SET_FRAME);
			g_Enemy[5].Rot += D3DXVECTOR3(D3DXToRadian(-20.0f) / LAST_PUNCH_SET_FRAME, 0, D3DXToRadian(12.0f) / LAST_PUNCH_SET_FRAME);

		}
		else
		{
			g_Enemy[2].Pos += D3DXVECTOR3(10.0f, 0, 10.0f);
			g_Enemy[3].Pos += D3DXVECTOR3(0, 0, -10.0f);

			g_PunchFrameCnt = 0;
			//�t�F�[�Y�i�s
			g_Last_Punch_Phase = PUNCH_PHASE_PUNCH;
		}
	}
	else if (g_Last_Punch_Phase == PUNCH_PHASE_PUNCH)
	{
		if (g_PunchFrameCnt < LAST_PUNCH_SLOW_FRAME)
		{
			for (int i = 0; i < ENEMY_MODEL_NUM; i++)
			{
				g_Enemy[i].Pos += D3DXVECTOR3(0, 0.5f / LAST_PUNCH_PUNCH_FRAME, -60.0f / LAST_PUNCH_PUNCH_FRAME);
			}
			g_Enemy[3].Pos += D3DXVECTOR3(5.0f / LAST_PUNCH_PUNCH_FRAME, -8.0f / LAST_PUNCH_PUNCH_FRAME, -10.0f / LAST_PUNCH_PUNCH_FRAME);

		}
		else
		{
			g_PunchFrameCnt = 0;
			//�t�F�[�Y�i�s
			g_Last_Punch_Phase = PUNCH_PHASE_SLOW;
		}
	}
	else if (g_Last_Punch_Phase == PUNCH_PHASE_SLOW)
	{
		if (Keyboard_IsTrigger(DIK_RETURN))//��ŕς���
		{

			g_PunchFrameCnt = 0;
			//�t�F�[�Y�i�s
			g_Last_Punch_Phase = PUNCH_PHASE_STOP;
		}
		else if (Keyboard_IsTrigger(DIK_NUMPADENTER))//��ŕς���
		{

			g_PunchFrameCnt = 0;
			//�t�F�[�Y�i�s
			g_Last_Punch_Phase = PUNCH_PHASE_STOP;
		}
	}
	else if (g_Last_Punch_Phase == PUNCH_PHASE_STOP)
	{
		if (g_PunchFrameCnt < LAST_PUNCH_STOP_FRAME)
		{
			/*for (int i = 0; i < ENEMY_MODEL_NUM; i++)
			{
				g_Enemy[i].Pos += D3DXVECTOR3(0, 0.5f / LAST_PUNCH_STOP_FRAME, -60.0f / LAST_PUNCH_STOP_FRAME);
			}*/

			//�f�o�b�N�p
			if (g_PunchFrameCnt < LAST_PUNCH_PUNCH_FRAME)
			{
				for (int i = 0; i < ENEMY_MODEL_NUM; i++)
				{
					g_Enemy[i].Pos += D3DXVECTOR3(0, 0.5f / LAST_PUNCH_PUNCH_FRAME, -30.0f / LAST_PUNCH_PUNCH_FRAME);
				}
				g_Enemy[3].Rot.y += 15.0f / LAST_PUNCH_PUNCH_FRAME;
			}
		}
		else
		{


			g_PunchFrameCnt = 0;
			//�t�F�[�Y�i�s
			g_Last_Punch_Phase = PUNCH_PHASE_YOIN;
		}
	}
	else if (g_Last_Punch_Phase == PUNCH_PHASE_YOIN)
	{
		if (g_PunchFrameCnt < LAST_PUNCH_YOIN_FRAME)
		{


		}
		else
		{
			g_Enemy[1].Pos = D3DXVECTOR3(0.0f, 22.0f, -56.5f);
			g_Enemy[1].Rot = D3DXVECTOR3(D3DXToRadian(-31.2f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));
			g_Enemy[2].Pos = D3DXVECTOR3(15.0f, 16.7f, -56.2f);
			g_Enemy[2].Rot = D3DXVECTOR3(D3DXToRadian(83.0f), D3DXToRadian(145.0f), D3DXToRadian(90.0f));
			g_Enemy[3].Pos = D3DXVECTOR3(-6.6f, 12.2f, -65.0f);
			g_Enemy[3].Rot = D3DXVECTOR3(D3DXToRadian(70.7f), D3DXToRadian(272.2f), D3DXToRadian(8.3f));
			g_Enemy[4].Pos = D3DXVECTOR3(3.1f, -5.0f, -65.0f);
			g_Enemy[4].Rot = D3DXVECTOR3(D3DXToRadian(56.4f), D3DXToRadian(16.3f), D3DXToRadian(0.0f));
			g_Enemy[5].Pos = D3DXVECTOR3(-6.0f, -8.0f, -60.0f);
			g_Enemy[5].Rot = D3DXVECTOR3(D3DXToRadian(-23.0f), D3DXToRadian(-3.0f), D3DXToRadian(12.0f));
			g_Enemy[6].Pos = D3DXVECTOR3(6.0f, 12.3f, -54.0f);
			g_Enemy[6].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(5.0f), D3DXToRadian(0.0f));

			g_PunchFrameCnt = 0;
			//�t�F�[�Y�i�s
			g_Last_Punch_Phase = PUNCH_PHASE_FLYAWAY;
		}
	}
	else if (g_Last_Punch_Phase == PUNCH_PHASE_FLYAWAY)
	{
		if (g_PunchFrameCnt < LAST_PUNCH_FLYAWAY_FRAME)
		{
			ENEMY_FLYAWAY();
		}
		else
		{
			g_PunchFrameCnt = 0;
			g_Last_Punch_Phase = PUNCH_PHASE_INFLYING;
		}
	}
	else if (g_Last_Punch_Phase == PUNCH_PHASE_INFLYING)
	{
		//�������s��
		if (g_PunchFrameCnt < LAST_PUNCH_FLYING_FRAME)
		{
			ENEMY_FLYAWAY();
		}
		else
		{
			if (!GetSkyFlag())
			{
				//�V�䊄�ꂽ���̕`��ɂ��邽�߃t���O��true�ɕύX�i�P��̂݁j
				SetSkyFlag_ture();
			}
			g_PunchFrameCnt = 0;
			g_Last_Punch_Phase = PUNCH_PHASE_OUTFLYING;
		}
	}
	else if (g_Last_Punch_Phase == PUNCH_PHASE_OUTFLYING)
	{
		//���̊O�̏����͂�����
		if (g_PunchFrameCnt < LAST_PUNCH_OUTFLYING_FRAME)
		{
			//ENEMY_OUTFLYAWAY();
			
		}
		else
		{
			g_PunchFrameCnt = 0;
			g_Last_Punch_Phase = PUNCH_PHASE_CLASH;

		}
		AddScore(100);
	}
	else if (g_Last_Punch_Phase == PUNCH_PHASE_CLASH)
	{
		//�ڕW���ɂԂ������Ƃ��̏����͂��̒���
		if (g_PunchFrameCnt < LAST_PUNCH_HIT_FRAME)
		{
			
			//�����`�悳�ꂽ��Ԃ�
			if (GetMoonFlag())
			{
				
				//���������ɂ�����G�̍��W�����ɋ߂Â��悤�������i10.0f�͓��̕�������ƂƂ����j
				if (g_Enemy[1].Pos.y + 6.0f < GetMoonPos_y())
				{

					AddScore(100);
					for (int i = 1; i < 7; i++)
					{
						
						g_Enemy[i].Pos += D3DXVECTOR3(0.0f, 1.2f, 0.6f);
					}
				}
				//���ɏՓ˂�����
				else
				{
					//���U���g�V�[���ɑJ��
					if (!GetResultStart())
					{
						SetResultStart_true();
					}
				}
			}
		}
		else
		{
			g_PunchFrameCnt = 0;

		}
	}
}

void ENEMY_FLYAWAY()
{
	g_FLYAWAY_Cnt++;

	if (g_FLYAWAY_Cnt < FLYAWAY_MOVE1_FRAME)
	{
		for (int i = 1; i < 7; i++)
		{
			g_Enemy[i].Pos += D3DXVECTOR3(0.0f, 2.0f / g_FLYAWAY_Cnt, 3.0f / g_FLYAWAY_Cnt);
		}
	}
	else
	{
		//�V�䊄���܂Ŕ��
		if (g_Enemy[1].Pos.y < ROOF_Y + 100.0f)
		{
			for (int i = 1; i < 7; i++)
			{
				g_Enemy[i].Pos += D3DXVECTOR3(0.0f, 30.0f, 10.0f);
			}
		}
		else
		{
		}
	}
}
//�ǉ�����8==========================================

//�p���`��Ɍ��̈ʒu�ɖ߂�������
void CreatePunchEndVec()
{
	//�e�������Ƃɋ����o���ăt���[�����Ŋ���
	for (int i = 0; i < ENEMY_MODEL_NUM - 1; i++)
	{
		//����
		g_PunchEndVec[i].x = (g_Enemy[i].Pos.x - g_ModelStaPos[i].x) / g_EndFrameList[g_PunchPattern[g_PunchCnt]];
		g_PunchEndVec[i].y = (g_Enemy[i].Pos.y - g_ModelStaPos[i].y) / g_EndFrameList[g_PunchPattern[g_PunchCnt]];
		g_PunchEndVec[i].z = (g_Enemy[i].Pos.z - g_ModelStaPos[i].z) / g_EndFrameList[g_PunchPattern[g_PunchCnt]];

		//�p�x
		g_PunchEndRot[i].x = (g_Enemy[i].Rot.x - g_ModelStaRot[i].x) / g_EndFrameList[g_PunchPattern[g_PunchCnt]];
		g_PunchEndRot[i].y = (g_Enemy[i].Rot.y - g_ModelStaRot[i].y) / g_EndFrameList[g_PunchPattern[g_PunchCnt]];
		g_PunchEndRot[i].z = (g_Enemy[i].Rot.z - g_ModelStaRot[i].z) / g_EndFrameList[g_PunchPattern[g_PunchCnt]];
	}
}

//�G��Getter
XMODEL* GetEnemy()
{
	return g_Enemy;
}

//�p���`�����ۂ���Getter
bool GetPunch_Flg()
{
	return Punch_Flg;
}

//�p���`�t�F�[�Y��Getter
PUNCH_PHASE GetPunchPhase()
{
	return g_PunchPhase;
}

//�p���`�̍��EGetter
bool GetPunchLR()
{
	return PunchLR;
}

//�����Ă���p���`�̎�ނ�Getter
PUNCH_PATTERN_INDEX GetPunchIndex()
{
	return g_Punch_Pattern_Index;
}

//�ǉ�����9===============================
//�Ō�̃p���`��Getter
LAST_PUNCH_PHASE GetLastPunchPhase()
{
	return g_Last_Punch_Phase;
}
//�ǉ�

void ENEMY_OUTFLYAWAY()
{
	g_FLYAWAY_Cnt++;

	for (int i = 1; i < 7; i++)
	{
		g_Enemy[i].Pos += D3DXVECTOR3(0.0f, 15.0f, 5.0f);
	}

}
int Getg_PunchFrameCnt()
{
	return g_PunchFrameCnt;
}