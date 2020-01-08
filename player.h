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
	//�ǉ�����1==========================================
	PLANIME_INDEX_FINISHPUNCH,
	//�ǉ�����1==========================================
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

//�ǉ�����2==========================================
void Finish_Punch_Pos();

void Finish_Punch();
//�ǉ�����2==========================================

//�v���C���[��Getter
XMODEL* GetPlayer();

bool GetDodgeFlg();

bool GetLrFlg();

