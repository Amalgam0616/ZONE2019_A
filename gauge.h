#pragma once

#include "main.h"

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void InitGauge(void);
void UninitGauge(void);
void UpdateGauge(void);
void DrawGauge(void);

//�Q�[�W�𑝂₷�֐��A�����͉��p�[�Z���g���₷��
void AddGauge(int Percent);
//�Q�[�W�����炷���֐��A�����͉��p�[�Z���g���炷��
void ReduceGauge(int Percent);

int GetGauge();