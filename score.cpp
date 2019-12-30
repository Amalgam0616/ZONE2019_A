#include "score.h"
#include "texture.h"
#include "sprite.h"
//========================
//  �O���[�o���ϐ��錾
//========================
static unsigned int	g_Score;	//�X�R�A������

//========================
//  ����������
//========================
void InitScore(void)
{
	//�X�R�A�̏�����
	g_Score = 0;
}
//�X�R�A�`��
void DrawScore(TextureIndex texture_index,float x, float y, int score, int fig, float sx, float sy, float offset)
{
	// �J���X�g�p�̍ő吔�l�����
	int count_stop_score = 1;

	for (int i = 0; i < fig; i++)
	{
		count_stop_score *= 10;
	}

	//�ő�l�̕␳����
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

//�X�R�A�`��(�A���t�@�l����)
void DrawScore(TextureIndex texture_index,float x, float y, int score, int fig, float sx, float sy, float offset,int alpha)
{
	// �J���X�g�p�̍ő吔�l�����
	int count_stop_score = 1;

	for (int i = 0; i < fig; i++)
	{
		count_stop_score *= 10;
	}

	//�ő�l�̕␳����
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
	if (n < 0 || n > 9) return;	//�X�R�A�̐�������

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

//���l�����
void Number_Draw_Alpha(TextureIndex texture_index, float x, float y, int n, float sx, float sy, int alpha)
{
	if (n < 0 || n > 9) return;	//�X�R�A�̐�������

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
//  �X�R�A�̑����Z����
//========================
void AddScore(int score)
{
	int add = g_Score + score;	//�ϐ�add�Ɍ��݂̃X�R�A�{�����i���������X�R�A�̒l�j������

	//������������̒l��0���傫���i���̒l�̏ꍇ�j
	if (add > 0)
	{
		g_Score += score;
	}
	else
	{
		//������������̒l���O�������ꍇ�̓}�C�i�X�̐�����\�����Ȃ��悤�ɃX�R�A�ɂO��ݒ肷��
		g_Score = 0;
	}
}
//========================
//  �X�R�A��getter
//========================
int GetScore(void)
{
	return g_Score;
}