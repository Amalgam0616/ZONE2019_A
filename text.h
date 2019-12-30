#pragma once

/*=================================================================
�쐬��: Amalgam
�T�v�F�e�L�X�g�֘A���g����悤�ɂȂ�֐��ł�
�@�@�@��̂̓��[�_�[��3d.cpp����e�L�X�g�֘A���������Ă��������ł��B
�@�@�@�����g���₷�����l�����ĉ��ς����Ă���܂��B

�@�@�@��������Ɋւ��ẮA�������Ă݂��͗ǂ����ǎg�����͂����ς�킩��Ȃ��ł��B
=================================================================*/

#include "main.h"

//�e�L�X�g�C���f�b�N�X �ǉ�����ۂ͂����� text.cpp �̃t�H���g���X�g��ύX���Ă�������
enum FONT_INDEX
{
	FONT_INDEX_001 = 0,

	FONT_INDEX_MAX,
};

//=============================================================================
// �v���g�^�C�v�錾
//=============================================================================

//�t�H���g�𐶐�����֐�
//�����P�FLPD3DXFONT�F�t�H���g������ꂽ���|�C���^
//�����Q�FFONT_INDEX�F�g�p����t�H���g�̃C���f�b�N�XNo.
//�����R�Fint�F�����̕�
//�����S�Fint�F�����̍���
HRESULT Text_CriateFont(LPD3DXFONT *p_Font, int w, int h, FONT_INDEX index, bool isItalic);

//�e�L�X�g��`�悷��֐�
//�����P�FLPD3DXFONT�F�g�p����t�H���g
//�����Q�Fchar*�F�\�����镶����̃|�C���^
//�����R�Fint�F���W(���R)
//�����S�Fint�F���W(�^�e)
//�����T�FFONT_INDEX�F�g���t�H���g�̃C���f�b�N�X�@�f�t�H���g�� FONT_INDEX_001
//�����U�FD3DXCOLOR�F�F�w��@�f�t�H���g�͍�
void TextDraw(LPD3DXFONT p_Font, const char *text, int X, int Y, FONT_INDEX index = FONT_INDEX_001, D3DXCOLOR color = D3DXCOLOR(0, 0, 0, 255));

//�I���֐�
void UninitText();

