#include "texture.h"
#include "player.h"
#include "enemy.h"
#include "score.h"
#include "main.h"
#include "sound.h"
#include "result.h"
#include "field.h"
#include "field_yoko.h"
#include "Roof.h"
#include "floor.h"
#include "Effect.h"
#include "camera.h"
#include "Ring.h"
#include "Ranking.h"
#include "gauge.h"

void InitGame(void)
{
	InitScore();

	InitField_Yoko();
	InitField();
	InitRoof();
	InitFloor();

	InitRing();
	InitEnemy();
	InitPlayer();
	Init_GameCamera();
	
	InitEffect();
	InitResult();
	InitGauge();
}

void UninitGame(void)
{
	UninitField_Yoko();
	UninitField();
	UninitRoof();
	UninitFloor();

	UninitRing();
	UninitEnemy();
	UninitPlayer();

	UninitEffect();
	UninitResult();
	UninitGauge();

	if (GetRankingStart() == true) {
		UninitRanking();
	}
}

void UpdateGame(void)
{
	UpdateField_Yoko();
	UpdateField();
	UpdateRoof();
	UpdateFloor();

	UpdateRing();
	UpdatePlayer();
	UpdateEnemy();

	UpdateEffect();
	UpdateResult();
	UpdateGauge();

	if (GetRankingStart() == true) {
		UpdateRanking();
	}
}

void DrawGame(void)
{
	DrawField_Yoko();
	DrawField();
	DrawRoof();
	DrawFloor();

	DrawRing();
	DrawEnemy();
	DrawEffect();

	DrawPlayer();


	SlowEffect();

	DrawResult();
	
	if (GetRankingStart() == true) {
		DrawRanking();
	}

	DrawGauge();
	DrawScore(TEXTURE_INDEX_WHITENUMBER, 1000.0f, 10.0f, GetScore(), 6, 0.15f, 0.15f, 40.0f);

}
