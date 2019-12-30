#pragma once

#include "main.h"

//==============================
//   関数宣言
//==============================
void InitScene(void);		//初期化
void UninitScene(void);		//終了
void UpdateScene(void);		//更新
void DrawScene(void);		//描画

void SetScene(SCENE_INDEX);		//引数に移動先のシーンを指定してシーン遷移
SCENE_INDEX GetScene();			//現在のシーンの取得