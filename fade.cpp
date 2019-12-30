#include "main.h"
#include "myDirect3D.h"
#include "fade.h"
#include "scene.h"

//==============================
//   �t�F�[�h�\����
//==============================
typedef struct FadeVertex_tag
{
	D3DXVECTOR4 position;
	D3DCOLOR color;
} FadeVertex;
#define FVF_FADE_VERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)


//==============================
//   �O���[�o���ϐ��錾
//==============================
static FADE_STATE g_FadeState;							//�t�F�[�h�̏��
static SCENE_INDEX g_FadeNextScene;						//���̃V�[���̕ێ�
static float g_FadeAlpha;								//�t�F�[�h�̃A���t�@�l
static D3DXCOLOR g_FadeColor(0.0f, 0.0f, 0.0f, 1.0f);	//�t�F�[�h�̐F

//==============================
//  �t�F�[�h����������
//==============================
void InitFade(void)
{
	g_FadeState = FADE_STATE_NONE;		//��Ԃ���ɂ���
	g_FadeNextScene = SCENE_INDEX_NONE;	//���̃V�[���̕ێ���Ԃ���ɂ���
	g_FadeAlpha = 0.0f;					//�A���t�@���O�ɂ���
}
//==============================
//  �t�F�[�h�I������
//==============================
void UninitFade(void)
{
}
//==============================
//  �t�F�[�h�X�V����
//==============================
void UpdateFade(void)
{
	//�t�F�[�h�A�E�g�̂Ƃ��͓��������ɂȂ�
	//SetFade(SCENE_INDEX Scene)�֐����Ăяo������g_FadeState��FADE_STATE_OUT�ɂȂ�
	if (g_FadeState == FADE_STATE_OUT) 
	{
		if (g_FadeAlpha >= 1.0f) 
		{
			g_FadeAlpha = 1.0f;
			g_FadeState = FADE_STATE_IN;
			SetScene(g_FadeNextScene);
		}
		g_FadeAlpha += 1.0f / 20.0f;
	}
	//�t�F�[�h�C���̂Ƃ��͍��������ɂȂ�
	else if (g_FadeState == FADE_STATE_IN) 
	{
		if (g_FadeAlpha <= 0.0f) 
		{
			g_FadeAlpha = 0.0f;
			g_FadeState = FADE_STATE_NONE;
		}
		g_FadeAlpha -= 1.0f / 20.0f;
	}
}
//==============================
//  �t�F�[�h�`�揈��
//==============================
void DrawFade(void)
{
	//�����t�F�[�h��ԂłȂ��ꍇ�͂�����DrawFade�̏������I��点��
	if (g_FadeState == FADE_STATE_NONE) 
	{
		return;
	}

	g_FadeColor.a = g_FadeAlpha;
	D3DCOLOR c = g_FadeColor;

	FadeVertex v[] = 
	{
		{ D3DXVECTOR4(        0.0f,          0.0f, 0.0f, 1.0f), c }, 
		{ D3DXVECTOR4(SCREEN_WIDTH,          0.0f, 0.0f, 1.0f), c }, 
		{ D3DXVECTOR4(        0.0f, SCREEN_HEIGHT, 0.0f, 1.0f), c }, 
		{ D3DXVECTOR4(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 1.0f), c }, 
	};

	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();

	pDevice->SetFVF(FVF_FADE_VERTEX);
	pDevice->SetTexture(0, NULL);
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(FadeVertex));
}
//==============================
//  �t�F�[�h�̃Z�b�g
//==============================
void SetFade(SCENE_INDEX Scene) 
{
	g_FadeNextScene = Scene;
	g_FadeState = FADE_STATE_OUT;
}
//==============================
//  ���݂̃t�F�[�h��Ԃ̎擾
//==============================
FADE_STATE GetFadeState() 
{
	return g_FadeState;
}