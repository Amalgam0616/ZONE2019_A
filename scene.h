#pragma once

#include "main.h"

//==============================
//   �֐��錾
//==============================
void InitScene(void);		//������
void UninitScene(void);		//�I��
void UpdateScene(void);		//�X�V
void DrawScene(void);		//�`��

void SetScene(SCENE_INDEX);		//�����Ɉړ���̃V�[�����w�肵�ăV�[���J��
SCENE_INDEX GetScene();			//���݂̃V�[���̎擾