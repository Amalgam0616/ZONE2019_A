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
#include "particle.h"

float g_TitleGlovePos;
int TitleGloveMoveCnt;
bool g_TitlePunchEffectFlg;
bool g_BigEnter;

void InitTitle(void)
{
	//初期化
	g_TitleGlovePos = 1220.0f;
	TitleGloveMoveCnt = 0;
	g_TitlePunchEffectFlg = false;

	InitRing();
	InitParticle();
	Init_TitleCamera();
	PlaySound(SOUND_LABEL_SE_HIT);
	g_BigEnter = false;

}

void UninitTitle(void)
{
	UninitRing();
	UninitParticle();

}

void UpdateTitle(void)
{
	UpdateRing();
	TitleGloveMoveCnt++;

	UpdateParticle();

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
	 
	//でかいエンターで入力
	if (Keyboard_IsTrigger(DIK_NUMPADENTER))
	{
		SetScene(SCENE_INDEX_GAME);
		g_BigEnter = true;
	}

	//キーボードで入力
	if (Keyboard_IsTrigger(DIK_RETURN))
	{
		SetScene(SCENE_INDEX_GAME);
		g_BigEnter = false;
	}
}

void DrawTitle(void)
{
	DrawRing();
	//タイトル背景
	//Sprite_Draw(TEXTURE_INDEX_TITLE_BG, 0.0f, 0.0f);
	DrawParticle();
	//タイトルロゴ
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
	//シーン遷移の指示
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

	//シーンチェンジの指示の絵の表示
	//パンチングマシーン
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
		//パンチングマシーンを殴ってるエフェクト
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
	//グローブ
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

bool GetBigEnter(){
	return g_BigEnter;
}