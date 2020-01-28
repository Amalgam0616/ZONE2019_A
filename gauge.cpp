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

int Gauge_V; //�Q�[�W�̓��g��V�l
int GaugeRange; //�Q�[�W�̂ǂ̂��炢�`�悷�邩
int GaugeAddCnt;   //�Q�[�W��100������%���₷��
int GaugeReduceCnt;   //�Q�[�W��100������%���炷��
int g_GaugeMaxAlpha;
bool g_GaugeMaxFlg;
int g_GaugeCnt;

void InitGauge(void)
{
	//������
	Gauge_V = 1024; //��ԉ�
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

	//���₷
	if (Keyboard_IsTrigger(DIK_7)) {
		AddGauge(10);
		/*GaugeRange += 1024 / 100;
		Gauge_V -= 1024 / 100;*/
	}
		//���₷
	if (Keyboard_IsTrigger(DIK_9)) {
		AddGauge(100);
		/*GaugeRange += 1024 / 100;
		Gauge_V -= 1024 / 100;*/
	}
	//���炷
	if (Keyboard_IsTrigger(DIK_8)) {
		ReduceGauge(10);
		//GaugeRange -= 1024 / 100;
		//Gauge_V += 1024 / 100;
	}

	//���₷
	if (GaugeAddCnt > 0) {
		g_GaugeCnt++;

		GaugeRange += 1024 / 100;
		Gauge_V -= 1024 / 100;
		GaugeAddCnt--;
	}

	//���炷
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

	//�ő�
	if (GaugeRange > 1024) {
		GaugeRange = 1024;
		Gauge_V = 0;
	}
	//�ŏ�
	else if(GaugeRange <= 0) {
		GaugeRange = 0;
		Gauge_V = 1024;
	}

	if (g_GaugeCnt >= 100) {
		g_GaugeMaxFlg = true;
	}
	//�Q�[�W���ő�ɂȂ�����
	if (g_GaugeMaxFlg == true) {
		g_GaugeMaxAlpha = 255;
	}
	DebugProc_Print((char*)"�Q�[�W�F%f\n", GaugeRange);

}

void DrawGauge(void)
{

	//�Q�[�W�̊O�g
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

	//�Q�[�W�̓��g
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

	//�Q�[�W�̏�
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

//�Q�[�W�𑝂₷�֐��A�����͉��p�[�Z���g���₷��
void AddGauge(int Percent) {
	GaugeAddCnt = Percent;
}

//�Q�[�W�����炷���֐��A�����͉��p�[�Z���g���炷��
void ReduceGauge(int Percent) {
	GaugeReduceCnt = Percent;
}

int GetGauge() {
	return g_GaugeCnt;
}
