#pragma once

#include "main.h"

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void InitCamera(void);
void UninitCamera(void);
void UpdateCamera(void);

void SetCamera(void);

//�e�V�[�����Ƃ̏�����
void Init_TitleCamera();
void Init_GameCamera();
void Init_RankingCamera();

//�J�����̍��W�������֐�
void CameraResetPos();

//�J������Y����](�����ړ�)����֐�
void Camera_Rot_Y();

//�J������X����](�����ړ�)����֐�
void Camera_Rot_X();

//���݂̃J�����̍��W�ƒ����_�Ƃ̋�����Ԃ��֐�
//�����F 0:X�̋�����Ԃ� 1:Y�̋�����Ԃ� 2:Z�̋�����Ԃ� 3:�S�Ă̋�����Ԃ�
D3DXVECTOR3 Camera_Length(int Return_Num);

//�Œ莲�ړ��F�w����W�Ɉړ�����֐�
//����1�F�J�����̈ړ��J�n�ʒu�̍��W
//����2�F�J�����̏I���ʒu�̍��W
//����3�F�ړ�����(�b)
void Camera_Move_Axis1(D3DXVECTOR3(Start_Pos), D3DXVECTOR3(End_Pos), float Move_Time);

//Y���ړ��FY���ړ�����֐�
//����1�F�J�����̈ړ��J�n�ʒu�̍��W
//����2�F�ǂꂾ��Y����]���邩(���̒l�Ȃ�E�ɉ�荞��)
//����3�F�ړ�����(�b)
void Camera_Move_Yspin(D3DXVECTOR3(Start_Pos), float Rota_Y, float Rota_Time);

//X���ړ��FX���ړ�����֐�
//����1�F�J�����̈ړ��J�n�ʒu�̍��W
//����2�F�ǂꂾ��X����]���邩(���̒l�Ȃ��ɉ�荞��)
//����3�F�ړ�����(�b)
void Camera_Move_Xspin(D3DXVECTOR3(Start_Pos), float Rota_X, float Rota_Time);

//�ǉ�����1==========================================
//�ȉ��Ō�̃p���`�֌W�̃J�������[�N
void LastPunchCamera1();

void LastPunchCamera2();

void LastPunchCamera3();

void LastPunchCamera4();

void Finish_CameraMove();
//�ǉ�����1==========================================