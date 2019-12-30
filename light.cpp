
#include "light.h"
#include "myDirect3D.h"
#include "input.h"
#include "debugproc.h"
//=============================================================================
// �}�N����`
//=============================================================================
#define	NUM_LIGHT		(3)		// ���C�g�̐�

//=============================================================================
// �O���[�o���ϐ�
//=============================================================================
D3DLIGHT9 g_aLight[NUM_LIGHT];		// ���C�g���

int R = 0;
int G = 0;
int B = 0;
//=============================================================================
// ���C�g�̏���������
//=============================================================================
void InitLight(void)
{
	//Direct3DDevice�̎擾
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice(); 
	D3DXVECTOR3 vecDir;		//���C�g�̕���

	// D3DLIGHT9�\���̂�0�ŃN���A����
	ZeroMemory(&g_aLight[0], sizeof(D3DLIGHT9));

	
	//==========================================================================
	/*
	// ���C�g�̃^�C�v�̐ݒ�(POINT��Position����S�����Ɍ����΂��j
	// ���̌����ˑS�����@�������ˋ����iRange)��2�� 
	g_aLight[0].Type = D3DLIGHT_POINT;

	// ���C�g�̈ʒu
	g_aLight[0].Position = D3DXVECTOR3(0.0f, 49.0f, 50.0f);
	// �g�U��
	g_aLight[0].Diffuse = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);
	// ����
	g_aLight[0].Ambient = D3DXCOLOR(0.3f, 0.3f, 0.3f, 0.0f);
	// ������(�������قǌ��������Ȃ�j
	g_aLight[0].Attenuation0 = 0.5f;

	D3DXVec3Normalize((D3DXVECTOR3*)&g_aLight[0].Direction, &vecDir);

	// ���C�g�͈�
	g_aLight[0].Range = 200.0f;
	// ���C�g�������_�����O�p�C�v���C���ɐݒ�
	pDevice->SetLight(0, &g_aLight[0]);
	// ���C�g��L����
	pDevice->LightEnable(0, TRUE);
	// ���C�e�B���O���[�h�L��
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	*/
	/*
	//==========================================================================

	// D3DLIGHT9�\���̂�0�ŃN���A����
	ZeroMemory(&g_aLight[1], sizeof(D3DLIGHT9));

	// ���C�g�̃^�C�v�̐ݒ�
	g_aLight[1].Type = D3DLIGHT_SPOT;

	// ���C�g�̈ʒu
	g_aLight[1].Position = D3DXVECTOR3(0.0f, 20.0f, 0.0f);
	// �g�U��
	g_aLight[1].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	// ����
	g_aLight[1].Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);

	// ���C�g�̕����̐ݒ�
	vecDir = D3DXVECTOR3(0.0f, -1.0f, 0.0f);

	D3DXVec3Normalize((D3DXVECTOR3*)&g_aLight[1].Direction, &vecDir);

	// �����̗L������
	g_aLight[1].Range = 50.0f;
	
	// �t�H�[���I�t�i�~���̓�������O���Ɍ������ċN������̌����j�A
	g_aLight[1].Falloff = 0.5f;

	// ������(�������قǌ��������Ȃ�j
	g_aLight[1].Attenuation0 = 0.2f;  
	
	// �����̃R�[���̊p�x
	g_aLight[1].Theta = D3DXToRadian(40.0f);
	// �O���̃R�[���̊p�x
	g_aLight[1].Phi = D3DXToRadian(60.0f);

	// ���C�g�������_�����O�p�C�v���C���ɐݒ�
	pDevice->SetLight(1, &g_aLight[1]);
	// ���C�g1��L����
	//pDevice->LightEnable(1, TRUE);
	// ���C�e�B���O���[�h�L��
	//pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	*/
	////==========================================================================

	
	// D3DLIGHT9�\���̂�0�ŃN���A����
	ZeroMemory(&g_aLight[2], sizeof(D3DLIGHT9));

	// ���C�g2�̃^�C�v�̐ݒ�
	g_aLight[2].Type = D3DLIGHT_DIRECTIONAL;

	// ���C�g2�̊g�U���̐ݒ�
	g_aLight[2].Diffuse = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);

	// ���C�g2�̋��ʔ��ˌ��̐ݒ�
	g_aLight[2].Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);

	// ���C�g2�̕����̐ݒ�
	vecDir = D3DXVECTOR3(0.80f, 0.10f, 0.40f);
	D3DXVec3Normalize((D3DXVECTOR3*)&g_aLight[2].Direction, &vecDir);

	// ���C�g2�������_�����O�p�C�v���C���ɐݒ�
	pDevice->SetLight(2, &g_aLight[2]);

	// ���C�g2��L����
	pDevice->LightEnable(2, TRUE);

	// ���C�e�B���O���[�h�L��
	pDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_RGBA(255, 255, 255, 255));
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	
	/*
	g_aLight[1].Type = D3DLIGHT_POINT;

	// ���C�g�̈ʒu
	g_aLight[1].Position = D3DXVECTOR3(0.0f, 21.0f, 0.0f);
	// �g�U��
	g_aLight[1].Diffuse = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);
	// ����
	g_aLight[1].Ambient = D3DXCOLOR(0.3f, 0.3f, 0.3f, 0.0f);
	// ������(�������قǌ��������Ȃ�j
	g_aLight[1].Attenuation0 = 0.7f;

	D3DXVec3Normalize((D3DXVECTOR3*)&g_aLight[0].Direction, &vecDir);

	// ���C�g�͈�
	g_aLight[1].Range = 200.0f;
	// ���C�g�������_�����O�p�C�v���C���ɐݒ�
	pDevice->SetLight(0, &g_aLight[1]);

	// ���C�g��L����
	pDevice->LightEnable(0, TRUE);
	// ���C�e�B���O���[�h�L��
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	*/
}
//=============================================================================
// ���C�g�̏I������
//=============================================================================
void UninitLight(void)
{
}
//=============================================================================
// ���C�g�̍X�V����
//=============================================================================
void UpdateLight(void)
{
	/*
	//Direct3DDevice�̎擾
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();

	if (Keyboard_IsPress(DIK_T))
	{
		g_aLight[0].Diffuse.r += 0.02f;
		g_aLight[0].Diffuse.g += 0.02f;
		g_aLight[0].Diffuse.b += 0.02f;
	}
	if (Keyboard_IsPress(DIK_Y))
	{
		g_aLight[0].Diffuse.r -= 0.02f;
		g_aLight[0].Diffuse.g -= 0.02f;
		g_aLight[0].Diffuse.b -= 0.02f;
	}
	if (Keyboard_IsPress(DIK_4))
	{
		g_aLight[0].Range += 1.0f;
	}
	if (Keyboard_IsPress(DIK_5))
	{
		g_aLight[0].Range -= 1.0f;
	}
	if (Keyboard_IsPress(DIK_K))
	{
		R++;
		G++;
		B++;
		pDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_RGBA(R, G, B, 255));
	}
	if (Keyboard_IsPress(DIK_L))
	{
		R--;
		G--;
		B--;
		pDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_RGBA(R, G, B, 255));
	}
	
	DebugProc_Print((char *)"�����_:%f\n", g_aLight[0].Attenuation0);
	DebugProc_Print((char *)"���͈̔�:%f\n", g_aLight[0].Position.y);
	DebugProc_Print((char *)"���ʔ��ˌ�:%d,%d,%d\n", R,G,B);
	
	// ���C�g�������_�����O�p�C�v���C���ɐݒ�
	pDevice->SetLight(0, &g_aLight[0]);
	*/
}

