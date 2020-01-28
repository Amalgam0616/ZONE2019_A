#define _CRT_SECURE_NO_WARNINGS
#include "main.h"
#include "player.h"
#include "camera.h"
#include "scene.h"
#include "texture.h"
#include "Ranking.h"
#include "sprite.h"
#include "input.h"
#include "stdio.h"
#include "math.h"
#include "score.h"
#include "debugproc.h"
#include "result.h"

#define MAX_RANKING	3	//スコア上位３つまで

//ランキングの構造体
typedef struct
{
	int high_score[MAX_RANKING];
	int PlayerScore;
}SCOREDATA;

//グローバル変数
SCOREDATA rank;
int TitleChangeCnt;
int RankingDrawCnt;
int MyScoreMove;

float g_RankingGlovePos;
int RankingGloveMoveCnt;
bool g_RankingPunchEffectFlg;
bool g_RankingStartFlg;


void InitRanking(void)
{
	rank.PlayerScore = GetScore();
	TitleChangeCnt = 0;
	RankingDrawCnt = 0;
	MyScoreMove = 0;
	g_RankingStartFlg = true;

	g_RankingGlovePos = 1220.0f;
	RankingGloveMoveCnt = 0;
	g_RankingPunchEffectFlg = false;

	FILE *fp;	/* ファイルポインタの宣言 */

	//【初期化の処理】
	//ファイルからデータを読み込んで
	//rankに入れる

	/*ファイルのオープン */
	if ((fp = fopen("asset/text/Score.txt", "rb+")) == NULL) {
		MessageBox(NULL, "txtファイルが読み込めません", "エラー", MB_OK);
		exit(EXIT_FAILURE);	/*エラーの場合は通常、異常終了する */
	}


	/*ファイルの読みこみ*/
	fread(&rank.high_score, sizeof(int), 3, fp);

	////スコアのソート
	SetRank(GetScore());

	//ファイルポインタを先頭に持ってくる
	fseek(fp, 0L, SEEK_SET);

	//【終了の処理】
	//rankのデータをファイルに保存する
	fwrite(rank.high_score, sizeof(int), 3, fp);

	/*ファイルのクローズ*/
	fclose(fp);
}

void UninitRanking(void)
{
	
}

void UpdateRanking(void)
{
	rank.high_score[0] = 999999;
	rank.high_score[1] = 999999;
	rank.high_score[2] = 999999;

	if (g_RankingStartFlg == true) {
		DebugProc_Print((char*)"スコア：%d", GetScore());
		//シーン遷移の秒数を数える
		TitleChangeCnt++;

		//シーン遷移
		//シーン遷移の絵
		RankingGloveMoveCnt++;

		if (RankingGloveMoveCnt >= 40) {
			if (g_RankingPunchEffectFlg == false) {
				g_RankingGlovePos = 1200.0f;
				g_RankingPunchEffectFlg = true;
				RankingGloveMoveCnt = 0;
			}
			else if (g_RankingPunchEffectFlg == true) {
				g_RankingGlovePos = 1230.0f;
				g_RankingPunchEffectFlg = false;
				RankingGloveMoveCnt = 0;
			}
		}

		//でかいエンターを押したら
		//5秒たったら
		if (Keyboard_IsTrigger(DIK_NUMPADENTER) ||
			TitleChangeCnt >= 600)
		{
			InitCamera();
			InitPlayer();
			SetScene(SCENE_INDEX_TITLE);
		}
		//スペースを押したら
		//5秒たったら
		else if (Keyboard_IsTrigger(DIK_SPACE) ||
			TitleChangeCnt >= 600)
		{
			InitCamera();
			InitPlayer();
			SetScene(SCENE_INDEX_TITLE);
		}
	}
}

void DrawRanking(void)
{


	if (RankingDrawCnt < 254 && MyScoreMove >= 330) {
		RankingDrawCnt += 2;
	}
	if (MyScoreMove <= 330){
		MyScoreMove += 3;
	}
	//背景
	Sprite_Draw
	(
		TEXTURE_INDEX_RANKING_BG,
		0,
		0,
		0,
		0,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		0,
		0,
		1.0f,
		1.0f,
		0.0f,
		RankingDrawCnt
	);

	//シーン遷移
	//Sprite_Draw(TEXTURE_INDEX_SKIP_SCENE_CHANGE, 0, 0);
	Sprite_Draw(TEXTURE_INDEX_SKIP_SCENE_CHANGE,
		1065.0f,
		50.0f,
		0,
		0,
		500,
		300,
		500 / 2,
		300 / 2,
		0.3f,
		0.2f,
		0.0f,
		RankingDrawCnt);

	//シーンチェンジの指示の絵の表示
	//パンチングマシーン
	Sprite_Draw(TEXTURE_INDEX_PUNCHINGMACHINE,
		1170.0f,
		50.0f,
		0,
		0,
		300,
		400,
		300 / 2,
		400 / 2,
		0.2f,
		0.2f,
		0.0f,
		RankingDrawCnt);

	if (g_RankingPunchEffectFlg == true) {
		//パンチングマシーンを殴ってるエフェクト
		Sprite_Draw(TEXTURE_INDEX_SCENE_CHANGE_EFFECT,
			1180.0f,
			25.0f,
			0,
			0,
			300,
			300,
			300 / 2,
			300 / 2,
			0.3f,
			0.3f,
			0.0f,
			RankingDrawCnt);
	}
	//グローブ
	Sprite_Draw(TEXTURE_INDEX_GLOVE,
		g_RankingGlovePos,
		25.0f,
		0,
		0,
		300,
		300,
		300 / 2,
		300 / 2,
		0.2f,
		0.2f,
		0.0f,
		RankingDrawCnt);

	//スコア描画（X位置Y位置　数値　桁数 拡大率X　拡大率Y  桁ごとのオフセット）
	//============================================
	//1位のスコア
	//============================================	
	//金表彰台
	Sprite_Draw
	(
		TEXTURE_INDEX_GOLDTABLE,
		SCREEN_WIDTH / 2,
		SCREEN_HEIGHT / 2 + 245,
		0,
		0,
		256,
		1357,
		256 / 2,
		1357 / 2,
		0.8f,
		0.5f,
		0.0f,
		RankingDrawCnt
	);
	DrawScore(TEXTURE_INDEX_GOLDNUMBER, 455, 210, rank.high_score[0], 6, 0.25f, 0.25f, 60, RankingDrawCnt);

	//============================================
	//2位のスコア
	//============================================
	//スコアが1位と同じでない場合
	if (rank.high_score[1] != rank.high_score[0]) {
		//銀表彰台ランク
		Sprite_Draw
		(
			TEXTURE_INDEX_SILVERTABLE,
			250,
			SCREEN_HEIGHT / 2 + 375,
			0,
			0,
			256,
			1357,
			256 / 2,
			1357 / 2,
			0.8f,
			0.5f,
			0.0f,
			RankingDrawCnt
		);
		DrawScore(TEXTURE_INDEX_SILVERNUMBER, 70, 340, rank.high_score[1], 6, 0.25f, 0.25f, 60, RankingDrawCnt);
	}
	//スコアが1位と同じ場合
	else {
		Sprite_Draw
		(
			TEXTURE_INDEX_GOLDTABLE,
			250,
			SCREEN_HEIGHT / 2 + 245,
			0,
			0,
			256,
			1357,
			256 / 2,
			1357 / 2,
			0.8f,
			0.5f,
			0.0f,
			RankingDrawCnt
		);
		DrawScore(TEXTURE_INDEX_GOLDNUMBER, 70, 210, rank.high_score[1], 6, 0.25f, 0.25f, 60, RankingDrawCnt);

	}

	//============================================
	//3位のスコア
	//============================================
	//スコアが1位と同じ場合
	if (rank.high_score[2] == rank.high_score[0]) {
		//銅表彰台ランク
		Sprite_Draw
		(
			TEXTURE_INDEX_GOLDTABLE,
			SCREEN_WIDTH / 4 * 3 + 60,
			SCREEN_HEIGHT / 2 + 245,
			0,
			0,
			256,
			1357,
			256 / 2,
			1357 / 2,
			0.8f,
			0.5f,
			0.0f,
			RankingDrawCnt
		);
		DrawScore(TEXTURE_INDEX_GOLDNUMBER, 840, 210, rank.high_score[2], 6, 0.25f, 0.25f, 60, RankingDrawCnt);

	}
	//スコアが2位と同じ場合
	else if (rank.high_score[2] == rank.high_score[1]) {
		//銅表彰台ランク
		Sprite_Draw
		(
			TEXTURE_INDEX_SILVERTABLE,
			SCREEN_WIDTH / 4 * 3 + 60,
			SCREEN_HEIGHT / 2 + 375,
			0,
			0,
			256,
			1357,
			256 / 2,
			1357 / 2,
			0.8f,
			0.5f,
			0.0f,
			RankingDrawCnt
		);
		DrawScore(TEXTURE_INDEX_SILVERNUMBER, 840, 340, rank.high_score[2], 6, 0.25f, 0.25f, 60, RankingDrawCnt);

	}
	//スコアが3位の場合
	else {
		//銅表彰台ランク
		Sprite_Draw
		(
			TEXTURE_INDEX_BRONZETABLE,
			SCREEN_WIDTH / 4 * 3 + 60,
			SCREEN_HEIGHT / 2 + 475,
			0,
			0,
			256,
			1357,
			256 / 2,
			1357 / 2,
			0.8f,
			0.5f,
			0.0f,
			RankingDrawCnt
		);
		DrawScore(TEXTURE_INDEX_BLONZENUMBER, 840, 440, rank.high_score[2], 6, 0.25f, 0.25f, 60, RankingDrawCnt);

	}

	//============================================
	//自分のスコア
	//============================================	
	//銅メダル級テクスチャ
	if (GetScore() < RANKING_BORDER1)
	{
		//スコア（X位置Y位置　数値　桁数 拡大率X　拡大率Y  桁ごとのオフセット）
		DrawScore(TEXTURE_INDEX_BLONZENUMBER, 320.0f, SCREEN_HEIGHT / 2 - MyScoreMove, GetScore(), 6, 0.4f, 0.4f, 105.0f,255);
	}
	//銀メダル級テクスチャ
	else if (GetScore() >= RANKING_BORDER1 && GetScore() < RANKING_BORDER2)
	{
		DrawScore(TEXTURE_INDEX_SILVERNUMBER, 320.0f, SCREEN_HEIGHT / 2 - MyScoreMove, GetScore(), 6, 0.4f, 0.4f, 105.0f, 255);
	}
	//金メダル級テクスチャ
	else if (GetScore() >= RANKING_BORDER2)
	{
		DrawScore(TEXTURE_INDEX_GOLDNUMBER, 320.0f, SCREEN_HEIGHT / 2 - MyScoreMove, GetScore(), 6, 0.4f, 0.4f, 105.0f, 255);
	}

}


void SetRank(int newscore)
{	//【記録の更新処理】
	int i, j, tmp;

	//rankのデータと比較して新しい記録の方が
	//大きければ記録を更新する
	for (i = 0; i < MAX_RANKING; i++)
	{
		if (rank.high_score[i] < newscore)
		{
			rank.high_score[i] = newscore;

			break;
		}

	}

	//ソート（並び替え）処理を入れる
	for (i = 0; i < MAX_RANKING; i++)
	{
		for (j = i + 1; j <  MAX_RANKING; j++)
		{//上から下への操作

			if (rank.high_score[i] < rank.high_score[j])
			{
				tmp = rank.high_score[i];

				rank.high_score[i] = rank.high_score[j];

				rank.high_score[j] = tmp;

			}
		}
	}


}

//デバッグ
//ランキングにするだけの関数
void SetRanking_true()
{
	g_RankingStartFlg = true;
}