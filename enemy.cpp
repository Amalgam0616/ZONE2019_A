
#include "myDirect3D.h"
#include "input.h"
#include "Xfile.h"
#include "enemy.h"
#include "debugproc.h"
#include "Effect.h"
#include <time.h>	//�����V�[�h���悤�Ȃ̂ŁA�����Ȃ炱���
#include "texture.h"
#include "player.h"

//�萔��` ==========================================================================

//�v���C���[�Ɏg�����f����
#define		ENEMY_MODEL_NUM	(7)

//�G�̏������WZ
#define ENEMY_POS_Z (26.0f)

//�A�b�p�[�Ő�����ԃX�s�[�h
#define UPPERED_SPEED (10.0f)

//�X���[�G���A���W
#define		SLOW_IN		(23.0f)
#define		SLOW_OUT	(15.0f)

//1�Q�[��������̃p���`��
#define		NUM_PUNCH		(10)

//�p���`�̎�ނ��Ƃ�START���珀�������܂ł̃t���[������ݒ�
#define		PPI_R_P_START_FRAME		(74)	//�E�ʏ�p���`
#define		PPI_L_P_START_FRAME		(74)	//���ʏ�p���`
#define		FLASH_P_START_FRAME		(74)	//�u�Ԉړ��p���`
#define		PPI_D_P_START_FRAME		(74)	//����o�[���Ă���p���`
#define		SPIN_P_START_FRAME		(12000)	//��]�p���`

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
//�����Ă���p���`�̎�ނ̊Ǘ�
PUNCH_PATTERN_INDEX g_Punch_Pattern_Index;
//�p���`�̃t���[���J�E���^
int g_PunchFrameCnt;
//�p���`�̃X�s�[�h
float Punch_Speed;
//�p���`�I��Z���W
float g_PunchEndLine;

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
	0,	//��]������
};

//�X���[�t���O(����)
bool SlowFlg;

//��{�֐���` ======================================================================

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

	//�p���`�p�^�[���쐬
	for (int i = 0; i < NUM_PUNCH; i++)
	{
		//g_PunchPattern[i] = rand() % (PPI_MAX - 1);
		g_PunchPattern[i] = SPIN_PUNCH_L;
	}

	//�p���`�����ۂ�
	Punch_Flg = false;
	//�p���`�X�s�[�h
	Punch_Speed = 3.0f;

	//�u�Ԉړ��̑��x�{��
	g_Slow_Multi = 1.0f;

	//�p���`�̃t�F�[�Y�Ǘ�
	g_PunchPhase = PUNCH_PHASE_CHARGE;
	//�����Ă���p���`�̎�ނ̊Ǘ�
	g_Punch_Pattern_Index = PUNCH_NULL;
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
				else if (g_PunchPattern[g_PunchCnt] == SPIN_PUNCH_L)
				{//���ɍU�������]�p���`
					//�����F���̂�X����]�ŃX���[���򂳂���\��
					//���邢�͐�p�̕ϐ��ŃJ�E���g����t���[����
				}
			}
		}

		//��������͂��ꂽ��X���[����
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
				else if (g_PunchPattern[g_PunchCnt] == SPIN_PUNCH_L)
				{//���ɍU�������]�p���`
					g_Punch_Pattern_Index = SPIN_PUNCH_L;
					SpinPunch_L();
				}
			}
		}
	}

	//�v���C���[�̃A�b�p�[�t�F�[�Y�ɍ��킹�Đ�����΂�
	if (GetUpper_Phase() == 1)
	{
		if (!g_Huttobi)
		{
			//������ё҂�
			EnemyUpperPos();

			g_Huttobi = true;

			//�A�j���[�V��������߂�
			isTaiki = false;
		}
	}
	else if (GetUpper_Phase() == 3)
	{
		//������΂�
		for (int i = 1; i < 7; i++)
		{
			g_Enemy[i].Pos += D3DXVECTOR3(0.0f, UPPERED_SPEED, UPPERED_SPEED * 0.7);
		}
	}


	//�f�o�b�O�p
	DebugProc_Print((char *)"�p���`�I�����C��Z���W�F[%f]\n", g_PunchEndLine);
	DebugProc_Print((char *)"g_Punch_Pattern_Index �F[%d]\n", g_Punch_Pattern_Index);
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
		R_FLASH_PUNCH_R();
	}

	if (Keyboard_IsPress(DIK_I))
	{
		EnemyPosReset();
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

	if (i > 5)
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

	if (GetUpper_Phase() != 3)
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
				Punch_Charge_Effect();
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
				Punch_Charge_Effect();
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
			Punch_Charge_Effect();
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
			Punch_Charge_Effect();
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
				Punch_Charge_Effect();
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
//���ɍU�������]�p���`(SPIN_PUNCH_L)(�쐬�ҁFAmalgam�ANo.04)
void SpinPunch_L()
{
	//�e��ϐ�
	static float radius;	//��]�̔��a

	//��]�p���`�p1�t���[��������̉�]�p�x(Dig)
	static float SpinDig;

	//�t���[���i�s
	g_PunchFrameCnt++;

	if (g_PunchPhase == PUNCH_PHASE_CHARGE)
	{//�\������
		if (g_PunchFrameCnt < SPIN_P_START_FRAME)
		{
			if (g_PunchFrameCnt == 1)
			{
				//���ꁫ�|������R�����g���Ƃ�
				//Punch_Charge_Effect();

				//����
				g_Enemy[2].Pos = D3DXVECTOR3(9.0f, 12.5f, 26.0f);
				g_Enemy[2].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));

				//�E��
				g_Enemy[3].Pos = D3DXVECTOR3(-9.0f, 12.5f, 26.0f);
				g_Enemy[3].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));

				radius = 9.0f;
				SpinDig = 2.0f;

				//PunchLR = true;
			}

			//����L����
			if (g_PunchFrameCnt < 50)
			{
				g_Enemy[2].Pos.x += 4.0f / 50;
				g_Enemy[3].Pos.x += -4.0f / 50;

				radius += 4.0f / 50;
				SpinDig += 20.0f / 50;
			}

			//������]
			float tmp = SpinDig * g_PunchFrameCnt;

			//����
			g_Enemy[2].Pos.x = ((cos(D3DXToRadian(tmp)) + sin(D3DXToRadian(tmp))) * radius) - (0.0f - g_Enemy[0].Pos.x);
			g_Enemy[2].Pos.z = ((-sin(D3DXToRadian(tmp)) + cos(D3DXToRadian(tmp))) * radius) - (0.0f - g_Enemy[0].Pos.z);
			//�E��
			g_Enemy[3].Pos.x = -((cos(D3DXToRadian(tmp)) + sin(D3DXToRadian(tmp))) * radius) - (0.0f - g_Enemy[0].Pos.x);
			g_Enemy[3].Pos.z = -((-sin(D3DXToRadian(tmp)) + cos(D3DXToRadian(tmp))) * radius) - (0.0f - g_Enemy[0].Pos.z);
		}
		else
		{
			//�v���C���[�̓��ւ̃x�N�g�����o��

			//�t�F�[�Y�i�s
			//g_PunchPhase = PUNCH_PHASE_SWING;
		}
	}
	else if (g_PunchPhase == PUNCH_PHASE_SWING)
	{//�p���`�{��
		if (g_Enemy[3].Pos.z >= g_PunchEndLine)
		{//�G���h���C���ɓ��B���ĂȂ���Γ�����(�v���C���[�͓G����݂�Z�}�C�i�X�����ɂ���)
			if (SlowFlg)
			{//�X���[�G���A���������瑬�x�ɉ����␳
			}
			else
			{
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