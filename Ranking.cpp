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

#define MAX_RANKING	3	//�X�R�A��ʂR�܂�

//�����L���O�̍\����
typedef struct
{
	int high_score[MAX_RANKING];
	int PlayerScore;
}SCOREDATA;

//�O���[�o���ϐ�
SCOREDATA rank;
int TitleChangeCnt;
int RankingDrawCnt;
int MyScoreMove;

float g_RankingGlovePos;
int RankingGloveMoveCnt;
bool g_RankingPunchEffectFlg;


void InitRanking(void)
{
	rank.PlayerScore = GetScore();
	TitleChangeCnt = 0;
	RankingDrawCnt = 0;
	MyScoreMove = 0;

	g_RankingGlovePos = 1220.0f;
	RankingGloveMoveCnt = 0;
	g_RankingPunchEffectFlg = false;

	FILE *fp;	/* �t�@�C���|�C���^�̐錾 */

	//�y�������̏����z
	//�t�@�C������f�[�^��ǂݍ����
	//rank�ɓ����

	/*�t�@�C���̃I�[�v�� */
	if ((fp = fopen("asset/text/Score.txt", "rb+")) == NULL) {
		MessageBox(NULL, "txt�t�@�C�����ǂݍ��߂܂���", "�G���[", MB_OK);
		exit(EXIT_FAILURE);	/*�G���[�̏ꍇ�͒ʏ�A�ُ�I������ */
	}


	/*�t�@�C���̓ǂ݂���*/
	fread(&rank.high_score, sizeof(int), 3, fp);

	////�X�R�A�̃\�[�g
	SetRank(GetScore());

	//�t�@�C���|�C���^��擪�Ɏ����Ă���
	fseek(fp, 0L, SEEK_SET);

	//�y�I���̏����z
	//rank�̃f�[�^���t�@�C���ɕۑ�����
	fwrite(rank.high_score, sizeof(int), 3, fp);

	/*�t�@�C���̃N���[�Y*/
	fclose(fp);

}

void UninitRanking(void)
{
	
}

void UpdateRanking(void)
{
	DebugProc_Print((char*)"�X�R�A�F%d", GetScore());
	//�V�[���J�ڂ̕b���𐔂���
	TitleChangeCnt++;

	//�V�[���J��
	//�V�[���J�ڂ̊G
	RankingGloveMoveCnt++;

	if (RankingGloveMoveCnt >= 40) {
		if (g_RankingPunchEffectFlg == false) {
			g_RankingGlovePos = 1170.0f;
			g_RankingPunchEffectFlg = true;
			RankingGloveMoveCnt = 0;
		}
		else if (g_RankingPunchEffectFlg == true) {
			g_RankingGlovePos = 1220.0f;
			g_RankingPunchEffectFlg = false;
			RankingGloveMoveCnt = 0;
		}
	}

	//�ł����G���^�[����������
	//5�b��������
	if (Keyboard_IsTrigger(DIK_NUMPADENTER) || 
		TitleChangeCnt >= 600)
	{
		InitCamera();
		InitPlayer();
		SetScene(SCENE_INDEX_TITLE);
	}
	//�X�y�[�X����������
	//5�b��������
	else if (Keyboard_IsTrigger(DIK_SPACE) || 
			TitleChangeCnt >= 600)
	{
		InitCamera();
		InitPlayer();
		SetScene(SCENE_INDEX_TITLE);
	}
}

void DrawRanking(void)
{
	if (RankingDrawCnt < 254 && MyScoreMove >= 340) {
		RankingDrawCnt += 2;
	}
	if (MyScoreMove <= 340){
		MyScoreMove += 3;
	}
	//�w�i
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

	//�V�[���J��
	//Sprite_Draw(TEXTURE_INDEX_SKIP_SCENE_CHANGE, 0, 0);
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
		RankingDrawCnt);

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
		RankingDrawCnt);

	if (g_RankingPunchEffectFlg == true) {
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
			RankingDrawCnt);
	}
	//�O���[�u
	Sprite_Draw(TEXTURE_INDEX_GLOVE,
		g_RankingGlovePos,
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
		RankingDrawCnt);

	//�X�R�A�`��iX�ʒuY�ʒu�@���l�@���� �g�嗦X�@�g�嗦Y  �����Ƃ̃I�t�Z�b�g�j
	//============================================
	//1�ʂ̃X�R�A
	//============================================	
	DrawScore(TEXTURE_INDEX_GOLDNUMBER, 400, 210, rank.high_score[0], 6, 0.3f, 0.3f, 80, RankingDrawCnt);

	//============================================
	//2�ʂ̃X�R�A
	//============================================
	//�X�R�A��1�ʂƓ����łȂ��ꍇ
	if (rank.high_score[1] != rank.high_score[0]) {
		DrawScore(TEXTURE_INDEX_SILVERNUMBER, 180, 340, rank.high_score[1], 6, 0.3f, 0.3f, 80, RankingDrawCnt);
	}
	//�X�R�A��1�ʂƓ����ꍇ
	else {
		DrawScore(TEXTURE_INDEX_GOLDNUMBER, 180, 340, rank.high_score[1], 6, 0.3f, 0.3f, 80, RankingDrawCnt);
	}

	//============================================
	//3�ʂ̃X�R�A
	//============================================
	//�X�R�A��1�ʂƓ����ꍇ
	if (rank.high_score[2] == rank.high_score[0]) {
		DrawScore(TEXTURE_INDEX_GOLDNUMBER, 670, 380, rank.high_score[2], 6, 0.3f, 0.3f, 80, RankingDrawCnt);
	}
	//�X�R�A��2�ʂƓ����ꍇ
	else if (rank.high_score[2] == rank.high_score[1]) {
		DrawScore(TEXTURE_INDEX_SILVERNUMBER, 670, 380, rank.high_score[2], 6, 0.3f, 0.3f, 80, RankingDrawCnt);
	}
	//�X�R�A��3�ʂ̏ꍇ
	else {
		DrawScore(TEXTURE_INDEX_BLONZENUMBER, 670, 380, rank.high_score[2], 6, 0.3f, 0.3f, 80, RankingDrawCnt);
	}

	//============================================
	//�����̃X�R�A
	//============================================	
	//�����_�����e�N�X�`��
	if (GetScore() < RANKING_BORDER1)
	{
		//�X�R�A�iX�ʒuY�ʒu�@���l�@���� �g�嗦X�@�g�嗦Y  �����Ƃ̃I�t�Z�b�g�j
		DrawScore(TEXTURE_INDEX_BLONZENUMBER, 270.0f, SCREEN_HEIGHT / 2 - MyScoreMove, GetScore(), 6, 0.5f, 0.5f, 120.0f,255);
	}
	//�⃁�_�����e�N�X�`��
	else if (GetScore() >= RANKING_BORDER1 && GetScore() < RANKING_BORDER2)
	{
		DrawScore(TEXTURE_INDEX_SILVERNUMBER, 270.0f, SCREEN_HEIGHT / 2 - MyScoreMove, GetScore(), 6, 0.5f, 0.5f, 120.0f, 255);
	}
	//�����_�����e�N�X�`��
	else if (GetScore() >= RANKING_BORDER2)
	{
		DrawScore(TEXTURE_INDEX_GOLDNUMBER, 270.0f, SCREEN_HEIGHT / 2 - MyScoreMove, GetScore(), 6, 0.5f, 0.5f, 120.0f, 255);
	}

}


void SetRank(int newscore)
{	//�y�L�^�̍X�V�����z
	int i, j, tmp;

	//rank�̃f�[�^�Ɣ�r���ĐV�����L�^�̕���
	//�傫����΋L�^���X�V����
	for (i = 0; i < MAX_RANKING; i++)
	{
		if (rank.high_score[i] < newscore)
		{
			rank.high_score[i] = newscore;

			break;
		}

	}

	//�\�[�g�i���ёւ��j����������
	for (i = 0; i < MAX_RANKING; i++)
	{
		for (j = i + 1; j <  MAX_RANKING; j++)
		{//�ォ�牺�ւ̑���

			if (rank.high_score[i] < rank.high_score[j])
			{
				tmp = rank.high_score[i];

				rank.high_score[i] = rank.high_score[j];

				rank.high_score[j] = tmp;

			}
		}
	}


}