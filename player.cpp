
#include "myDirect3D.h"
#include "input.h"
#include "Xfile.h"
#include "player.h"
#include "texture.h"
#include "sprite.h"
#include "enemy.h"
#include "Effect.h"
#include "score.h"
#include "sound.h"
#include "gauge.h"
#include "Title.h"

//�萔��` ==========================================================================

//�v���C���[�Ɏg�����f����
#define		PLAYER_MODEL_NUM	(6)

//����֘A
//��𓮂�����
#define		MOVE_FRAME_MAX		(5)
//����I��
#define		MOVE_FRAME_END		(60)
//���E�킩��₷��������
#define		LEFT				(true)
#define		RIGHT				(false)

//�O���[�o���ϐ���` =================================================================

//�v���C���[��XMODEL�錾������
//0:���@1:���@2:����@3:�E��@4:�����@5:�E��
XMODEL g_Player[PLAYER_MODEL_NUM] = {};

//���̊p�x(�ʍ�)
float g_Leg_Rotate;
static bool Leg_Flg;
static bool Step_Stop;
static bool g_Once_AddScore_Flg;

//�ǉ�����1==========================================
//Finish_Punch�p�錾
bool g_Finish_Flg;
int g_Finish_Cnt;
//�ǉ�����1==========================================

//�_���[�W�t���O
bool damage_flag;

//���[���h���W
static D3DXMATRIXA16 g_mtxWorld;

//����֘A
//����t���O
static bool g_DodgeFlg;
//���E�t���O (true:L false:R)
static bool g_LrFlg;

//�X�R�A��ǉ�����t���O
static bool g_AddScoreFlg;

//�X�R�A�����炷�t���O
static bool g_Reduce_ScoreFlg;

//�A�j���[�V�����̃��X�g
static PL_ANIME g_AnimeList[ANIME_INDEX_MAX] =
{
	//ANIME_INDEX_MORMAL�@�ҋ@���[�V����
	{ 10, 10, D3DXVECTOR3(0.0f, D3DXToRadian(0.0f), 0.0f),D3DXVECTOR3(0.0f, 1.0f, 0.0f)},
	//ANIME_INDEX_DODGE_L�@��������[�V�����@����ɖ߂郂�[�h�̎���2�ڂ̗v�f��25���炢�ɂ���
	{ 6, 10, D3DXVECTOR3(0.0f, 0.0f, D3DXToRadian(10.0f)), D3DXVECTOR3(7.0f, -1.8f, 0.0f)},
	//ANIME_INDEX_DODGE_R�@�E������[�V�����@����ɖ߂郂�[�h�̎���2�ڂ̗v�f��25���炢�ɂ���
	{ 6, 10, D3DXVECTOR3(0.0f, 0.0f, D3DXToRadian(-10.0f)), D3DXVECTOR3(-7.0f, -1.8f, 0.0f)},
	//ANIME_INDEX_DAMAGE�@�_���[�W���[�V����
	{ 3, 25, D3DXVECTOR3(D3DXToRadian(30.0f), 0.0f, 0.0f), D3DXVECTOR3(0.0f, -1.0f, -2.5f)},
};

//�A�j���[�V�����̃t���[�����J�E���g
static int g_AnimFrameCnt;
//��u�O�̃A�j���[�V�����J�E���^
static int OldFrame;
//�A�j���[�V�����~�߂�t���O
static bool AnimStopFlg;

//�ҋ@��Ԃ��ۂ� �ҋ@�Ffalse ����ȊO�Ftrue
static bool isAction;

//�v���C���[�����s���Ă���A�j���X�e�[�g
static PLAYER_ANIME_INDEX g_PlAnimState;

//1�t���[��������̊p�x�ω���
static D3DXVECTOR3 g_AnglePerFrame;

//1�t���[��������̍��W�ω���
static D3DXVECTOR3 g_MovePerFrame;

void DamageEffect();
//��{�֐���` ======================================================================

//������
HRESULT InitPlayer()
{
	//�ǂݍ��݁E�����l�ݒ�
	{
		//XFile����3D���f�������[�h
		if (FAILED(LoadXFile(&g_Player[0], XFILE_INDEX_P_BODY_001)) ||		//����
			FAILED(LoadXFile(&g_Player[1], XFILE_INDEX_P_HEAD_001)) ||		//��
			FAILED(LoadXFile(&g_Player[2], XFILE_INDEX_P_GROBE_L_001)) ||	//����(�O����݂ĉE)
			FAILED(LoadXFile(&g_Player[3], XFILE_INDEX_P_GROBE_R_001)) ||	//�E��(�O����݂č�)
			FAILED(LoadXFile(&g_Player[4], XFILE_INDEX_P_LEG_L_001)) ||		//����(�O����݂ĉE)
			FAILED(LoadXFile(&g_Player[5], XFILE_INDEX_P_LEG_R_001)))		//�E��(�O����݂č�)
		{
			//���s�����炱��
			return E_FAIL;
		}

		PlayerPosReset();
	}

	//�e��ϐ�������
	g_Leg_Rotate = 0.0f;
	Leg_Flg = false;
	g_Once_AddScore_Flg = false;

	g_DodgeFlg = false;
	g_LrFlg = false;
	g_AddScoreFlg = false;
	g_Reduce_ScoreFlg = false;

	Step_Stop = false;

	//�A�j���[�V�����֘A
	g_AnimFrameCnt = 0;
	g_PlAnimState = PLANIME_INDEX_NORMAL;
	isAction = false;
	g_AnglePerFrame = {};
	g_MovePerFrame = {};
	OldFrame = g_AnimFrameCnt;
	AnimStopFlg = false;

	//�ǉ�����2==========================================
	//Finish�֘A
	g_Finish_Flg = false;
	g_Finish_Cnt = 0;
	//�ǉ�����2==========================================

	damage_flag = false;
	return S_OK;
}
//�I��
void UninitPlayer()
{
	//���b�V���̉��
	for (int i = 0; i < PLAYER_MODEL_NUM; i++)
	{
		SAFE_RELEASE(g_Player[i].pMesh);
	}
}

//�X�V
void UpdatePlayer()
{
	//�ǉ�����3==========================================
	//���ꂽ�Ԃ�f�o�b�N�p��������R�����g�A�E�g
	/*if (Keyboard_IsTrigger(DIK_NUMPADENTER))
	{
		Step_Stop = true;
	}*/


	if (GetPunchIndex() == LAST_PUNCH)
	{
		Step_Stop = true;

		if (Keyboard_IsTrigger(DIK_M))
		{
			PlayerPosReset();
		}
	}
	//�ǉ�����3==========================================

	if (!Step_Stop)
	{
		//�A�j���[�V�����i�s
		Animation();
	}

	//�ҋ@���[�V�����ȊO�̃A�j���[�V�������s���Ă��銎�A�G���p���`���łȂ��ꍇ�̓L�[���͂��s���Ȃ�
	if (!isAction)
	{
		if (GetPunch_Flg())
		{
			//�L�[���͂ɉ����ăX�e�[�g�ω�
			//�ł����G���^�[�ł��Ƃ�
			if (GetBigEnter() == true) {
				if (!isAction && Keyboard_IsRelease(DIK_D) && !g_DodgeFlg)
				{//�E���
					g_PlAnimState = PLANIME_INDEX_DODGE_L;
					//�A�j���[�V�����t���[��������
					g_AnimFrameCnt = 0;
					//�A�N�V�������ɂ���
					isAction = true;

					//�������Ƀt���O�𗧂Ă�
					g_LrFlg = false;
					//�X�R�A��ǉ�����t���O�𗧂Ă�
					g_AddScoreFlg = true;

					//�X�R�A�𑝂₷
					if (g_AddScoreFlg == true && g_Once_AddScore_Flg == false && (GetPunchIndex() == PPI_RIGHT_PUNCH))
					{
						AddScore(30000 * CoefCal(true));
						AddGauge(13 - (int)CoefCal(false));
						//��x�̂݃X�R�A��������t���O�𐳂ɂ���
						g_Once_AddScore_Flg = true;
						//����t���O�𐳂ɂ���
						g_DodgeFlg = true;
					}

					if (g_AddScoreFlg == true && g_Once_AddScore_Flg == false && (GetPunchIndex() == PPI_R_JUMP_PUNCH))
					{
						AddScore(60000 * CoefCal(true));
						AddGauge(15 - (int)CoefCal(false));
						//��x�̂݃X�R�A��������t���O�𐳂ɂ���
						g_Once_AddScore_Flg = true;
						//����t���O�𐳂ɂ���
						g_DodgeFlg = true;
					}

					if (g_Once_AddScore_Flg == false && GetPunchIndex() == R_FLASH_PUNCH)
					{
						AddScore(90000 * CoefCal(true));
						AddGauge(17 - (int)CoefCal(false));
						//��x�̂݃X�R�A��������t���O�𐳂ɂ���
						g_Once_AddScore_Flg = true;
						//����t���O�𐳂ɂ���
						g_DodgeFlg = true;
					}

					if (g_Once_AddScore_Flg == false && GetPunchIndex() == PPI_DUNK_PUNCH)
					{
						//����t���O�𐳂ɂ���
						g_DodgeFlg = true;
					}
				}
				else if (!isAction && Keyboard_IsRelease(DIK_A) && !g_DodgeFlg)
				{//�����
					//�X�e�[�g�ω�
					g_PlAnimState = PLANIME_INDEX_DODGE_R;
					//�A�j���[�V�����t���[��������
					g_AnimFrameCnt = 0;
					//�A�N�V�������ɂ���
					isAction = true;

					//�������Ƀt���O�𗧂Ă�
					g_LrFlg = true;
					//�X�R�A��ǉ�����t���O�𗧂Ă�
					g_AddScoreFlg = true;

					//�X�R�A�𑝂₷
					if (g_AddScoreFlg == true && g_Once_AddScore_Flg == false && (GetPunchIndex() == PPI_LEFT_PUNCH))
					{
						AddScore(30000 * CoefCal(true));
						AddGauge(13 - (int)CoefCal(false));
						//��x�̂݃X�R�A��������t���O�𐳂ɂ���
						g_Once_AddScore_Flg = true;
						//����t���O�𐳂ɂ���
						g_DodgeFlg = true;
					}

					if (g_AddScoreFlg == true && g_Once_AddScore_Flg == false && (GetPunchIndex() == PPI_L_JUMP_PUNCH))
					{
						AddScore(60000 * CoefCal(true));
						AddGauge(15 - (int)CoefCal(false));
						//��x�̂݃X�R�A��������t���O�𐳂ɂ���
						g_Once_AddScore_Flg = true;
						//����t���O�𐳂ɂ���
						g_DodgeFlg = true;
					}

					if (g_Once_AddScore_Flg == false && GetPunchIndex() == L_FLASH_PUNCH)
					{
						AddScore(90000 * CoefCal(true));
						AddGauge(17 - (int)CoefCal(false));
						//��x�̂݃X�R�A��������t���O�𐳂ɂ���
						g_Once_AddScore_Flg = true;
						//����t���O�𐳂ɂ���
						g_DodgeFlg = true;
					}

					if (g_Once_AddScore_Flg == false && GetPunchIndex() == PPI_DUNK_PUNCH)
					{
						//����t���O�𐳂ɂ���
						g_DodgeFlg = true;
					}
				}
			}
			//�L�[�{�[�h�ł��Ƃ�
			else if (GetBigEnter() == false) {
				if (!isAction && Keyboard_IsTrigger(DIK_D) && !g_DodgeFlg)
				{//�E���
					g_PlAnimState = PLANIME_INDEX_DODGE_L;
					//�A�j���[�V�����t���[��������
					g_AnimFrameCnt = 0;
					//�A�N�V�������ɂ���
					isAction = true;

					//�������Ƀt���O�𗧂Ă�
					g_LrFlg = false;
					//�X�R�A��ǉ�����t���O�𗧂Ă�
					g_AddScoreFlg = true;

					//�X�R�A�𑝂₷
					if (g_AddScoreFlg == true && g_Once_AddScore_Flg == false && (GetPunchIndex() == PPI_RIGHT_PUNCH))
					{
						AddScore(30000 * CoefCal(true));
						AddGauge(13 - (int)CoefCal(false));
						//��x�̂݃X�R�A��������t���O�𐳂ɂ���
						g_Once_AddScore_Flg = true;
						//����t���O�𐳂ɂ���
						g_DodgeFlg = true;
					}

					if (g_AddScoreFlg == true && g_Once_AddScore_Flg == false && (GetPunchIndex() == PPI_R_JUMP_PUNCH))
					{
						AddScore(60000 * CoefCal(true));
						AddGauge(15 - (int)CoefCal(false));
						//��x�̂݃X�R�A��������t���O�𐳂ɂ���
						g_Once_AddScore_Flg = true;
						//����t���O�𐳂ɂ���
						g_DodgeFlg = true;
					}

					if (g_Once_AddScore_Flg == false && GetPunchIndex() == R_FLASH_PUNCH)
					{
						AddScore(90000 * CoefCal(true));
						AddGauge(17 - (int)CoefCal(false));
						//��x�̂݃X�R�A��������t���O�𐳂ɂ���
						g_Once_AddScore_Flg = true;
						//����t���O�𐳂ɂ���
						g_DodgeFlg = true;
					}

					if (g_Once_AddScore_Flg == false && GetPunchIndex() == PPI_DUNK_PUNCH)
					{
						//����t���O�𐳂ɂ���
						g_DodgeFlg = true;
					}
				}
				else if (!isAction && Keyboard_IsTrigger(DIK_A) && !g_DodgeFlg)
				{//�����
					//�X�e�[�g�ω�
					g_PlAnimState = PLANIME_INDEX_DODGE_R;
					//�A�j���[�V�����t���[��������
					g_AnimFrameCnt = 0;
					//�A�N�V�������ɂ���
					isAction = true;

					//�������Ƀt���O�𗧂Ă�
					g_LrFlg = true;
					//�X�R�A��ǉ�����t���O�𗧂Ă�
					g_AddScoreFlg = true;

					//�X�R�A�𑝂₷
					if (g_AddScoreFlg == true && g_Once_AddScore_Flg == false && (GetPunchIndex() == PPI_LEFT_PUNCH))
					{
						AddScore(30000 * CoefCal(true));
						AddGauge(13 - (int)CoefCal(false));
						//��x�̂݃X�R�A��������t���O�𐳂ɂ���
						g_Once_AddScore_Flg = true;
						//����t���O�𐳂ɂ���
						g_DodgeFlg = true;
					}

					if (g_AddScoreFlg == true && g_Once_AddScore_Flg == false && (GetPunchIndex() == PPI_L_JUMP_PUNCH))
					{
						AddScore(60000 * CoefCal(true));
						AddGauge(15 - (int)CoefCal(false));
						//��x�̂݃X�R�A��������t���O�𐳂ɂ���
						g_Once_AddScore_Flg = true;
						//����t���O�𐳂ɂ���
						g_DodgeFlg = true;
					}

					if (g_Once_AddScore_Flg == false && GetPunchIndex() == L_FLASH_PUNCH)
					{
						AddScore(90000 * CoefCal(true));
						AddGauge(17 - (int)CoefCal(false));
						//��x�̂݃X�R�A��������t���O�𐳂ɂ���
						g_Once_AddScore_Flg = true;
						//����t���O�𐳂ɂ���
						g_DodgeFlg = true;
					}

					if (g_Once_AddScore_Flg == false && GetPunchIndex() == PPI_DUNK_PUNCH)
					{
						//����t���O�𐳂ɂ���
						g_DodgeFlg = true;
					}
				}
			}
		}
	}

	//�_���[�W���[�V��������
	//����̃p���`����Ȃ��ꍇ
	if (GetPunchIndex() != PPI_DUNK_PUNCH)
	{
		if (GetPunchPhase() == PUNCH_PHASE_RETURN && !g_DodgeFlg && g_PlAnimState != PLANIME_INDEX_DAMAGE)
		{
			g_PlAnimState = PLANIME_INDEX_DAMAGE;
			//�A�j���[�V�����t���[��������
			g_AnimFrameCnt = 0;
			//�A�N�V�������ɂ���
			isAction = true;
			//�A�j���[�V�����X�g�b�v����������
			AnimStopFlg = false;
		}
	}
	else	//����̃p���`�̏ꍇ
	{
		if (GetPunchPhase() == PUNCH_PHASE_RETURN && g_DodgeFlg && g_PlAnimState != PLANIME_INDEX_DAMAGE)
		{
			g_PlAnimState = PLANIME_INDEX_DAMAGE;
			//�A�j���[�V�����t���[��������
			g_AnimFrameCnt = 0;
			//�A�N�V�������ɂ���
			isAction = true;
			//�A�j���[�V�����X�g�b�v����������
			AnimStopFlg = false;
		}
		else if (GetPunchPhase() == PUNCH_PHASE_RETURN && g_Once_AddScore_Flg == false && !g_DodgeFlg)
		{
			AddGauge(13);
			AddScore(60000);
			//��x�̂݃X�R�A��������t���O�𐳂ɂ���
			g_Once_AddScore_Flg = true;
		}
	}

	//����{�^�����ǂ�����������Ă�����A�j���[�V�����X�g�b�v����������
	//�f�o�b�O�̎��́u������Ă��Ȃ��Ƃ��v�ɂ��Ƃ���
	//�f�J�G���^�[�̎�
	if (GetBigEnter() == true) {
		if ((AnimStopFlg && Keyboard_IsPress(DIK_A) && Keyboard_IsPress(DIK_D)))
		{
			AnimStopFlg = false;
		}
	}
	//�L�[�{�[�h�̂Ƃ�
	if (GetBigEnter() == false) {
		if ((AnimStopFlg && !Keyboard_IsPress(DIK_A) && !Keyboard_IsPress(DIK_D)))
		{
			AnimStopFlg = false;
		}
	}

	//�A�j�����X�g�b�v����Ă��Ȃ�������t���[���i�s
	if (!AnimStopFlg)
	{
		g_AnimFrameCnt++;	//�����Ł@1�@�ȏ�ɂȂ�
	}

	//�f�o�b�O(�Ώ̎����p)
	//AnimStopFlg = false;

	//�X�R�A�̍Ď擾���\�ɂ���
	if (g_Once_AddScore_Flg && !GetPunch_Flg())
	{
		g_Once_AddScore_Flg = false;
	}

	//�ǉ�����4==========================================
	//FinishPunch���łĂ�悤�ɂȂ�����ʂ�悤�ɂȂ�FENTER��FinishPunch�𔭓��\�ɂ���
	if (GetLastPunchPhase() == PUNCH_PHASE_SLOW)
	{
		if (Keyboard_IsTrigger(DIK_RETURN))
		{
			g_Finish_Flg = true;
			Finish_Punch_Pos();
		}
		else if (Keyboard_IsTrigger(DIK_NUMPADENTER))
		{
			g_Finish_Flg = true;
			Finish_Punch_Pos();
		}
	}


	//g_Finish_FLg�����ɂȂ����Ƃ��ʂ��FinishPunch������
	if (GetLastPunchPhase() == PUNCH_PHASE_STOP)
	{
		if (g_Finish_Flg)
		{
			Finish_Punch();
		}
	}
	//�ǉ�����4==========================================

}
//�`��
void DrawPlayer()
{
	//Direct3DDevice�̎擾
	LPDIRECT3DDEVICE9 p_D3DDevice = GetD3DDevice();

	//���[���h�g�����X�t�H�[��(��΍��W�ϊ�)
	D3DXMatrixIdentity(&g_mtxWorld);

	//���f�����J��Ԃ�
	for (int j = 5; j >= 0; j--)
	{
		//���[���h�}�g���N�X����ēK�p
		p_D3DDevice->SetTransform(D3DTS_WORLD, MakeWorldMatrix(&g_Player[j], &g_mtxWorld));

		//�}�e���A���̐������J��Ԃ�
		for (DWORD i = 0; i < g_Player[j].dwNumMaterials; i++)
		{
			//�}�e���A���Z�b�g
			p_D3DDevice->SetMaterial(&g_Player[j].pMaterials[i]);
			//�e�N�X�`���Z�b�g
			p_D3DDevice->SetTexture(0, g_Player[j].pTextures[i]);
			//���ߓx�ݒ�
			//������t�F�[�Y���O���[�u���������ɂ��Ȃ�
			if (GetLastPunchPhase() < PUNCH_PHASE_SLOW)
			{
				if (j == 2 || j == 3)
				{
					g_Player[j].pMaterials[i].Diffuse.a = 1.0f;
				}
				else
				{
					g_Player[j].pMaterials[i].Diffuse.a = 0.3f;
				}
			}

			if (GetLastPunchPhase() >= PUNCH_PHASE_STOP)
			{
				g_Player[j].pMaterials[i].Diffuse.a = 1.0f;
			}
			//������`��
			g_Player[j].pMesh->DrawSubset(i);
		}
	}


	//�_���[�W���󂯂��Ƃ��̂��
	if (damage_flag)
	{
		DamageEffect();
	}
	//���߂�I�̃e�N�X�`��
	if (GetLastPunchPhase() == PUNCH_PHASE_SLOW) {
		Sprite_Draw
		(
			TEXTURE_INDEX_PUSHUPPER,
			SCREEN_HEIGHT / 2 + 350,
			SCREEN_WIDTH / 2 - 50,
			0,
			0,
			800,
			800,
			800 / 2,
			800 / 2,
			0.5f,
			0.5f,
			0.0f
		);
	}
}

//���̑������Ȋ֐���` =============================================================

//������A�j���[�V����������֐�
void Animation()
{
	//g_AnimFrameCnt �� OldFrame �Ɠ���(���Z����Ă��Ȃ�)�ꍇ�̓A�j���[�V�������Ȃ�
	if (g_AnimFrameCnt != OldFrame)
	{
		//g_AnimFrame�@���@0�@�ȏ㊎�@StrFrame�@�ȉ��������ꍇ�́A�A�j���X�e�[�g���ɓ��������ʂ����߂ĂP�t���[����������
		if ((g_AnimFrameCnt >= 0 + 1) && (g_AnimFrameCnt <= g_AnimeList[g_PlAnimState].StrFrame + 1))
		{
			//g_AnimFrameCnt�@���@0�@�������ꍇ�́A���f���̍��W�E��]�������ʒu�ɖ߂��āA�A�j���X�e�[�g���ɂP�t���[��������̓������v�Z
			if (g_AnimFrameCnt == 0 + 1)
			{
				PlayerPosReset();
				Leg_Flg = true;

				//1�t���[��������̊p�x
				g_AnglePerFrame = D3DXVECTOR3(g_AnimeList[g_PlAnimState].Angle.x / g_AnimeList[g_PlAnimState].StrFrame,
					g_AnimeList[g_PlAnimState].Angle.y / g_AnimeList[g_PlAnimState].StrFrame,
					g_AnimeList[g_PlAnimState].Angle.z / g_AnimeList[g_PlAnimState].StrFrame);

				//1�t���[��������̈ړ�
				g_MovePerFrame = D3DXVECTOR3(g_AnimeList[g_PlAnimState].Move.x / g_AnimeList[g_PlAnimState].StrFrame,
					g_AnimeList[g_PlAnimState].Move.y / g_AnimeList[g_PlAnimState].StrFrame,
					g_AnimeList[g_PlAnimState].Move.z / g_AnimeList[g_PlAnimState].StrFrame);
			}

			//�A�j���X�e�[�g���̓���
			AnimMovingParFrame();
		}
		//g_AnimFrameCnt�@���@StrFrame + 1�@�ȏ㊎�@StrFrame + EndFrame�@�ȉ��������ꍇ�́A�A�j���X�e�[�g���ɓ��������ʂ����߂ĂP�t���[����������
		else if ((g_AnimFrameCnt >= g_AnimeList[g_PlAnimState].StrFrame + 2) && (g_AnimFrameCnt <= (g_AnimeList[g_PlAnimState].StrFrame + g_AnimeList[g_PlAnimState].EndFrame + 1)))
		{
			//g_AnimFrameCnt�@���@StrFrame�@���@1�@�傫���ꍇ�́A1�t���[��������̈ړ�������߂�p�ɍČv�Z����
			if (g_AnimFrameCnt == (g_AnimeList[g_PlAnimState].StrFrame + 2))
			{
				Leg_Flg = false;
				//�p�x�@�S�̂̊p�x�ɖ߂��āA�A��̃t���[�����Ŋ����āA���]
				g_AnglePerFrame = D3DXVECTOR3((g_AnimeList[g_PlAnimState].Angle.x / g_AnimeList[g_PlAnimState].EndFrame) * -1.0f,
					(g_AnimeList[g_PlAnimState].Angle.y / g_AnimeList[g_PlAnimState].EndFrame) * -1.0f,
					(g_AnimeList[g_PlAnimState].Angle.z / g_AnimeList[g_PlAnimState].EndFrame) * -1.0f);

				//�ړ��ʁ@�S�̂̈ړ��ʂɖ߂��āA�A��̃t���[�����Ŋ����āA���]
				g_MovePerFrame = D3DXVECTOR3((g_AnimeList[g_PlAnimState].Move.x / g_AnimeList[g_PlAnimState].EndFrame) * -1.0f,
					(g_AnimeList[g_PlAnimState].Move.y / g_AnimeList[g_PlAnimState].EndFrame) * -1.0f,
					(g_AnimeList[g_PlAnimState].Move.z / g_AnimeList[g_PlAnimState].EndFrame) * -1.0f);

				//��𒆊��ǂ�������������Ă��Ȃ���Ԃ�������A�j���[�V�������X�g�b�v����
				if ((g_PlAnimState == PLANIME_INDEX_DODGE_L) || (g_PlAnimState == PLANIME_INDEX_DODGE_R))
				{
					//�f�o�b�O�̎��́I���Ȃ��ŁA�t�b�g�y�_������Ƃ��́I���āB
					if (!Keyboard_IsPress(DIK_D) || !Keyboard_IsPress(DIK_A))
					{
						AnimStopFlg = true;
					}
				}
			}

			//�A�j���X�e�[�g���̓���
			AnimMovingParFrame();
		}
		//g_AnimFrameCnt�@���@StrFrame + EndFrame + 1�@�ȏゾ�����ꍇ�́A�A�j���[�V�������I�����đҋ@���[�V�����ɖ߂�
		else if (g_AnimFrameCnt >= (g_AnimeList[g_PlAnimState].StrFrame + g_AnimeList[g_PlAnimState].EndFrame + 2))
		{

			//g_PlAnimState��ҋ@���[�V�����ɕς���
			g_PlAnimState = PLANIME_INDEX_NORMAL;
			//g_AnimFrameCnt��0�ɖ߂�
			g_AnimFrameCnt = 0;
			//�t���[��������̈ړ���]��0�ɖ߂�
			g_AnglePerFrame = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			g_MovePerFrame = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

			//isAction��true��������false�ɖ߂�
			if (isAction)
			{
				isAction = false;
			}

			//����Ƃ����߂�
			g_DodgeFlg = false;
			g_LrFlg = false;
		}

		//OldFrame�𓯊�����
		OldFrame = g_AnimFrameCnt;
	}
}

//�A�j���X�e�[�g���̓���(����폜���邩������Ȃ�)
void AnimMovingParFrame()
{
	switch (g_PlAnimState)
	{
	case PLANIME_INDEX_NORMAL:
		//�ҋ@���[�V�����@���A���A���r�@�𓮂���
		for (int i = 0; i < PLAYER_MODEL_NUM - 2; i++)
		{
			damage_flag = false;
			//���W�ړ�
			g_Player[i].Pos += g_MovePerFrame;
			//�p�x�ω�
			g_Player[i].Rot += g_AnglePerFrame;
		}

		break;
	case PLANIME_INDEX_DODGE_R:

		//��������[�V�����@���A���A���r�@�𓮂���
		//���W�ړ�
		g_Player[0].Pos += g_MovePerFrame;
		g_Player[1].Pos += g_MovePerFrame * 1.25f;
		g_Player[2].Pos += g_MovePerFrame * 1.2f;
		g_Player[3].Pos += g_MovePerFrame * 1.0f;
		g_Player[4].Pos.x += g_MovePerFrame.x / 2.0f;
		g_Player[5].Pos.x += g_MovePerFrame.x / 2.0f;
		//�p�x�ω�
		g_Player[0].Rot += g_AnglePerFrame;
		g_Player[1].Rot += g_AnglePerFrame * 2.0f;
		g_Player[2].Rot -= g_AnglePerFrame * 1.2f;
		g_Player[3].Rot += g_AnglePerFrame;
		break;
	case PLANIME_INDEX_DODGE_L:

		//�E������[�V�����@���A���A���r�@�𓮂���
		//���W�ړ�
		g_Player[0].Pos += g_MovePerFrame;
		g_Player[1].Pos += g_MovePerFrame * 1.3f;
		g_Player[2].Pos += g_MovePerFrame * 1.05f;
		g_Player[3].Pos += g_MovePerFrame * 1.25f;
		g_Player[4].Pos.x += g_MovePerFrame.x / 2.05f;
		g_Player[5].Pos.x += g_MovePerFrame.x / 2.05f;
		//�p�x�ω�
		g_Player[0].Rot += g_AnglePerFrame;
		g_Player[1].Rot += g_AnglePerFrame * 2.0f;
		g_Player[2].Rot += g_AnglePerFrame;
		g_Player[3].Rot -= g_AnglePerFrame * 1.2f;
		break;
	case PLANIME_INDEX_DAMAGE:
		damage_flag = true;
		//�_���[�W���[�V�����@���A���A���r�@�𓮂���
		//���W�ړ�
		g_Player[0].Pos += g_MovePerFrame;
		g_Player[1].Pos += g_MovePerFrame * 2.0f;
		g_Player[2].Pos += g_MovePerFrame;
		g_Player[3].Pos += g_MovePerFrame;
		//�p�x�ω�
		g_Player[0].Rot += g_AnglePerFrame;
		g_Player[1].Rot += g_AnglePerFrame;
		g_Player[2].Rot += g_AnglePerFrame;
		g_Player[3].Rot += g_AnglePerFrame;
		break;
	default:
		break;
	}
}

//�ǉ�����5==========================================
//FinshPunch�̃|�W�V�����Ɗ֐�
void Finish_Punch_Pos()
{
	g_Player[0].Pos += D3DXVECTOR3(0, 0, 0);
	g_Player[0].Rot += D3DXVECTOR3(D3DXToRadian(-20.0f), 0, 0);
	g_Player[1].Pos += D3DXVECTOR3(0, 0, 3.0f);
	g_Player[1].Rot += D3DXVECTOR3(D3DXToRadian(-30.0f), 0, 0);
	g_Player[2].Pos += D3DXVECTOR3(0, 0, 6.0f);
	g_Player[2].Rot += D3DXVECTOR3(D3DXToRadian(-80.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));
	g_Player[3].Pos += D3DXVECTOR3(0, 0, 0);
	g_Player[3].Rot += D3DXVECTOR3(0, 0, 0);
	g_Player[4].Pos += D3DXVECTOR3(0, 0, 5.0f);
	g_Player[4].Rot += D3DXVECTOR3(0, 0, 0);
	g_Player[5].Pos += D3DXVECTOR3(0, 0, -5.0f);
	g_Player[5].Rot += D3DXVECTOR3(D3DXToRadian(-10.0f), 0, 0);
}

void Finish_Punch()
{
	g_Finish_Cnt++;
	for (int i = 0; i < PLAYER_MODEL_NUM; i++)
	{
		g_Player[i].Pos.z += 160.0f / (g_Finish_Cnt * 4);
	}
}
//�ǉ�����5==========================================

//����t���O��Getter
bool GetDodgeFlg()
{
	return g_DodgeFlg;
}

//���E�t���O��Getter
bool GetLrFlg()
{
	return g_LrFlg;
}

void PlayerPosReset()
{

	//���W�E��]�E�g�k�@�����l�ݒ�
	//����
	g_Player[0].Pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Player[0].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(180.0f), D3DXToRadian(0.0f));
	g_Player[0].Scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	//��
	g_Player[1].Pos = D3DXVECTOR3(0.0f, 8.0f, 0.0f);
	g_Player[1].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(180.0f), D3DXToRadian(0.0f));
	g_Player[1].Scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	//���r
	g_Player[2].Pos = D3DXVECTOR3(-3.5f, 4.0f, 3.0f);
	g_Player[2].Rot = D3DXVECTOR3(D3DXToRadian(165.0f), D3DXToRadian(180.0f), D3DXToRadian(255.0f));
	g_Player[2].Scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	//�E�r
	g_Player[3].Pos = D3DXVECTOR3(3.5f, 2.5f, 3.0f);
	g_Player[3].Rot = D3DXVECTOR3(D3DXToRadian(165.0f), D3DXToRadian(180.0f), D3DXToRadian(105.0f));
	g_Player[3].Scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	//����
	g_Player[4].Pos = D3DXVECTOR3(-2.5f, -7.0f, 0.0f);
	g_Player[4].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(180.0f), D3DXToRadian(0.0f));
	g_Player[4].Scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	//�E��
	g_Player[5].Pos = D3DXVECTOR3(2.5f, -7.0f, 0.0f);
	g_Player[5].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(180.0f), D3DXToRadian(0.0f));
	g_Player[5].Scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
}

//�v���C���[��Getter
XMODEL* GetPlayer()
{
	return g_Player;
}
