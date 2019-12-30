#pragma once
#include "texture.h"

#define NUMBER_WIDTH  (300)
#define NUMBER_HEIGHT (300)

//x���ʒu�@y�c�ʒu�@score�\������l  fig����  bZero�[���̎��ɕ\�����邩�t���O
//void DrawScore(float x, float y, int score, int fig, bool bZero);
void DrawScore(TextureIndex texture_index,float x, float y, int score, int fig, float sx, float sy, float offset);

//�X�R�A�`��(�A���t�@�l����)
void DrawScore(TextureIndex texture_index, float x, float y, int score, int fig, float sx, float sy, float offset, int alpha);

// 0 <= n <= 9
void Number_Draw(TextureIndex texture_index,float x, float y, int n, float sx, float sy);

//���`�A���i���o�[�v�Z
void Number_Draw_Alpha(TextureIndex texture_index, float x, float y, int n, float sx, float sy, int alpha);

//�ȉ��A�킵�������肢�����܂�

//g_Score�̏�����
void InitScore(void);

//�X�R�A�̑����Z����
void AddScore(int score);

//�X�R�A�̃Q�b�^�[
int GetScore(void);