#pragma once

#include "main.h"
#include "texture.h"
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEffect(void);
void UninitEffect(void);
void UpdateEffect(void);
void DrawEffect(void);

void SlowEffect();
void DamageEffect();
HRESULT MakeVertexChargeEffect(LPDIRECT3DDEVICE9 pDevice);
HRESULT MakeVertexWindEffect(LPDIRECT3DDEVICE9 pDevice);
HRESULT MakeVertexHitEffect(LPDIRECT3DDEVICE9 pDevice);


void Punch_Charge_Effect_Animation();
void Punch_Charge_Effect();

void Punch_Wind_Effect_Animation();
void Punch_Wind_Effect();

void Punch_Hit_Effect_Animation();
void Punch_Hit_Effect();

//�G�t�F�N�g�̎�ށA���摜�̉��ɉ����邩�A���摜�̏c�ɉ����邩�A�\������X�ʒu�A�\������Y�ʒu���\������Z�ʒu�AX�����̃T�C�Y�AY�����̃T�C�Y
void PlayEffect(TextureIndex EffectNum, int xNum, int yNum, float EffectX, float EffectY, float EffectZ, float SizeX, float SizeY);