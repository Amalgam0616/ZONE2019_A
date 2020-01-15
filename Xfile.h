#pragma once

/*==================================================================================
����ҁFAmalgam
�T�v�FXfile���g���₷��1�̊֐��œǂݍ��݂ł���悤�ɂ��Ă݂܂���
�@�@�@��̂�GM�Ŏg����LoadTexture�֐��Ɠ��������ŁADX��Texture�����@�\�𐷂荞��ł݂܂����B
�@�@�@�A�h���X�w����C���f�b�N�X�̕���������₷���ł���H����
==================================================================================*/

#include "main.h"

//XFile��3D���f�����������\����
typedef struct
{
	LPD3DXMESH				pMesh;			//���b�V��
	D3DMATERIAL9			*pMaterials;	//�}�e���A��
	LPDIRECT3DTEXTURE9		*pTextures;		//�e�N�X�`��

	DWORD dwNumMaterials;		//3D���f���Ɏg���Ă���}�e���A���̐�

	D3DXVECTOR3 Pos, Rot, Scale;	//���W�A��]�p�x�A�g��k��
}XMODEL;

//XFile�̃C���f�b�N�X�@�ǉ�����ꍇ�͂����ƁAcpp�̕���g_XFileList�ɏ����Ă�������
enum XFILE_INDEX
{
	//�v���C���[
	XFILE_INDEX_P_BODY_001 = 0,
	XFILE_INDEX_P_HEAD_001,
	XFILE_INDEX_P_GROBE_L_001,
	XFILE_INDEX_P_GROBE_R_001,
	XFILE_INDEX_P_LEG_L_001,
	XFILE_INDEX_P_LEG_R_001,
	//�G�l�~�[
	XFILE_INDEX_E_BODY_001,
	XFILE_INDEX_E_HEAD_001,
	XFILE_INDEX_E_GROBE_L_001,
	XFILE_INDEX_E_GROBE_R_001,
	XFILE_INDEX_E_LEG_L_001,
	XFILE_INDEX_E_LEG_R_001,
	XFILE_INDEX_E_DMGBODY_001,

	//�����O
	XFILE_INDEX_RING,
	//��
	XFILE_INDEX_MOON,
	//��s�@
	XFILE_INDEX_AIRPLANE,
	//UFO
	XFILE_INDEX_UFO,

	XFILE_INDEX_MAX,

};

//XFile���w�肵�ēǂݍ��ފ֐�
//�����P�FXMODEL�FXfile��ǂݍ��ޕϐ��̃A�h���X
//�����Q�FXFILE_INDEX�F�ǂݍ���Xfile�̃C���f�b�N�X
HRESULT LoadXFile(XMODEL *model, XFILE_INDEX index);

//�e��s���Z�߂Čv�Z���Ă������
//�����P�FXMODEL�F�Ώۂ�XMODEL�ϐ��̃A�h���X
//�����Q�FD3DXMATRIXA16�F�v�Z���ʂ����郏�[���h�}�g���N�X
D3DXMATRIXA16 *MakeWorldMatrix(XMODEL *model, D3DXMATRIXA16 *mtxWorld);

