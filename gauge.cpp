#include "main.h"
#include "texture.h"
#include "result.h"
#include "sprite.h"
#include "input.h"
#include "field.h"
#include "enemy.h"
#include <math.h>
#include "scene.h"
#include "score.h"
#include "camera.h"
#include "player.h"
#include "Ranking.h"
#include "gauge.h"
#include "debugproc.h"

int Gauge_V; //ゲージの内枠のV値
int GaugeRange; //ゲージのどのくらい描画するか
int GaugeAddCnt;   //ゲージを100％中何%増やすか
int GaugeReduceCnt;   //ゲージを100％中何%減らすか
int g_GaugeMaxAlpha;
bool g_GaugeMaxFlg;
int g_GaugeCnt;

void InitGauge(void)
{
	//初期化
	Gauge_V = 1024; //一番下
	GaugeRange = 0;
	GaugeAddCnt = 0;
	GaugeReduceCnt = 0;
	g_GaugeMaxAlpha = 130;
	g_GaugeMaxFlg = false;
	g_GaugeCnt = 0;

}

void UninitGauge(void)
{
}

void UpdateGauge(void)
{

	//増やす
	if (Keyboard_IsTrigger(DIK_7)) {
		AddGauge(10);
		/*GaugeRange += 1024 / 100;
		Gauge_V -= 1024 / 100;*/
	}
		//増やす
	if (Keyboard_IsTrigger(DIK_9)) {
		AddGauge(100);
		/*GaugeRange += 1024 / 100;
		Gauge_V -= 1024 / 100;*/
	}
	//減らす
	if (Keyboard_IsTrigger(DIK_8)) {
		ReduceGauge(10);
		//GaugeRange -= 1024 / 100;
		//Gauge_V += 1024 / 100;
	}

	//増やす
	if (GaugeAddCnt > 0) {
		g_GaugeCnt++;

		GaugeRange += 1024 / 100;
		Gauge_V -= 1024 / 100;
		GaugeAddCnt--;
	}

	//減らす
	if (GaugeReduceCnt > 0) {
		g_GaugeCnt--;
		GaugeRange -= 1024 / 100;
		Gauge_V += 1024 / 100;
		GaugeReduceCnt--;
	}


	if (GaugeAddCnt <= 0) {
		GaugeAddCnt = 0;
	}

	if (GaugeReduceCnt <= 0) {
		GaugeReduceCnt = 0;
	}

	//最大
	if (GaugeRange > 1024) {
		GaugeRange = 1024;
		Gauge_V = 0;
	}
	//最小
	else if(GaugeRange <= 0) {
		GaugeRange = 0;
		Gauge_V = 1024;
	}

	if (g_GaugeCnt >= 100) {
		g_GaugeMaxFlg = true;
	}
	//ゲージが最大になった時
	if (g_GaugeMaxFlg == true) {
		g_GaugeMaxAlpha = 255;
	}
	DebugProc_Print((char*)"ゲージ：%f\n", GaugeRange);

}

void DrawGauge(void)
{

	//ゲージの外枠
	Sprite_Draw
	(
		TEXTURE_INDEX_GAUGE_OUTSIDE,
		1100,
		100,
		0,
		0,
		241,
		1024,
		0,
		0,
		0.5f,
		0.5f,
		0.0f,
		130
	);

	//ゲージの内枠
	Sprite_Draw
	(
		TEXTURE_INDEX_GAUGE_INSIDE,
		1098,
		1123,
		0,
		Gauge_V,
		241,
		GaugeRange,
		0,
		1024 + GaugeRange,
		0.5f,
		0.5f,
		0.0f,
		255
	);

	//ゲージの上
	Sprite_Draw
	(
		TEXTURE_INDEX_GAUGE_CIRCLE,
		1099,
		97,
		0,
		0,
		241,
		1024,
		0,
		0,
		0.5f,
		0.5f,
		0.0f,
		g_GaugeMaxAlpha
	);
}

//ゲージを増やす関数、引数は何パーセント増やすか
void AddGauge(int Percent) {
	GaugeAddCnt = Percent;
}

//ゲージを減らすか関数、引数は何パーセント減らすか
void ReduceGauge(int Percent) {
	GaugeReduceCnt = Percent;
}

int GetGauge() {
	return g_GaugeCnt;
}
