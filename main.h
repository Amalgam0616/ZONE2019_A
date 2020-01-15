#pragma once

#include <Windows.h>
#include <d3dx9.h>

//==============================
//   �萔��`
//==============================
#define SCREEN_WIDTH	(1280)	// �X�N���[��(�N���C�A���g�̈�)�̕�
#define SCREEN_HEIGHT	(720)	// �X�N���[��(�N���C�A���g�̈�)�̍���

#define	NUM_VERTEX		(1000)		// ���_��
#define	NUM_POLYGON		(1000)		// �|���S����

//========================================================
// ��L�Q�c�|���S�����_�t�H�[�}�b�g�ɍ��킹���\���̂̒�`
//========================================================
typedef struct
{
	D3DXVECTOR3 pos;	// ���_���W
	float rhw;			// ���W�ϊ��p�W��(1.0f�ŌŒ�)
	D3DCOLOR col;		// ���_�J���[
	D3DXVECTOR2 tex;	// �e�N�X�`�����W
} VERTEX_2D;

// �Q�c�|���S�����_�t�H�[�}�b�g( ���_���W[2D] / ���_�J���[ / �e�N�X�`�����W )
#define	FVF_VERTEX_2D	(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

//========================================================
// ��L�R�c�|���S�����_�t�H�[�}�b�g�ɍ��킹���\���̂̒�`
//========================================================
typedef struct
{
	D3DXVECTOR3 pos;	// ���_���W
	D3DXVECTOR3 nor;	// �@���x�N�g��
	D3DCOLOR col;		// ���_�J���[
	D3DXVECTOR2 tex;	// �e�N�X�`�����W


} VERTEX_3D;

// �R�c�|���S�����_�t�H�[�}�b�g( ���_���W[3D] / �@�� / ���_�J���[ / �e�N�X�`�����W )
#define	FVF_VERTEX_3D	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)

//���S�ɉ������(Releace�֐������S�Ɏg�p����)
#define SAFE_RELEASE(p) {if(p){(p)->Release();(p)=NULL;}}

//====================================
//  �V�[����`(enum)
//====================================
enum SCENE_INDEX
{
	SCENE_INDEX_NONE = 0,

	SCENE_INDEX_TITLE,		//�^�C�g��
	SCENE_INDEX_GAME,		//�Q�[��
	SCENE_INDEX_RANKING,	//RANKING

	SCENE_INDEX_MAX			//�ő�V�[����
};

//====================================
//  �֐��錾
//====================================
//HWND�̃Q�b�^�[
HWND GetHWND();