
#pragma once

#include "main.h"

//========================================
//  �}�N����`
//========================================
#define	ROOF_X	(100.0f)						// �V��̕�
#define ROOF_Y	(670.0f)						// �V��̍���
#define ROOF_Z	(167.0f)						// �V���Z�l�i�G�l�~�[�̂Ԃ���ʒu�j

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitRoof(void);
void UninitRoof(void);
void UpdateRoof(void);
void DrawRoof(void);
int GetCnt();