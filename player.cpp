
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

//�A�b�p�[�p�錾
static int Upper_Frame;
static int Upper_Phase;

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

//�A�b�p�[�֘A
static float Body_Move1;
static float Body_Move2;
static float Head_Move1;
static float Grobe_Grobe_L;
static float Grobe_Grobe_R;
static float Rotate_Z_R2;
static float Rotate_Z_R3;
static float Leg_L_Move;
static float Leg_R_Move;


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

	//�A�b�p�[�֘A
	Upper_Frame = 0;
	Upper_Phase = -1;
	Body_Move1 = 0.0f;
	Body_Move2 = 0.0f;
	Head_Move1 = 0.0f;
	Grobe_Grobe_L = 0.0f;
	Grobe_Grobe_R = 0.0f;
	Rotate_Z_R2 = 1.0f;
	Rotate_Z_R3 = 1.0f;
	Leg_L_Move = 0.0f;
	Leg_R_Move = 0.0f;


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
	if (Keyboard_IsTrigger(DIK_NUMPADENTER))
	{
		Step_Stop = true;
	}
	else if (Keyboard_IsTrigger(DIK_RETURN))
	{
		Step_Stop = true;
	}

	if (!Step_Stop)
	{
		//�A�j���[�V�����i�s
		Animation();
	}
	else
	{
		if (Upper_Phase == -1)
		{
			//Upper_Phase��0�ɂ���
			Upper_Phase = 0;
		}
		Upper();
	}

	//�ҋ@���[�V�����ȊO�̃A�j���[�V�������s���Ă��銎�A�G���p���`���łȂ��ꍇ�̓L�[���͂��s���Ȃ�
	if (!isAction)
	{
		if (GetPunch_Flg())
		{
			//�L�[���͂ɉ����ăX�e�[�g�ω�
			if (!isAction && Keyboard_IsTrigger(DIK_D) && !g_DodgeFlg)
			{//�E���
				g_PlAnimState = PLANIME_INDEX_DODGE_L;
				//�A�j���[�V�����t���[��������
				g_AnimFrameCnt = 0;
				//�A�N�V�������ɂ���
				isAction = true;

				//�������Ƀt���O�𗧂Ă�
				g_LrFlg = true;
				//�X�R�A��ǉ�����t���O�𗧂Ă�
				g_AddScoreFlg = true;

				//�X�R�A�𑝂₷
				if (g_AddScoreFlg == true && g_Once_AddScore_Flg == false && GetPunchIndex() == PPI_RIGHT_PUNCH)
				{
					AddScore(1000);
					AddGauge(10);
					//��x�̂݃X�R�A��������t���O�𐳂ɂ���
					g_Once_AddScore_Flg = true;
					//����t���O�𐳂ɂ���
					g_DodgeFlg = true;
				}

				if (g_Once_AddScore_Flg == false && GetPunchIndex() == R_FLASH_PUNCH)
				{
					AddScore(1000);
					AddGauge(10);
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
				g_LrFlg = false;
				//�X�R�A��ǉ�����t���O�𗧂Ă�
				g_AddScoreFlg = true;

				//�X�R�A�𑝂₷
				if (g_AddScoreFlg == true && g_Once_AddScore_Flg == false && (GetPunchIndex() == PPI_LEFT_PUNCH))
				{
					AddScore(1000);
					//��x�̂݃X�R�A��������t���O�𐳂ɂ���
					g_Once_AddScore_Flg = true;
					//����t���O�𐳂ɂ���
					g_DodgeFlg = true;
				}

				if (g_Once_AddScore_Flg == false && GetPunchIndex() == L_FLASH_PUNCH)
				{
					AddGauge(10);
					AddScore(1000);
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
			AddGauge(10);
			AddScore(1000);
			//��x�̂݃X�R�A��������t���O�𐳂ɂ���
			g_Once_AddScore_Flg = true;
		}
	}

	//����{�^�����ǂ�����������Ă�����A�j���[�V�����X�g�b�v����������
	//�f�o�b�O�̎��́u������Ă��Ȃ��Ƃ��v�ɂ��Ƃ���
	if ((AnimStopFlg && !Keyboard_IsPress(DIK_A) && !Keyboard_IsPress(DIK_D)))
	{
		AnimStopFlg = false;
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
			if (Upper_Phase == -1) {
				if (j == 2 || j == 3) {
					g_Player[j].pMaterials[i].Diffuse.a = 1.0f;
				}
				else {
					g_Player[j].pMaterials[i].Diffuse.a = 0.3f;
				}
			}
			//������t�F�[�Y���O���[�u���������ɂ��Ȃ�
			if (Upper_Phase >= 1) {
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
	if (Upper_Phase == 1) {
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
					if ((Keyboard_IsPress(DIK_D)) || (Keyboard_IsPress(DIK_A)))
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

void Upper()
{
	//�t���[�����Z
	Upper_Frame++;

	//�t�F�[�Y���̏���
	switch (Upper_Phase)
	{
		//�X�e�b�v�̃t�F�[�Y
	case 0:
		if (Upper_Frame < 10)
		{
			static float Phase1_Frame = 10.0f;
			Upper_Body1();
			Upper_Head1();
			Upper_Grobe_L1();
			Upper_Grobe_R1();
			Upper_Leg_L();
			Upper_Leg_R1();
		}
		else
		{
			//���̃t�F�[�Y�̏���&�t���[�����Z�b�g(�Ȍ�else���S�Ăɓ���)
			g_Player[3].Rot = D3DXVECTOR3(D3DXToRadian(180.0f), D3DXToRadian(90.0f), D3DXToRadian(0.0f));
			//g_Player[0].Rot.y +=D3DXToRadian(30.0f);
			Upper_Phase++;
			Upper_Frame = 0;
		}
		break;

		//�L�[����
	case 1:
		if (Keyboard_IsTrigger(DIK_RETURN))
		{
			Upper_Phase++;
			Upper_Frame = 0;
		}
		else if (Keyboard_IsTrigger(DIK_NUMPADENTER))
		{
			Upper_Phase++;
			Upper_Frame = 0;
		}
		break;

		//�A�b�p�[�̑̂ɓ�����܂ł̃t�F�[�Y
	case 2:

		//�r��̂܂œ���������
		if (Upper_Frame < 8)
		{
			static float Phase2_Frame = 10.0f;
			Upper_Grobe_R2(Phase2_Frame);
		}
		//�q�b�g�X�g�b�v
		if (Upper_Frame >= 10) {
			Upper_Phase++;
			Upper_Frame = 0;
		}
		break;

		//�A�b�p�[�̑̂ɓ���������̃t�F�[�Y
	case 3:
		//�̂���r��U��؂鏈��
		if (Upper_Frame < 8)
		{
			//�X�R�A�𑝂₷
			AddScore(111111);
			static float Phase3_Frame = 10.0f;
			Upper_Grobe_R3(Phase3_Frame);
		}
		break;

	default:
		break;
	}
}

//�p�[�c���Ƃ̃A�b�p�[���̓���
void Upper_Body1()
{

	Body_Move1 = Body_Move1 + 0.75f;
	g_Player[0].Pos.y -= 0.05f;
	g_Player[0].Pos.z = Body_Move1;
	g_Player[0].Rot.x -= D3DXToRadian(3.0f);
}

void Upper_Body2()
{
	Body_Move2 += 3.0f;
	g_Player[0].Rot.y = D3DXToRadian(Body_Move2);
}

void Upper_Head1()
{
	Head_Move1 = Head_Move1 + 1.2f;
	g_Player[1].Pos.y -= 0.12f;
	g_Player[1].Pos.z = Head_Move1;
	g_Player[1].Rot.x -= D3DXToRadian(3.0f);
}

void Upper_Grobe_L1()
{
	Grobe_Grobe_L = Grobe_Grobe_L + 0.75f;
	g_Player[2].Pos.y -= 0.05f;
	g_Player[2].Pos.z = Grobe_Grobe_L * 1.5f;
	g_Player[2].Rot.x -= D3DXToRadian(3.0f);

}

void Upper_Grobe_R1()
{
	Grobe_Grobe_R = Grobe_Grobe_R + 0.75f;
	g_Player[3].Pos.y -= 0.05f;
	g_Player[3].Pos.z = Grobe_Grobe_R * 1.5f;
	g_Player[3].Rot.x -= D3DXToRadian(3.0f);

}

void Upper_Grobe_R2(float Time)
{
	Rotate_Z_R2 = Rotate_Z_R2 * 1.35f;
	g_Player[3].Pos.x += D3DXToRadian(90.0f / Time) * -1.8f;
	g_Player[3].Pos.y += D3DXToRadian(90.0f / Time) * 1.5f; //1.5f
	g_Player[3].Pos.z += D3DXToRadian(90.0f / Time) * 11.0f;
	g_Player[3].Rot.z += D3DXToRadian(Rotate_Z_R2);
}

void Upper_Grobe_R3(float Time)
{
	Rotate_Z_R3 = Rotate_Z_R3 * 1.35f;
	g_Player[3].Pos.y += D3DXToRadian(90.0f / Time) * 6.5f;
	g_Player[3].Pos.z += D3DXToRadian(90.0f / Time) * 7.0f;
	g_Player[3].Rot.z += D3DXToRadian(Rotate_Z_R3);
}

void Upper_Leg_L()
{
	Leg_L_Move += 1.5f;
	g_Player[4].Pos.z = Leg_L_Move;
}

void Upper_Leg_R1()
{
	Leg_R_Move += 0.05f;
	g_Player[5].Pos.y += Leg_R_Move;
	g_Player[5].Rot.x -= D3DXToRadian(8.0f);
}

//�v���C���[��Getter
XMODEL* GetPlayer()
{
	return g_Player;
}

//Upper_Phase��Getter
int GetUpper_Phase()
{
	return Upper_Phase;
}
