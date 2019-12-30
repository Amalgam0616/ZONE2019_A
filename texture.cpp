#include <d3dx9.h>
//#include "debugPrintf.h"
#include "mydirect3d.h"
#include "texture.h"
#include "main.h"

#define TEXTURE_FILENAME_MAX (64)

typedef struct TextureFile_tag
{
	char filename[TEXTURE_FILENAME_MAX];
	int width;
	int height;
} TextureFile;

static const TextureFile g_TextureFiles[] =
{
	{ "asset/texture/roof_std.png", 160, 160 },
	{ "asset/texture/roof_broke.png", 160, 160 },

	{ "asset/texture/金メダル級.png", 1200, 500 },
	{ "asset/texture/銀メダル級.png", 1200, 500 },
	{ "asset/texture/銅メダル級.png", 1200, 500 },
	{ "asset/texture/Result_BG.png", SCREEN_WIDTH / 1.15, SCREEN_HEIGHT / 1.2 },
	{ "asset/texture/Ranking_BG.png", SCREEN_WIDTH , SCREEN_HEIGHT },
	{ "asset/texture/taitoru.png", 1024, 724 },
	{ "asset/texture/Title_BG.png", SCREEN_WIDTH, SCREEN_HEIGHT},
	{ "asset/texture/SKIP.png",500, 300},
	{ "asset/texture/START.png",500, 300},
	{ "asset/texture/HitEffect.png",240, 480},
	{ "asset/texture/Audience.png",566, 1049},

	{ "asset/texture/WhatAreYouScore.png",716,78},
	{ "asset/texture/FinishText.png",800,800},

	{ "asset/texture/PunchIngMachine.png",300,400},
	{ "asset/texture/GloveTexture.png",300,300},
	{ "asset/texture/SceneChangeEffect.png",300,300},


	{ "asset/texture/Goldnumber.png", 3000, 300 },
	{ "asset/texture/Silvernumber.png", 3000, 300 },
	{ "asset/texture/Blonzenumber.png", 3000, 300 },
	{ "asset/texture/whitenumber.png", 3000, 300 },

	{ "asset/texture/Damage.png",SCREEN_WIDTH, SCREEN_HEIGHT },
	{ "asset/texture/SlowEffect.png",SCREEN_WIDTH, SCREEN_HEIGHT},
	{ "asset/texture/GrobeEffect1.png",400, 800},
	{ "asset/texture/GrobeEffect2.png",400, 800},

	{ "asset/texture/gauge_1.png",241, 1024},
	{ "asset/texture/gauge_2.png",241, 1024},
	{ "asset/texture/gauge_3.png",241, 1024},

};

static const int TEXTURE_FILE_COUNT = sizeof(g_TextureFiles) / sizeof(g_TextureFiles[0]);

static_assert(TEXTURE_INDEX_MAX == TEXTURE_FILE_COUNT, "TEXTURE_INDEX_MAX != TEXTURE_FILE_COUNT");

static LPDIRECT3DTEXTURE9 g_pTextures[TEXTURE_FILE_COUNT] = {};

int LoadTexture(void)
{   
    LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();
	if( !pDevice )
	{
		return TEXTURE_FILE_COUNT;
	}

	int failed_count = 0;

	for( int i = 0; i < TEXTURE_FILE_COUNT; i++ )
	{
		
		if( FAILED(D3DXCreateTextureFromFile(pDevice, g_TextureFiles[i].filename, &g_pTextures[i])) ) 
		{
            // DebugPrintf("テクスチャの読み込みに失敗 ... %s\n", g_TextureFiles[i].filename);
			failed_count++;
		}
	}

	return failed_count;
}

void UninitTexture(void)
{
	for( int i = 0; i < TEXTURE_FILE_COUNT; i++ )
	{
		
		if( g_pTextures[i] )
		{
			g_pTextures[i]->Release();
			g_pTextures[i] = NULL;
		}
	}
}

LPDIRECT3DTEXTURE9 Texture_GetTexture(TextureIndex index)
{
	if (index < 0 || index >= TEXTURE_INDEX_MAX)
	{
		return NULL;
	}

	return g_pTextures[index];
}

int Texture_GetWidth(TextureIndex index)
{
	if (index < 0 || index >= TEXTURE_INDEX_MAX)
	{
		return NULL;
	}

	return g_TextureFiles[index].width;
}

int Texture_GetHeight(TextureIndex index)
{
	if (index < 0 || index >= TEXTURE_INDEX_MAX)
	{
		return NULL;
	}

	return g_TextureFiles[index].height;
}
