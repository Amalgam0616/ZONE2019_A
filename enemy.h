#pragma once

/*====================================================================================
�쐬�ҁF(Amalgam)���㊰�K
�쐬�J�n�����F2019/11/07/14:25

�G�������\�[�X�R�[�h�ł��B
====================================================================================*/

#include "main.h"
#include "Xfile.h"

//�u�Ԉړ��p���`�i�E�j�̏����ʒu�A��]�B�����̓p�[�c�̔z��Ɠ���
#define R_FLASH_PUNCH_R_POS0 D3DXVECTOR3(-10.0f,15.0f,26.0f) 
#define R_FLASH_PUNCH_R_POS1 D3DXVECTOR3(-10.0f,30.0f,26.0f) 
#define R_FLASH_PUNCH_R_POS2 D3DXVECTOR3(-5.6f,21.0f,20.0f) 
#define R_FLASH_PUNCH_R_POS3 D3DXVECTOR3(-30.0f,28.0f,39.0f) 
#define R_FLASH_PUNCH_R_POS4 D3DXVECTOR3(-18.0f,1.0f,26.0f) 
#define R_FLASH_PUNCH_R_POS5 D3DXVECTOR3(-16.0f,2.85f,26.0f) 

#define R_FLASH_PUNCH_R_ROT0 D3DXVECTOR3(D3DXToRadian(0.0f),D3DXToRadian(20.0f),D3DXToRadian(0.0f)) 
#define R_FLASH_PUNCH_R_ROT1 D3DXVECTOR3(D3DXToRadian(-22.0f),D3DXToRadian(-26.0f),D3DXToRadian(0.0f)) 
#define R_FLASH_PUNCH_R_ROT2 D3DXVECTOR3(D3DXToRadian(32.0f),D3DXToRadian(162.0f),D3DXToRadian(90.0f)) 
#define R_FLASH_PUNCH_R_ROT3 D3DXVECTOR3(D3DXToRadian(90.0f),D3DXToRadian(140.0f),D3DXToRadian(180.0f)) 
#define R_FLASH_PUNCH_R_ROT4 D3DXVECTOR3(D3DXToRadian(0.0f),D3DXToRadian(0.0f),D3DXToRadian(0.0f)) 
#define R_FLASH_PUNCH_R_ROT5 D3DXVECTOR3(D3DXToRadian(0.0f),D3DXToRadian(0.0f),D3DXToRadian(0.0f)) 

//�u�Ԉړ��p���`�i���j�̏����ʒu�A��]�B�����̓p�[�c�̔z��Ɠ���
#define L_FLASH_PUNCH_POS0 D3DXVECTOR3(10.0f,15.0f,26.0f) 
#define L_FLASH_PUNCH_POS1 D3DXVECTOR3(10.0f,30.0f,26.0f) 
#define L_FLASH_PUNCH_POS2 D3DXVECTOR3(25.4f,27.2f,35.9f) 
#define L_FLASH_PUNCH_POS3 D3DXVECTOR3(2.5f,21.0f,23.4f) 
#define L_FLASH_PUNCH_POS4 D3DXVECTOR3(24.0f,1.0f,26.0f) 
#define L_FLASH_PUNCH_POS5 D3DXVECTOR3(6.0f,2.85f,26.0f) 

#define L_FLASH_PUNCH_ROT0 D3DXVECTOR3(D3DXToRadian(-2.4f),D3DXToRadian(-8.0f),D3DXToRadian(0.0f)) 
#define L_FLASH_PUNCH_ROT1 D3DXVECTOR3(D3DXToRadian(-26.5f),D3DXToRadian(21.4f),D3DXToRadian(0.0f)) 
#define L_FLASH_PUNCH_ROT2 D3DXVECTOR3(D3DXToRadian(90.0f),D3DXToRadian(121.0f),D3DXToRadian(55.6f)) 
#define L_FLASH_PUNCH_ROT3 D3DXVECTOR3(D3DXToRadian(20.3f),D3DXToRadian(155.7f),D3DXToRadian(231.5f)) 
#define L_FLASH_PUNCH_ROT4 D3DXVECTOR3(D3DXToRadian(0.0f),D3DXToRadian(0.0f),D3DXToRadian(0.0f)) 
#define L_FLASH_PUNCH_ROT5 D3DXVECTOR3(D3DXToRadian(0.0f),D3DXToRadian(0.0f),D3DXToRadian(0.0f))




//�p���`�̃t�F�[�Y(���)
enum PUNCH_PHASE
{
	PUNCH_PHASE_CHARGE = 0,
	PUNCH_PHASE_SWING,
	PUNCH_PHASE_RETURN,

};

//�p���`�p�^�[�����[
enum PUNCH_PATTERN_INDEX
{
	PPI_RIGHT_PUNCH = 0,	//�E�ʏ�p���`
	PPI_LEFT_PUNCH,			//���ʏ�p���`
	R_FLASH_PUNCH,			//�E�u�Ԉړ��p���`
	L_FLASH_PUNCH,			//���u�Ԉړ��p���`
	PPI_DUNK_PUNCH,			//����o�[���Ă���p���`

	PUNCH_NULL,				//�p���`���ĂȂ����

	PPI_MAX
};


//������
HRESULT InitEnemy();
//�I��
void UninitEnemy();
//�`��
void DrawEnemy();
//�X�V
void UpdateEnemy();

//���̑�

//���f���������ʒu�ɖ߂��֐�
void EnemyPosReset();

void EnemyUpperPos();

void EnmAnimation();

void R_FLASH_PUNCH_R();

//�p���`�֐��Q
//�E�ʏ�p���`
void RightPunch();
//���ʏ�p���`
void LeftPunch();
//�E�u�Ԉړ��p���`
void Right_R_FLASH_PUNCH();
//���u�Ԉړ��p���`
void Left_R_FLASH_PUNCH();
//����o�[���Ă���p���`
void DunkPunch();

//�p���`��Ɍ��̈ʒu�ɖ߂�������
void CreatePunchEndVec();

//�G��Getter
XMODEL* GetEnemy();

//�p���`�����ۂ���Getter
bool GetPunch_Flg();

//�p���`�t�F�[�Y��Getter
PUNCH_PHASE GetPunchPhase();

//�p���`�̍��EGetter
bool GetPunchLR();

//�����Ă���p���`�̎�ނ�Getter
PUNCH_PATTERN_INDEX GetPunchIndex();