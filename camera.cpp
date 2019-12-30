
#include "camera.h"
#include "myDirect3D.h"
#include "input.h"
#include <stdlib.h>
#include "player.h"
#include "enemy.h"
#include "scene.h"
//=============================================================================
// �}�N����`
//=============================================================================

//Z���W�����l
#define CAMERA_POS_Z (80.0f)

//�J�����̎��_(�J�����̍��W)�����ʒu(���ꂢ��H)
#define	CAMERA_POSV_X		(0.0f)
#define	CAMERA_POSV_Y		(6.0f)
#define	CAMERA_POSV_Z		(-CAMERA_POS_Z)

//�J�����̒����_�����ʒu(���ꂢ��H)
#define	CAMERA_POSR_X		(0.0f)
#define	CAMERA_POSR_Y		(3.0f)
#define	CAMERA_POSR_Z		(0.0f)

//�r���[���ʂ̎���p(����͗v�肻��)
#define	VIEW_ANGLE			(D3DXToRadian(45.0f))

//�r���[���ʂ̃A�X�y�N�g��(����͗v�肻��)
#define	VIEW_ASPECT			((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)

//�r���[���ʂ�NearZ�AFarZ�l(����͗v�肻��)
#define	VIEW_NEAR_Z			(1.0f)		//������0.0f�w�肵�Ă��܂��ƁA�����ɉ����܂ŕ`�悷��悤�ɂȂ��Ă��܂��̂Œ���
#define	VIEW_FAR_Z			(5000.0f)

//�J������Y���̉���
#define CAMERA_TOP_LIMIT    (330.0f)
#define CAMERA_BOTTOM_LIMIT (210.0f)

// �J�����ړ����̔��a(���ꂢ��H)
#define	CAMERA_RAD		(CAMERA_POS_Z)

//�J�����̊�{��]���x(��œ���ȓ��������邽�߉��u��)
#define CAMERA_SPEED	(5.0f)
#define TITLECAMERA_SPEED	(0.2f)

//�ړ�����(�̂��ɏ�����])
#define AXIS1_TIME 3.0f
#define YSPIN_TIME 6.0f
#define XSPIN_TIME 3.0f

//=============================================================================
// �J�����̍\����
//=============================================================================
typedef struct
{
	D3DXVECTOR3 posV;				// ���_
	D3DXVECTOR3 posR;				// �����_
	D3DXVECTOR3 vecU;				// ������x�N�g��
	D3DXMATRIX mtxProjection;		// �v���W�F�N�V�����}�g���b�N�X
	D3DXMATRIX mtxView;				// �r���[�}�g���b�N�X

}CAMERA;

//=============================================================================
// �O���[�o���ϐ�
//=============================================================================
CAMERA g_Camera;					//�J�������
float g_CameraRot_X;				//�J�����p�x(X)
float g_CameraRot_Y;				//�J�����p�x(Y)
float g_CameraRot_Z;				//�J�����p�x(Z)
static bool g_Reflect_Flg;			//�^�C�g���ŉ�������Ƃ��̃t���O
int g_TitleCamera_frame;			//�^�C�g���̃t���[����
bool g_Player_Camera;				//�J�������v���C���[�̎��_�ɂ���t���O
int g_Upper_Cnt;					//�A�b�p�[�̎��ɃJ�������~�߂�J�E���g

//=========================================================================
//��������J�����̓��ꋓ���p�̕ϐ�
//=========================================================================
static float g_Axis1_Scond;	//���̈ړ�����(�P�ʂ͕b)
static bool g_Axis1_Flg;		//�Œ莲�ړ��̃X�C�b�`
static int g_Axis1_Time_Frame;	//�Œ莲�ړ��̋N������(�ړ����Ԃ��t���[�����Ŏ󂯎��)

static float g_Yspin_Scond;		//���̈ړ�����(�P�ʂ͕b)
static bool g_Yspin_Flg;		//Y����]�ړ��̃X�C�b�`
static int g_Yspin_Time_Frame;	//Y����]�ړ��̋N������(�ړ����Ԃ��t���[�����Ŏ󂯎��)

static float g_Xspin_Scond;		//���̈ړ�����(�P�ʂ͕b)
static bool g_Xspin_Flg;		//Y����]�ړ��̃X�C�b�`
static int g_Xspin_Time_Frame;	//Y����]�ړ��̋N������(�ړ����Ԃ��t���[�����Ŏ󂯎��)

//=============================================================================
// �J�����̏���������
//=============================================================================
void InitCamera(void)
{
	CameraResetPos();						//�J�����̍��W������
	g_Upper_Cnt = 0;

}

//=============================================================================
// �J�����̏I������
//=============================================================================
void UninitCamera(void)
{
	//�V�[�����Ƃɏ����������肷��񂾂낤���ǁA�V�[�����X�ȃV�[���ڍs���Ɨv��Ȃ��̂�������Ȃ�
}

//=============================================================================
// �J�����̍X�V����
//=============================================================================
void UpdateCamera(void)
{
	//====================================================================================
	//�J�������W�ύX
	//====================================================================================

	if (GetScene() == SCENE_INDEX_TITLE)
	{
		//g_TitleCamera_frame ++;
		if (g_Reflect_Flg && g_CameraRot_X > 330)
		{
			g_Reflect_Flg = false;
		}
		else if (!g_Reflect_Flg && g_CameraRot_X < 210)
		{
			g_Reflect_Flg = true;
		}

		//�^�C�g���̎��̍X�V����
		if (g_Reflect_Flg)
		{
			//Camera_Move_Yspin(D3DXVECTOR3(CAMERA_POSV_X, CAMERA_POSV_Y, CAMERA_POSV_Z), 60.0f, 3.0f);
			g_CameraRot_X += TITLECAMERA_SPEED;
			g_CameraRot_Z += TITLECAMERA_SPEED;
			Camera_Rot_Y();
		}
		else
		{
			g_CameraRot_X -= TITLECAMERA_SPEED;
			g_CameraRot_Z -= TITLECAMERA_SPEED;
			Camera_Rot_Y();
		}
	}
	else if (GetScene() == SCENE_INDEX_GAME)
	{
		//�Q�[���̎��̍X�V����

		g_Player_Camera = true;
		//if (Keyboard_IsPress(DIK_W))
		//{// �E�ړ�
		//	//�p�x���Z
		//	g_CameraRot_X += CAMERA_SPEED;
		//	g_CameraRot_Z += CAMERA_SPEED;
		//	Camera_Rot_Y();
		//}
		//if (Keyboard_IsPress(DIK_A))
		//{// ���ړ�
		//	g_CameraRot_X -= CAMERA_SPEED;
		//	g_CameraRot_Z -= CAMERA_SPEED;
		//	Camera_Rot_Y();
		//}

		//if (Keyboard_IsPress(DIK_D))
		//{// �E�ړ�
		//	//�p�x���Z
		//	if (g_CameraRot_Y < CAMERA_TOP_LIMIT)
		//	{
		//		g_CameraRot_Y += CAMERA_SPEED;
		//	}
		//	Camera_Rot_X();
		//}
		//if (Keyboard_IsPress(DIK_S))
		//{// ���ړ�
		//	if (g_CameraRot_Y > CAMERA_BOTTOM_LIMIT)
		//	{
		//		g_CameraRot_Y -= CAMERA_SPEED;
		//	}
		//	Camera_Rot_X();
		//}

		//�J�����̈ʒu�ƈړ����Ԃ̃��Z�b�g
		if (Keyboard_IsTrigger(DIK_R))
		{
			g_Camera.posV = D3DXVECTOR3(CAMERA_POSV_X, CAMERA_POSV_Y, CAMERA_POSV_Z);
			g_Camera.posR = D3DXVECTOR3(CAMERA_POSR_X, CAMERA_POSR_Y, CAMERA_POSR_Z);
			g_CameraRot_X = 270.0f;
			g_CameraRot_Y = 270.0f;
			g_CameraRot_Z = 270.0f;
			g_Axis1_Scond = AXIS1_TIME;
			g_Yspin_Scond = YSPIN_TIME;
		}

		if (Keyboard_IsPress(DIK_O))
		{
			g_Camera.posV.z++;
		}

		if (Keyboard_IsPress(DIK_L))
		{
			g_Camera.posV.z--;
		}

		//�u1�v�ŌŒ莲�ړ��̃t���O��true�ɂ���
		if (Keyboard_IsTrigger(DIK_1) && !g_Axis1_Flg)
		{
			g_Axis1_Flg = true;
			g_Axis1_Time_Frame = g_Axis1_Scond * 60;	//Axis1�`�Ɉړ����Ԃ��t���[�����ɂ��đ������
		}

		//�Œ莲�ړ��̃t���O��true���A���ړ����Ԃ��c���Ă��邩������
		if (g_Axis1_Flg && g_Axis1_Time_Frame > 0.0f)
		{
			//�Œ莲�ړ�
			Camera_Move_Axis1(g_Camera.posV,
				D3DXVECTOR3(g_Camera.posV.x + 20, g_Camera.posV.y, g_Camera.posV.z + 40.0f),
				g_Axis1_Scond);

			g_Axis1_Time_Frame--;

			//�ړ�����(�t���[��)��0�ȉ��ɂȂ�����Œ莲�ړ��̃t���O��false�ɂ���
			if (g_Axis1_Time_Frame <= 0)
			{
				g_Axis1_Flg = false;
			}
		}


		//�u2�v��Y����]�ړ��̃t���O��true�ɂ���
		if (Keyboard_IsTrigger(DIK_2) && !g_Yspin_Flg)
		{
			g_Yspin_Flg = true;
			g_Yspin_Time_Frame = g_Yspin_Scond * 60;	//Spin1�`�Ɉړ����Ԃ��t���[�����ɂ��đ������
		}

		//Y����]�ړ��̃t���O��true���A���ړ����Ԃ��c���Ă��邩������
		if (g_Yspin_Flg && g_Yspin_Time_Frame > 0.0f)
		{
			//Y����]�ړ�
			Camera_Move_Yspin(g_Camera.posV,
				180.0f,
				g_Yspin_Scond);

			g_Yspin_Time_Frame--;

			//�ړ�����(�t���[��)��0�ȉ��ɂȂ���Y����]�ړ��̃t���O��false�ɂ���
			if (g_Yspin_Time_Frame <= 0)
			{
				g_Yspin_Flg = false;
			}
		}


		//�u3�v��X����]�ړ��̃t���O��true�ɂ���
		if (Keyboard_IsTrigger(DIK_3) && !g_Xspin_Flg)
		{
			g_Xspin_Flg = true;
			g_Xspin_Time_Frame = g_Xspin_Scond * 60;	//Xspin�`�Ɉړ����Ԃ��t���[�����ɂ��đ������
		}

		//X����]�ړ��̃t���O��true���A���ړ����Ԃ��c���Ă��邩������
		if (g_Xspin_Flg && g_Xspin_Time_Frame > 0.0f)
		{
			/*if (g_CameraRot_Y < CAMERA_TOP_LIMIT || g_CameraRot_Y > CAMERA_BOTTOM_LIMIT)
			{
				g_Xspin_Flg = false;
			}*/
			//X����]�ړ�
			Camera_Move_Xspin(g_Camera.posV,
				60.0f,
				g_Xspin_Scond);

			g_Xspin_Time_Frame--;

			//�ړ�����(�t���[��)��0�ȉ��ɂȂ���X����]�ړ��̃t���O��false�ɂ���
			if (g_Xspin_Time_Frame <= 0)
			{
				g_Xspin_Flg = false;
			}
		}

		if (Keyboard_IsTrigger(DIK_P))
		{
			if (!g_Player_Camera)
			{
				g_Player_Camera = true;
			}
			else
			{
				g_Player_Camera = false;
			}
		}


		//�A�b�p�[�ɓ�������
		if (GetUpper_Phase() == 1)
		{
			g_Camera.posV = D3DXVECTOR3(-50, 10, -50);
		}
		//�A�b�p�[�����܂�����G�ɃJ�������Ǐ]����
		else if (GetUpper_Phase() == 3)
		{
			//�����ɂ͒Ǐ]�����Ȃ��悤�ɂ��鏈��
			g_Upper_Cnt++;
			if (g_Upper_Cnt >= 90) {
				g_Camera.posR = D3DXVECTOR3((GetEnemy() + 1)->Pos.x, (GetEnemy() + 1)->Pos.y, (GetEnemy() + 1)->Pos.z);
			}
		}
		if (GetUpper_Phase() == -1)
		{
			//g_Camera.posR = D3DXVECTOR3(GetEnemy()->Pos.x, GetEnemy()->Pos.y + 7.0f, GetEnemy()->Pos.z);
			g_Camera.posV.x = (GetPlayer() + 1)->Pos.x;
			g_Camera.posV.z = (GetPlayer() + 1)->Pos.z - 7;
		}


	}
	else if (GetScene() == SCENE_INDEX_RANKING)
	{
		//�����L���O�̎��̍X�V����
	}

}

//=============================================================================
// �J�����̐ݒ菈��
//=============================================================================

//�J�������̐ݒ�(Draw�֐����g���Ƃ���(���f���Ƃ���Draw���钼�O���x�^�[)�ɒu���Ă�������)
void SetCamera(void)
{
	LPDIRECT3DDEVICE9 p_Device = GetD3DDevice();

	// �v���W�F�N�V�����}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&g_Camera.mtxProjection);

	// �v���W�F�N�V�����}�g���b�N�X�̍쐬
	D3DXMatrixPerspectiveFovLH(&g_Camera.mtxProjection,
		VIEW_ANGLE,				// �r���[���ʂ̎���p
		VIEW_ASPECT,			// �r���[���ʂ̃A�X�y�N�g��
		VIEW_NEAR_Z,			// �r���[���ʂ�NearZ�l
		VIEW_FAR_Z);			// �r���[���ʂ�FarZ�l

// �v���W�F�N�V�����}�g���b�N�X�̐ݒ�
	p_Device->SetTransform(D3DTS_PROJECTION, &g_Camera.mtxProjection);

	// �r���[�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&g_Camera.mtxView);

	// �r���[�}�g���b�N�X�̍쐬
	D3DXMatrixLookAtLH(&g_Camera.mtxView,
		&g_Camera.posV,		// �J�����̎��_
		&g_Camera.posR,		// �J�����̒����_
		&g_Camera.vecU);	// �J�����̏�����x�N�g��

// �r���[�}�g���b�N�X�̐ݒ�
	p_Device->SetTransform(D3DTS_VIEW, &g_Camera.mtxView);
}

void Init_TitleCamera()
{
	CameraResetPos();						//�J�����̍��W������

	g_TitleCamera_frame = 0;				//�^�C�g���̃t���[�����̏�����
	g_Reflect_Flg = false;					//�t���O�̏�����
}

void Init_GameCamera()
{
	CameraResetPos();						//�J�����̍��W������

	//�J�����̒����_�̐ݒ�
	g_Camera.posR = D3DXVECTOR3(GetEnemy()->Pos.x, GetEnemy()->Pos.y + 7.0f, GetEnemy()->Pos.z);

	g_Axis1_Scond = AXIS1_TIME;				//�Œ莲�ړ��̈ړ�����(�o�O�邩��Ƃ肠�����������)
	g_Axis1_Flg = false;					//�t���O�̏�����

	g_Yspin_Scond = YSPIN_TIME;				//Y����]�ړ��̈ړ�����(�o�O�邩��Ƃ肠�����������)
	g_Yspin_Flg = false;					//�t���O�̏�����

	g_Xspin_Scond = XSPIN_TIME;				//X����]�ړ��̈ړ�����(�o�O�邩��Ƃ肠�����������)
	g_Xspin_Flg = false;					//�t���O�̏�����
}

void Init_RankingCamera()
{
	g_Camera.posR = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

void Camera_Rot_Y()
{
	g_Camera.posV.x = cosf(D3DXToRadian(g_CameraRot_X)) * CAMERA_RAD;
	g_Camera.posV.z = sinf(D3DXToRadian(g_CameraRot_Z)) * CAMERA_RAD;
}

void Camera_Rot_X()
{
	g_Camera.posV.y = cosf(D3DXToRadian(g_CameraRot_Y)) * CAMERA_RAD;
}

D3DXVECTOR3 Camera_Length(int Return_Num)
{
	D3DXVECTOR3 leng = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	
	switch (Return_Num)
	{
	case 0:
		leng.x = fabs(g_Camera.posV.x - g_Camera.posR.x);
		return leng;

	case 1:
		leng.y = fabs(g_Camera.posV.y - g_Camera.posR.y);
		return leng;

	case 2:
		leng.z = fabs(g_Camera.posV.z - g_Camera.posR.z);
		return leng;

	case 3:
		leng.x = fabs(g_Camera.posV.x - g_Camera.posR.x);
		leng.y = fabs(g_Camera.posV.y - g_Camera.posR.y);
		leng.z = fabs(g_Camera.posV.z - g_Camera.posR.z);
		return leng;

		//�O�̂���
	default:
		return leng;
	}
}

void CameraResetPos()
{
	//�X�C�b�`���ŃV�[�����Ƃɏ����ʒu�ݒ肷�邱�ƂɂȂ邩�������B�v�m�F
	g_Camera.posV = D3DXVECTOR3(CAMERA_POSV_X, CAMERA_POSV_Y, CAMERA_POSV_Z);
	g_Camera.posR = D3DXVECTOR3(CAMERA_POSR_X, CAMERA_POSR_Y, CAMERA_POSR_Z);
	g_Camera.vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	g_Player_Camera = false;

	g_CameraRot_X = 270.0f;			//�J�����p�x(X)
	g_CameraRot_Y = 270.0f;			//�J�����p�x(Y)
	g_CameraRot_Z = 270.0f;			//�J�����p�x(Z)

}

//�Œ莲�ړ��F���ړ��֐�
void Camera_Move_Axis1(D3DXVECTOR3(Start_Pos), D3DXVECTOR3(End_Pos), float Move_Time)
{
	g_Camera.posV.x += (End_Pos.x - Start_Pos.x) / (Move_Time * 60);
	g_Camera.posV.y += (End_Pos.y - Start_Pos.y) / (Move_Time * 60);
	g_Camera.posV.z += (End_Pos.z - Start_Pos.z) / (Move_Time * 60);
}

//Y����]�ړ��F�J������]
void Camera_Move_Yspin(D3DXVECTOR3(Start_Pos), float Rota_Y, float Spin_Time)
{
	g_CameraRot_X += Rota_Y / (Spin_Time * 60);
	g_CameraRot_Z += Rota_Y / (Spin_Time * 60);
	g_Camera.posV.x = cosf(D3DXToRadian(g_CameraRot_X)) * CAMERA_RAD;
	g_Camera.posV.z = sinf(D3DXToRadian(g_CameraRot_Z)) * CAMERA_RAD;
}

//X����]�ړ��F�J������]
void Camera_Move_Xspin(D3DXVECTOR3(Start_Pos), float Rota_X, float Spin_Time)
{
	g_CameraRot_Y += Rota_X / (Spin_Time * 60);
	g_Camera.posV.y = cosf(D3DXToRadian(g_CameraRot_Y)) * CAMERA_RAD;
}

void Upeer_CameraMove()
{
	g_Camera.posR = D3DXVECTOR3((GetEnemy() + 1)->Pos);
}