#include "texture.h"
#include "player.h"
#include "enemy.h"
#include "score.h"
#include "main.h"
#include "sound.h"
#include "result.h"
#include "field.h"
#include "Roof.h"
#include "floor.h"
#include "Effect.h"
#include "camera.h"
#include "Ring.h"
#include "sky.h"
#include "particle.h"
#include "gauge.h"
#include "Ranking.h"


static bool SkyFlag;

void InitGame(void)
{
	SkyFlag = false;

	InitScore();

	InitField();
	InitRoof();
	InitFloor();

	InitRing();

	InitEnemy();
	InitPlayer();
	Init_GameCamera();

	InitSky();

	InitEffect();
	InitResult();
	InitGauge();
	InitParticle();
	if (GetRankingStart() == true) {
		DrawRanking();
	}
}

void UninitGame(void)
{
	UninitField();
	UninitRoof();
	UninitFloor();

	UninitRing();
	UninitEnemy();
	UninitPlayer();

	UninitSky();
	UninitEffect();
	UninitGauge();
	UninitParticle();

	UninitResult();
	if (GetRankingStart() == true) {
		UninitRanking();
	}
}

void UpdateGame(void)
{
	if (!GetResultStart())
	{
		if (!SkyFlag)
		{
			UpdateField();
			UpdateRoof();
			UpdateFloor();

			UpdateRing();
			UpdateGauge();
			UpdatePlayer();
		}
	}
	UpdateEnemy();
	UpdateParticle();

	if (SkyFlag)
	{
		UpdateSky();
	}

	UpdateEffect();

	if (GetRankingStart() == true) {
		UpdateRanking();
	}

	if (GetResultStart())
	{
		UpdateResult();
	}
}

void DrawGame(void)
{
	if (!GetResultStart())
	{
		if (!SkyFlag)
		{
			DrawField();
			DrawRoof();
			DrawFloor();
			DrawRing();
		}
	}

	DrawEnemy();
	DrawParticle();

	if (!SkyFlag)
	{
		DrawPlayer();
		DrawGauge();
	}
	if (SkyFlag)
	{
		DrawSky();
	}

	SlowEffect();

	if (GetResultStart())
	{
		DrawResult();
	}

	if (GetRankingStart() == true) {
		DrawRanking();
	}
	else {
		DrawScore(TEXTURE_INDEX_WHITENUMBER, 1000.0f, 10.0f, GetScore(), 6, 0.15f, 0.15f, 40.0f);
	}
}
void SetSkyFlag_ture(void)
{
	SkyFlag = true;
}
bool GetSkyFlag(void)
{
	return SkyFlag;
}
