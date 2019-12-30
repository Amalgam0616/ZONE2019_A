#include "scene.h"
#include "Ranking.h"
#include "Title.h"
#include "score.h"
#include "game.h"
//====================================
//  �O���[�o���ϐ��錾
//====================================
static SCENE_INDEX g_SceneIndex;

//====================================
//�@�V�[������������
//====================================
void InitScene()
{
	//�V�[������ɂ���
	g_SceneIndex = SCENE_INDEX_NONE;
}
//====================================
//�@�V�[���I������
//====================================
void UninitScene(void)
{
	//�e�V�[�����Ƃ�Uninit�֐����Ăяo��
	switch (g_SceneIndex) 
	{
		case SCENE_INDEX_NONE:
			break;

		case SCENE_INDEX_TITLE:
			UninitTitle();
			break;

		case SCENE_INDEX_GAME:
			UninitGame();
			break;

		case SCENE_INDEX_RANKING:
			UninitRanking();
			break;
	}
}
//====================================
//�@�V�[���X�V����
//====================================
void UpdateScene(void)
{
	//�e�V�[�����Ƃ�Update�֐����Ăяo��
	switch (g_SceneIndex)
	{
		case SCENE_INDEX_NONE:
			break;

		case SCENE_INDEX_TITLE:
			UpdateTitle();
			break;

		case SCENE_INDEX_GAME:
			UpdateGame();
			break;

		case SCENE_INDEX_RANKING:
			UpdateRanking();
			break;
	}
}
//====================================
//�@�V�[���`�揈��
//====================================
void DrawScene(void)
{
	//�e�V�[�����Ƃ�Draw�֐����Ăяo��
	switch (g_SceneIndex) 
	{
		case SCENE_INDEX_NONE:
			break;

		case SCENE_INDEX_TITLE:
			DrawTitle();
			break;

		case SCENE_INDEX_GAME:
			DrawGame();
			break;

		case SCENE_INDEX_RANKING:
			DrawRanking();
			break;
	}
}
//====================================
//�@�V�[���̃Z�b�g����
//====================================
void SetScene(SCENE_INDEX scene)
{
	//�O�V�[���̏I������
	switch (g_SceneIndex) 
	{
		case SCENE_INDEX_NONE:
			break;

		case SCENE_INDEX_TITLE:
			UninitTitle();
			break;

		case SCENE_INDEX_GAME:
			UninitGame();
			break;

		case SCENE_INDEX_RANKING:
			UninitRanking();
			break;

		default:
			break;
	}

	//�V�[���؂�ւ�
	g_SceneIndex = scene;

	//�؂�ւ����V�[���̏������i�e�V�[�����Ƃ�Init�֐����Ăяo��)
	switch (g_SceneIndex) 
	{
		case SCENE_INDEX_NONE:
			break;

		case SCENE_INDEX_TITLE:
			InitScore();
			InitTitle();
			break;

		case SCENE_INDEX_GAME:
			InitGame();
			break;

		case SCENE_INDEX_RANKING:
			InitRanking();
			break;

		default:
			break;
	}
}
//====================================
//�@���݂̃V�[���̎擾
//====================================
SCENE_INDEX GetScene()
{
	return g_SceneIndex;
}
