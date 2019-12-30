#include "scene.h"
#include "Ranking.h"
#include "Title.h"
#include "score.h"
#include "game.h"
//====================================
//  グローバル変数宣言
//====================================
static SCENE_INDEX g_SceneIndex;

//====================================
//　シーン初期化処理
//====================================
void InitScene()
{
	//シーンを空にする
	g_SceneIndex = SCENE_INDEX_NONE;
}
//====================================
//　シーン終了処理
//====================================
void UninitScene(void)
{
	//各シーンごとにUninit関数を呼び出す
	switch (g_SceneIndex) 
	{
		case SCENE_INDEX_NONE:
			break;

		case SCENE_INDEX_TITLE:
			UninitTitle();
			break;

		case SCENE_INDEX_GAME:
			UninitGame();
			break;

		case SCENE_INDEX_RANKING:
			UninitRanking();
			break;
	}
}
//====================================
//　シーン更新処理
//====================================
void UpdateScene(void)
{
	//各シーンごとにUpdate関数を呼び出す
	switch (g_SceneIndex)
	{
		case SCENE_INDEX_NONE:
			break;

		case SCENE_INDEX_TITLE:
			UpdateTitle();
			break;

		case SCENE_INDEX_GAME:
			UpdateGame();
			break;

		case SCENE_INDEX_RANKING:
			UpdateRanking();
			break;
	}
}
//====================================
//　シーン描画処理
//====================================
void DrawScene(void)
{
	//各シーンごとにDraw関数を呼び出す
	switch (g_SceneIndex) 
	{
		case SCENE_INDEX_NONE:
			break;

		case SCENE_INDEX_TITLE:
			DrawTitle();
			break;

		case SCENE_INDEX_GAME:
			DrawGame();
			break;

		case SCENE_INDEX_RANKING:
			DrawRanking();
			break;
	}
}
//====================================
//　シーンのセット処理
//====================================
void SetScene(SCENE_INDEX scene)
{
	//前シーンの終了処理
	switch (g_SceneIndex) 
	{
		case SCENE_INDEX_NONE:
			break;

		case SCENE_INDEX_TITLE:
			UninitTitle();
			break;

		case SCENE_INDEX_GAME:
			UninitGame();
			break;

		case SCENE_INDEX_RANKING:
			UninitRanking();
			break;

		default:
			break;
	}

	//シーン切り替え
	g_SceneIndex = scene;

	//切り替えたシーンの初期化（各シーンごとにInit関数を呼び出す)
	switch (g_SceneIndex) 
	{
		case SCENE_INDEX_NONE:
			break;

		case SCENE_INDEX_TITLE:
			InitScore();
			InitTitle();
			break;

		case SCENE_INDEX_GAME:
			InitGame();
			break;

		case SCENE_INDEX_RANKING:
			InitRanking();
			break;

		default:
			break;
	}
}
//====================================
//　現在のシーンの取得
//====================================
SCENE_INDEX GetScene()
{
	return g_SceneIndex;
}
