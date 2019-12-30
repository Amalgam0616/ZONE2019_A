#pragma once

#include <d3d9.h>
#include "scene.h"

//====================================
//  �t�F�[�h��`(enum)
//====================================
enum FADE_STATE 
{
	FADE_STATE_NONE = 0,
	FADE_STATE_IN,			//�t�F�[�h�C��
	FADE_STATE_OUT			//�t�F�[�h�A�E�g
};

//==============================
//   �֐��錾
//==============================
void InitFade(void);		//������
void UninitFade(void);		//���
void UpdateFade(void);			//�X�V
void DrawFade(void);			//�`��

void SetFade(SCENE_INDEX Scene);	//FadeIn,FadeOut���s�����̃V�[���Ɉڂ�B�����Ɉڂ�V�[����INDEX��^����
FADE_STATE GetFadeState();			//���݂�Fade�̏�Ԃ��擾����