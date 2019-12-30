#pragma once


/*====================================================================================
�쐬�ҁF(Amalgam)���㊰�K
�쐬�J�n�����F2019/11/07/14:25

�v���C���[�������\�[�X�R�[�h�ł��B
====================================================================================*/

#include "main.h"
#include "Xfile.h"

//�A�j���[�V�����̃C���f�b�N�X
enum PLAYER_ANIME_INDEX
{
	PLANIME_INDEX_NORMAL = 0,
	PLANIME_INDEX_DODGE_L,
	PLANIME_INDEX_DODGE_R,
	PLANIME_INDEX_DAMAGE,
	PLANIME_INDEX_UPPER,

	ANIME_INDEX_MAX,
};

//�A�j���[�V�����֘A�\����
typedef struct
{
	int		StrFrame;		//�J�n����I���܂�

	int		EndFrame;		//�I�����珉���ʒu�܂�

	D3DXVECTOR3		Angle;	//�p�x
	D3DXVECTOR3		Move;	//�ړ�����
}PL_ANIME;

//������
HRESULT InitPlayer();
//�I��
void UninitPlayer();
//�`��
void DrawPlayer();
//�X�V
void UpdatePlayer();

//���̑�

//������A�j���[�V����������֐�
void Animation();

//�A�j���X�e�[�g���̓���(����폜����\���A��)
void AnimMovingParFrame();


void PlayerPosReset();

//�A�b�p�[�֘A
void Upper();

//�A�b�p�[���̊e�p�[�c�̓���
void Upper_Body1();

void Upper_Body2();

void Upper_Head1();

void Upper_Grobe_L1();

void Upper_Grobe_R1();

void Upper_Grobe_R2(float Time);

void Upper_Grobe_R3(float Time);

void Upper_Leg_L();

void Upper_Leg_R1();

//�v���C���[��Getter
XMODEL* GetPlayer();

bool GetDodgeFlg();

bool GetLrFlg();

//Upper_Phase��Getter
int GetUpper_Phase();
