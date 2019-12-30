#include "score.h"
#include "texture.h"
#include "sprite.h"
//========================
//  グローバル変数宣言
//========================
static unsigned int	g_Score;	//スコアを入れる

//========================
//  初期化処理
//========================
void InitScore(void)
{
	//スコアの初期化
	g_Score = 0;
}
//スコア描画
void DrawScore(TextureIndex texture_index,float x, float y, int score, int fig, float sx, float sy, float offset)
{
	// カンスト用の最大数値を作る
	int count_stop_score = 1;

	for (int i = 0; i < fig; i++)
	{
		count_stop_score *= 10;
	}

	//最大値の補正処理
	if (score >= count_stop_score)
	{
		score = count_stop_score - 1;
	}

	for (int i = 0; i < fig; i++)
	{
		int n = score % 10;
		score /= 10;

		Number_Draw(texture_index,x + offset * (fig - (i + 1)), y, n, sx, sy);
	}
}

//スコア描画(アルファ値あり)
void DrawScore(TextureIndex texture_index,float x, float y, int score, int fig, float sx, float sy, float offset,int alpha)
{
	// カンスト用の最大数値を作る
	int count_stop_score = 1;

	for (int i = 0; i < fig; i++)
	{
		count_stop_score *= 10;
	}

	//最大値の補正処理
	if (score >= count_stop_score)
	{
		score = count_stop_score - 1;
	}

	for (int i = 0; i < fig; i++)
	{
		int n = score % 10;
		score /= 10;

		Number_Draw_Alpha(texture_index,x + offset * (fig - (i + 1)), y, n, sx, sy, alpha);
	}
}


void Number_Draw(TextureIndex texture_index,float x, float y, int n, float sx, float sy)
{
	if (n < 0 || n > 9) return;	//スコアの数字分割

	Sprite_Draw
	(
		texture_index,
		x,
		y,
		NUMBER_WIDTH * n,
		0,
		NUMBER_WIDTH,
		NUMBER_HEIGHT,
		0,
		0,
		sx,
		sy,
		0.0f
	);

}

//α値あり版
void Number_Draw_Alpha(TextureIndex texture_index, float x, float y, int n, float sx, float sy, int alpha)
{
	if (n < 0 || n > 9) return;	//スコアの数字分割

	Sprite_Draw
	(
		texture_index,
		x,
		y,
		NUMBER_WIDTH * n,
		0,
		NUMBER_WIDTH,
		NUMBER_HEIGHT,
		0,
		0,
		sx,
		sy,
		0.0f,
		alpha
	);
}
//========================
//  スコアの足し算処理
//========================
void AddScore(int score)
{
	int add = g_Score + score;	//変数addに現在のスコア＋引数（足したいスコアの値）を入れる

	//もし足した後の値が0より大きい（正の値の場合）
	if (add > 0)
	{
		g_Score += score;
	}
	else
	{
		//もし足した後の値が０を下回る場合はマイナスの数字を表示しないようにスコアに０を設定する
		g_Score = 0;
	}
}
//========================
//  スコアのgetter
//========================
int GetScore(void)
{
	return g_Score;
}