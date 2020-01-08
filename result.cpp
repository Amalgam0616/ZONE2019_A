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

bool g_ResultStart; //�e�X�g�p

int g_Audience_Anime_V;
int AnimetionCnt;
int g_ResultAlpha;
int RankingChangeCnt; //�����L���O�փV�[���J�ڂ���b��

float playery;

bool g_Ranking_Start; //�����L���O�ɑJ�ڂ���ϐ�

float g_ResultGlovePos;
int ResultGloveMoveCnt;
bool g_ResultPunchEffectFlg;

void InitResult(void)
{
	g_ResultStart = false;
	g_Ranking_Start = false;

	AnimetionCnt = 0;

	g_Audience_Anime_V = 1;

	g_ResultAlpha = 0;
	RankingChangeCnt = 0;

	g_ResultGlovePos = 1220.0f;
	ResultGloveMoveCnt = 0;
	g_ResultPunchEffectFlg = false;

}

void UninitResult(void)
{
}

void UpdateResult(void)
{
	playery = (GetEnemy() + 1)->Pos.y;
	//���ʒu�܂Ŕ�񂾂����U���g���I�����ĂȂ��Ƃ�
	if (playery >= 1500 && g_Ranking_Start == false)
	{
		g_ResultStart = true;
	}
	else {
		g_ResultStart = false;
	}

	//�V�[���J��
	//�V�[���J�ڂ̊G
	ResultGloveMoveCnt++;

	if (ResultGloveMoveCnt >= 40) {
		if (g_ResultPunchEffectFlg == false) {
			g_ResultGlovePos = 1170.0f;
			g_ResultPunchEffectFlg = true;
			ResultGloveMoveCnt = 0;
		}
		else if (g_ResultPunchEffectFlg == true) {
			g_ResultGlovePos = 1220.0f;
			g_ResultPunchEffectFlg = false;
			ResultGloveMoveCnt = 0;
		}
	}

	//�ǉ�����1==========================================
	//�ł������G���^�[�L�[����������
	//�A�b�p�[���Ă���20�b��������(��X�A�I�u�W�F�N�g�ɏՓ˂�����ɂȂ�Ǝv��)
	if ((Keyboard_IsTrigger(DIK_NUMPADENTER) && GetLastPunchPhase() == PUNCH_PHASE_CLASH && g_ResultStart == true) ||
		(GetLastPunchPhase() == PUNCH_PHASE_CLASH && g_ResultStart == true && RankingChangeCnt >= 600))
	{
		g_Ranking_Start = true;
		InitRanking();
	}
	//���ʂ̃G���^�[�L�[
	else if (Keyboard_IsTrigger(DIK_RETURN) && GetLastPunchPhase() == PUNCH_PHASE_CLASH && g_ResultStart == true ||
		(GetLastPunchPhase() == PUNCH_PHASE_CLASH && g_ResultStart == true && RankingChangeCnt >= 600))
	{
		g_Ranking_Start = true;
		InitRanking();
	}
	//�ǉ�����1==========================================
}

void DrawResult(void)
{
	//MakeVertexField();
	if (g_ResultStart == true && g_Ranking_Start == false)
	{
		RankingChangeCnt++;

		//���𔖂�����e�N�X�`��
		Sprite_Draw(TEXTURE_INDEX_RESULT_BG, ((SCREEN_WIDTH - SCREEN_WIDTH / 1.15f) / 2), ((SCREEN_HEIGHT - SCREEN_HEIGHT / 1.2f) / 2));


		//�����_�����e�N�X�`��
		if (GetScore() < RANKING_BORDER1)
		{
			//Sprite_Draw(TEXTURE_INDEX_BLONZERANK, (SCREEN_WIDTH - 600) / 2, 100.0f);
			Sprite_Draw(TEXTURE_INDEX_BLONZERANK,
				SCREEN_WIDTH / 2,
				100.0f,
				0,
				0,
				1200,
				500,
				1200 / 2,
				500 / 2,
				0.2f,
				0.2f,
				0.0f,
				255);
			//�X�R�A�iX�ʒuY�ʒu�@���l�@���� �g�嗦X�@�g�嗦Y  �����Ƃ̃I�t�Z�b�g�j
			DrawScore(TEXTURE_INDEX_BLONZENUMBER, 270.0f, SCREEN_HEIGHT / 2, GetScore(), 6, 0.5f, 0.5f, 120.0f);


		}
		//�⃁�_�����e�N�X�`��
		else if (GetScore() >= RANKING_BORDER1 && GetScore() < RANKING_BORDER2)
		{
			//Sprite_Draw(TEXTURE_INDEX_SILVERRANK, (SCREEN_WIDTH - 600) / 2, 100.0f);
			Sprite_Draw(TEXTURE_INDEX_SILVERRANK,
				SCREEN_WIDTH / 2,
				175.0f,
				0,
				0,
				1200,
				500,
				1200 / 2,
				500 / 2,
				0.5f,
				0.5f,
				0.0f,
				255);
			DrawScore(TEXTURE_INDEX_SILVERNUMBER, 270.0f, SCREEN_HEIGHT / 2, GetScore(), 6, 0.5f, 0.5f, 120.0f);
		}
		//�����_�����e�N�X�`��
		else if (GetScore() >= RANKING_BORDER2)
		{
			//Sprite_Draw(TEXTURE_INDEX_GOLDRANK, (SCREEN_WIDTH - 600) / 2, 100.0f);
			Sprite_Draw(TEXTURE_INDEX_GOLDRANK,
				SCREEN_WIDTH / 2,
				100.0f,
				0,
				0,
				1200,
				500,
				1200 / 2,
				500 / 2,
				0.2f,
				0.2f,
				0.0f,
				255);
			DrawScore(TEXTURE_INDEX_GOLDNUMBER, 270.0f, SCREEN_HEIGHT / 2, GetScore(), 6, 0.5f, 0.5f, 120.0f);
		}

		//�V�[���J�ڎw���e�N�X�`��
		//�V�[���J�ڂ̎w��
		Sprite_Draw(TEXTURE_INDEX_SKIP_SCENE_CHANGE,
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

		if (g_ResultPunchEffectFlg == true) {
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
			g_ResultGlovePos,
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
		//���Ȃ��̃X�R�A�́c
		//Sprite_Draw(TEXTURE_INDEX_WHATSCORE, SCREEN_WIDTH - 1266, SCREEN_HEIGHT - 450);

	}
}

bool GetRankingStart() {
	return g_Ranking_Start;
}