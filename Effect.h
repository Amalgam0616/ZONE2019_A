#pragma once

#include "main.h"
#include "texture.h"
//*****************************************************************************
// プロトタイプ宣言
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

//エフェクトの種類、元画像の横に何個あるか、元画像の縦に何個あるか、表示するX位置、表示するY位置ｍ表示するZ位置、X方向のサイズ、Y方向のサイズ
void PlayEffect(TextureIndex EffectNum, int xNum, int yNum, float EffectX, float EffectY, float EffectZ, float SizeX, float SizeY);