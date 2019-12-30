#include "main.h"
#include "texture.h"
#include "Title.h"
#include "sprite.h"
#include "input.h"
#include "Ring.h"
#include <math.h>
#include "camera.h"
#include "scene.h"
#include "sound.h"

float g_TitleGlovePos;
int TitleGloveMoveCnt;
bool g_TitlePunchEffectFlg;


void InitTitle(void)
{
	//������
	g_TitleGlovePos = 1220.0f;
	TitleGloveMoveCnt = 0;
	g_TitlePunchEffectFlg = false;

	InitRing();
	Init_TitleCamera();
	PlaySound(SOUND_LABEL_SE_HIT);
}

void UninitTitle(void)
{
	UninitRing();
}

void UpdateTitle(void)
{
	UpdateRing();
	TitleGloveMoveCnt++;

	if (TitleGloveMoveCnt >= 40) {
		if (g_TitlePunchEffectFlg == false) {
			g_TitleGlovePos = 1170.0f;
			g_TitlePunchEffectFlg = true;
			TitleGloveMoveCnt = 0;
		}
		else if (g_TitlePunchEffectFlg == true) {
			g_TitleGlovePos = 1220.0f;
			g_TitlePunchEffectFlg = false;
			TitleGloveMoveCnt = 0;
		}
	}
	 
	if (Keyboard_IsTrigger(DIK_NUMPADENTER))
	{
		SetScene(SCENE_INDEX_GAME);
	}

	if (Keyboard_IsTrigger(DIK_RETURN))
	{
		SetScene(SCENE_INDEX_GAME);
	}
}

void DrawTitle(void)
{
	DrawRing();
	//�^�C�g���w�i
	//Sprite_Draw(TEXTURE_INDEX_TITLE_BG, 0.0f, 0.0f);

	//�^�C�g�����S
	Sprite_Draw(TEXTURE_INDEX_TITLE_LOGO,
		SCREEN_WIDTH / 2,
		300.0f,
		1024,
		724,
		1024,
		724,
		1024 / 2,
		724 / 2,
		0.8f,
		0.8f,
		0.0f,
		255);

	//Sprite_Draw(TEXTURE_INDEX_TITLE_LOGO, (SCREEN_WIDTH - 838) / 2, 200.0f);
	//�V�[���J�ڂ̎w��
	Sprite_Draw(TEXTURE_INDEX_GAME_SCENE_CHANGE,
		950.0f,
		650.0f,
		0,
		0,
		500,
		300,
		500 / 2,
		300 / 2,
		0.5f,
		0.4f,
		0.0f,
		255);

	//�V�[���`�F���W�̎w���̊G�̕\��
	//�p���`���O�}�V�[��
	Sprite_Draw(TEXTURE_INDEX_PUNCHINGMACHINE,
		1120.0f,
		650.0f,
		0,
		0,
		300,
		400,
		300 / 2,
		400 / 2,
		0.3f,
		0.3f,
		0.0f,
		255);

	if (g_TitlePunchEffectFlg == true) {
		//�p���`���O�}�V�[���������Ă�G�t�F�N�g
		Sprite_Draw(TEXTURE_INDEX_SCENE_CHANGE_EFFECT,
			1130.0f,
			625.0f,
			0,
			0,
			300,
			300,
			300 / 2,
			300 / 2,
			0.4f,
			0.4f,
			0.0f,
			255);
	}
	//�O���[�u
	Sprite_Draw(TEXTURE_INDEX_GLOVE,
		g_TitleGlovePos,
		625.0f,
		0,
		0,
		300,
		300,
		300 / 2,
		300 / 2,
		0.3f,
		0.3f,
		0.0f,
		255);

}