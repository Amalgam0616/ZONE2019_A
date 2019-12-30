#pragma once

#include <windows.h>
#include "xaudio2.h"

//=================================================
//�@�T�E���h���x����`(enum)
//  ��sound.cpp��31�s�ڂ��L�q����O�ɏ������ƁI
//=================================================
typedef enum
{
	//  ��
	//  SOUND_LABEL_BGM_TITLE,	//�^�C�g��
	//  ���̂悤�ɋL�q���邱�ƁB

	SOUND_LABEL_SE_HIT,	//�^�C�g��

	SOUND_LABEL_MAX,	//�Ȃ̍ő吔

} SOUND_LABEL;

//==============================
//  �֐��錾
//==============================
bool InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(SOUND_LABEL label);
void StopSound(SOUND_LABEL label);
void StopSound(void);
