#include "sky.h"
#include "myDirect3D.h"
#include "input.h"
#include "texture.h"
#include "debugproc.h"
#include "enemy.h"
#include "texture.h"
#include "sprite.h"
#include "time.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define ENEMY_HEAD		(1)					// �G�l�~�[�̓����w��
#define MODEL_NUM		(3)					//���f���̐�
#define	MODEL_MOVE		(10.0f)				// �ړ���

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void SetModel(int ModelNum, D3DXVECTOR3 Pos, D3DXVECTOR3 Rot, D3DXVECTOR3 Scale);
void DrawModel();
void PlusModelPos(int ModelNum, float x, float y, float z);
void PlusModelRot(int ModelNum, float x, float y, float z);
void InitUFO(LPDIRECT3DDEVICE9 pDevice);
void DrawUFO();
//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffSky = NULL;	//���_�o�b�t�@�ւ̃|�C���^
D3DXMATRIXA16			g_mtxWorldModel;

D3DXMATRIX				g_mtxWorldSky;		// ���[���h�}�g���b�N�X
D3DXVECTOR3				g_posSky;			// ��̈ʒu
D3DXVECTOR3				g_rotSky;			// ��̌���(��])
D3DXVECTOR3				g_sclSky;			// ��̑傫��(�X�P�[��)
float					ty;					// �e�N�X�`���̌��ݎw��l(�c�j
float					PlusX;		// ����X�N���[��������̂Ɏg�p,���̒l���X�N���[�����i��ł���

bool					CloudFlag;			//�_�X�v���C�g��`�悷�邩���Ȃ����̃t���O
bool					AirplaneFlag;		//3D�I�u�W�F�N�g��`�悷�邩���Ȃ����̃t���O
bool					UFOFlag;			//3D�I�u�W�F�N�g��`�悷�邩���Ȃ����̃t���O
bool					MoonFlag;			//3D�I�u�W�F�N�g��`�悷�邩���Ȃ����̃t���O
int						CntFrame;			//�t���[�������l�𒴂��邲�Ƃɑ����J�E���^

LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffUFO = NULL;	// ���_�o�b�t�@�ւ̃|�C���^
D3DXMATRIX				g_mtxWorldUFO;			// ���[���h�}�g���b�N�X
D3DXVECTOR3				g_posUFO;				// UFO�̈ʒu
D3DXVECTOR3				g_rotUFO;				// UFO�̌���(��])
D3DXVECTOR3				g_sclUFO;				// UFO�̑傫��(�X�P�[��)

float					AddUFO_Y;
float					AddUFO_Z;

bool					UFO_AwayFlag;
//=========================================================================
//���f���̐����p��
XMODEL g_SkyModel[MODEL_NUM] = {};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitSky(void)
{
	srand((unsigned int)time(NULL));

	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();

	//���f����ǂݍ���
	if (FAILED(LoadXFile(&g_SkyModel[0], XFILE_INDEX_AIRPLANE)) ||
		FAILED(LoadXFile(&g_SkyModel[1], XFILE_INDEX_UFO)) ||
		FAILED(LoadXFile(&g_SkyModel[2], XFILE_INDEX_MOON)))
	
	{
		return E_FAIL;
	}

	//UFO�I�u�W�F�N�g�̍쐬
	InitUFO(pDevice);

	// �I�u�W�F�N�g�̒��_�o�b�t�@�𐶐�
	if (FAILED(pDevice->CreateVertexBuffer
	(
		sizeof(VERTEX_3D) * NUM_VERTEX,	// ���_�f�[�^�p�Ɋm�ۂ���o�b�t�@�T�C�Y(�o�C�g�P��)
		D3DUSAGE_WRITEONLY,			// ���_�o�b�t�@�̎g�p�@�@
		FVF_VERTEX_3D,				// �g�p���钸�_�t�H�[�}�b�g
		D3DPOOL_MANAGED,			// ���\�[�X�̃o�b�t�@��ێ����郁�����N���X���w��
		&g_pVtxBuffSky,				// ���_�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^
		NULL)))						// NULL�ɐݒ�
	{
		return E_FAIL;
	}

	{//���_�o�b�t�@�̒��g�𖄂߂�
		VERTEX_3D *pVtx;

		// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
		g_pVtxBuffSky->Lock(0, 0, (void**)&pVtx, 0);

		// ���_���W�̐ݒ�
		pVtx[0].pos = D3DXVECTOR3(-50.0f, 50.0f, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(50.0f, 50.0f, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-50.0f, -50.0f, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(50.0f, -50.0f, 0.0f);

		// �@���̐ݒ�
		pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

		// ���_�J���[�̐ݒ�
		pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// �e�N�X�`�����W�̐ݒ�
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[3].tex = D3DXVECTOR2(0.0f, 0.0f);

		// ���_�f�[�^���A�����b�N����
		g_pVtxBuffSky->Unlock();
	}

	//�w�i�̋�̏����ݒ�===================================================
	g_posSky = D3DXVECTOR3(0.0f,0.0f,0.0f);
	g_sclSky = D3DXVECTOR3(0.1f, 20.0f, 20.0f);
	g_rotSky = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(0.0f), D3DXToRadian(6.0f));

	//�o������3D�I�u�W�F�N�g�̏����ݒ�=====================================
	//��s�@
	g_SkyModel[0].Pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_SkyModel[0].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));
	g_SkyModel[0].Scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	//�l�H�q��
	g_SkyModel[1].Pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_SkyModel[1].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(45.0f), D3DXToRadian(45.0f));
	g_SkyModel[1].Scale = D3DXVECTOR3(0.3f, 0.3f, 0.3f);

	//��
	g_SkyModel[2].Pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_SkyModel[2].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));
	g_SkyModel[2].Scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	ty = 0.005f;

	CloudFlag = false;
	AirplaneFlag = false;
	UFOFlag = false;
	MoonFlag = false;
	CntFrame = 0;
	AddUFO_Y = 0.0f;
	AddUFO_Z = 0.0f;
	UFO_AwayFlag = false;

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void UninitSky(void)
{
	if (g_pVtxBuffSky != NULL)
	{// ���_�o�b�t�@�̊J��
		g_pVtxBuffSky->Release();
		g_pVtxBuffSky = NULL;
	}
	//���b�V���������
	for (int i = 0; i < MODEL_NUM; i++)
	{
		SAFE_RELEASE(g_SkyModel[i].pMesh);
	}
}
//=============================================================================
// �X�V����
//=============================================================================
void UpdateSky(void)
{
	//�w�i�̋�̈ʒu�ݒ�
	g_posSky.x = -15.0f;
	g_posSky.y = (GetEnemy() + ENEMY_HEAD)->Pos.y;
	g_posSky.z = (GetEnemy() + ENEMY_HEAD)->Pos.z;
	
	g_posUFO = D3DXVECTOR3(10.0f, (GetEnemy() + ENEMY_HEAD)->Pos.y + AddUFO_Y,(GetEnemy() + ENEMY_HEAD)->Pos.z + 200.0f + AddUFO_Z);
	//g_posUFO.x = 10.0f;
	//g_posUFO.y = (GetEnemy() + ENEMY_HEAD)->Pos.y;
	//g_posUFO.z = (GetEnemy() + ENEMY_HEAD)->Pos.z;
	
	//��̃e�N�X�`�������ɐi�߂Ă���
	ty += 0.0015f;

	if (ty < 1.0f)
	{
		{	//���_�o�b�t�@�̒��g�𖄂߂�
			VERTEX_3D *pVtx;

			// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
			g_pVtxBuffSky->Lock(0, 0, (void**)&pVtx, 0);

			// �e�N�X�`�����W�̐ݒ�
			pVtx[0].tex = D3DXVECTOR2(0.0f, 1.0f - ty);
			pVtx[1].tex = D3DXVECTOR2(1.0f, 1.0f - ty);
			pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f - ty + 0.005f);
			pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f - ty + 0.005f);

			// ���_�f�[�^���A�����b�N����
			g_pVtxBuffSky->Unlock();
		}
	}
	DebugProc_Print((char*)"enemy.x:%f,enemy.y:%f,enemy.z:%f", (GetEnemy() + ENEMY_HEAD)->Pos.x, (GetEnemy() + ENEMY_HEAD)->Pos.y, (GetEnemy() + ENEMY_HEAD)->Pos.z);
	DebugProc_Print((char*)"sky.x:%f,sky.y:%f,sky.z:%f\n", g_posSky.x, g_posSky.y, g_posSky.z);
	DebugProc_Print((char*)"frame:%d\n",Getg_PunchFrameCnt());
	DebugProc_Print((char*)"CntFrame:%d\n",CntFrame);
	DebugProc_Print((char*)"Rot(x,y,z):%f,%f,%f", g_SkyModel[1].Rot.x, g_SkyModel[1].Rot.y, g_SkyModel[1].Rot.z);

	DebugProc_Print((char*)"UFO.x:%f,UFO.y:%f,UFO.z:%f\n", g_posUFO.x, g_posUFO.y, g_posUFO.z);
}
//=============================================================================
// ��`�揈��
//=============================================================================
void DrawSky(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();
	D3DXMATRIX mtxView, mtxScale, mtxTranslate,mtxRot;
	//�A���t�@�e�X�g��L����
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	//���C�e�B���O�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	//���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&g_mtxWorldSky);

	//�r���[�}�g���b�N�X���擾
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	//POLYGON�𐳖ʂɌ�����
#if 1
	//�t�s������߂�
	D3DXMatrixInverse(&g_mtxWorldSky, NULL, &mtxView);
	g_mtxWorldSky._41 = 0.0f;
	g_mtxWorldSky._42 = 0.0f;
	g_mtxWorldSky._43 = 0.0f;
#else
	g_mtxWorldSky._11 = mtxViex._11;
	g_mtxWorldSky._12 = mtxViex._21;
	g_mtxWorldSky._13 = mtxViex._31;
	g_mtxWorldSky._21 = mtxViex._12;
	g_mtxWorldSky._22 = mtxViex._22;
	g_mtxWorldSky._23 = mtxViex._32;
	g_mtxWorldSky._31 = mtxViex._13;
	g_mtxWorldSky._32 = mtxViex._23;
	g_mtxWorldSky._33 = mtxViex._33;
#endif

	//�X�P�[���𔽉f
	D3DXMatrixScaling(&mtxScale, g_sclSky.x, g_sclSky.y, g_sclSky.z);
	D3DXMatrixMultiply(&g_mtxWorldSky, &g_mtxWorldSky, &mtxScale);

	//�ړ��𔽉f
	D3DXMatrixTranslation(&mtxTranslate, g_posSky.x, g_posSky.y, g_posSky.z);
	D3DXMatrixMultiply(&g_mtxWorldSky, &g_mtxWorldSky, &mtxTranslate);

	//��]�𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_rotSky.y, g_rotSky.z, g_rotSky.x);
	D3DXMatrixMultiply(&g_mtxWorldSky, &g_mtxWorldSky, &mtxRot);

	// ���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldSky);

	//���_�o�b�t�@���f�o�C�X�̃f�[�^�X�g���[���Ƀo�C���h
	pDevice->SetStreamSource(0, g_pVtxBuffSky, 0, sizeof(VERTEX_3D));

	//���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_3D);

	D3DMATERIAL9 mat = {};

	mat.Diffuse.r = 1.0f;
	mat.Diffuse.g = 1.0f;
	mat.Diffuse.b = 1.0f;
	mat.Diffuse.a = 1.0f;

	mat.Ambient.r = 1.0f;
	mat.Ambient.g = 0.0f;
	mat.Ambient.b = 1.0f;
	mat.Ambient.a = 1.0f;

	//�}�e���A���Z�b�g
	pDevice->SetMaterial(&mat);

	//�e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, Texture_GetTexture(TEXTURE_INDEX_SKY));

	//POLYGON�̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0,12);

	//���C�e�B���O��L���ɂ���
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	//�A���t�@�e�X�g�𖳌���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);


	//==============================�_�Ȃ񂩂��������ɒN������Ă���܂���

	//���˂��j����90�t���[���ŉ_�`��
	if (Getg_PunchFrameCnt() >= 90)
	{
		CloudFlag = true;
	}
	if(CloudFlag)
	{
		//�_�̂��イ����
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, - 100.0f, -4100.0f + PlusX);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2,   300.0f, -4100.0f + PlusX);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2,   700.0f, -4100.0f + PlusX);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2,  1100.0f, -4100.0f + PlusX);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, - 100.0f, -4000.0f + PlusX);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2,   300.0f, -4000.0f + PlusX);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2,   700.0f, -4000.0f + PlusX);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2,  1100.0f, -4000.0f + PlusX);

		//�_�΂�΂�
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, 300.0f, -3300.0f + PlusX);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, 600.0f, -2100.0f + PlusX);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, 1100.0f, -1000.0f + PlusX);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, 0.0f, -600.0f + PlusX);

		PlusX += 20.0f;
	}
	//260�t���[���Ŕ�s�@�`��
	if (Getg_PunchFrameCnt() >= 260)
	{
		if (!AirplaneFlag)
		{
			AirplaneFlag = true;
			SetModel
			(
				0,
				D3DXVECTOR3((GetEnemy() + ENEMY_HEAD)->Pos.x - 8.0f, (GetEnemy() + ENEMY_HEAD)->Pos.y + 400.0f, (GetEnemy() + ENEMY_HEAD)->Pos.z + 200.0f),
				//�f�o�b�O�p�F
				//D3DXVECTOR3((GetEnemy() + ENEMY_HEAD)->Pos.x, (GetEnemy() + ENEMY_HEAD)->Pos.y, (GetEnemy() + ENEMY_HEAD)->Pos.z),
				D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(20.0f), D3DXToRadian(20.0f)),
				D3DXVECTOR3(0.3f, 0.3f, 0.3f)
			);
		}
	}
	if(AirplaneFlag)
	{
		{
			//��s�@�̍��W�𓮂���
			PlusModelPos(0, 0.0f, -5.0f, -2.5f);
		}
	}
	//330(329) + 60�t���[���Ől�H�q���`��
	if (AirplaneFlag == true && Getg_PunchFrameCnt() >= 60 && CntFrame == 1)
	{
		if (!UFOFlag)
		{
			UFOFlag = true;
			SetModel
			(
				1,
				D3DXVECTOR3((GetEnemy() + ENEMY_HEAD)->Pos.x - 8.0f, (GetEnemy() + ENEMY_HEAD)->Pos.y + 400.0f, (GetEnemy() + ENEMY_HEAD)->Pos.z + 200.0f),
				//�f�o�b�O�p�F
				//D3DXVECTOR3((GetEnemy() + ENEMY_HEAD)->Pos.x, (GetEnemy() + ENEMY_HEAD)->Pos.y, (GetEnemy() + ENEMY_HEAD)->Pos.z),
				D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(0.0f), D3DXToRadian(45.0f)),
				D3DXVECTOR3(3.0f, 3.0f, 3.0f)
			);
		}
	}
	if (UFOFlag)
	{
		{
			//�l�H�q���̍��W�𓮂���
			PlusModelPos(1, 0.0f,-4.5f, -2.0f);
			PlusModelRot(1, D3DXToRadian(0.0f), D3DXToRadian(0.5f), D3DXToRadian(0.0f));
		}
	}
	//330(329) + 250�t���[���Ō��`��
	if (UFOFlag == true && Getg_PunchFrameCnt() >= 250 && CntFrame == 1)
	{
		if (!MoonFlag)
		{
			MoonFlag = true;
			SetModel
			(
				2,
				D3DXVECTOR3((GetEnemy() + ENEMY_HEAD)->Pos.x, (GetEnemy() + ENEMY_HEAD)->Pos.y + 68.0f, (GetEnemy() + ENEMY_HEAD)->Pos.z + 32.0f),
				//�f�o�b�O�p�F
				//D3DXVECTOR3((GetEnemy() + ENEMY_HEAD)->Pos.x, (GetEnemy() + ENEMY_HEAD)->Pos.y, (GetEnemy() + ENEMY_HEAD)->Pos.z),
				D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(0.0f), D3DXToRadian(45.0f)),
				D3DXVECTOR3(3.0f, 3.0f, 3.0f)
			);
		}
	}
	if (MoonFlag)
	{
		{
			//���̊p�x�����傢������
			PlusModelRot(2, D3DXToRadian(0.0f), D3DXToRadian(0.001f), D3DXToRadian(0.0f));
		}
	}
	DrawModel();
	if (Getg_PunchFrameCnt() == 329)
	{
		CntFrame++;
	}
	
	//==============================UFO�̕`��
	if (AirplaneFlag == true && Getg_PunchFrameCnt() >= rand() % 180 + 80 && CntFrame == 1)
	{
		DrawUFO();

		if (!UFO_AwayFlag)
		{
			if (g_posUFO.z >= 220.0f)
			{
				AddUFO_Z -= 10.0f;
			}
			else if (g_posUFO.z < 220.0f)
			{
				AddUFO_Y -= 6.0f;
				AddUFO_Z -= 5.5f;

				if (g_posUFO.y <= 740.0f && g_posUFO.z <= 162.0f)
				{
					UFO_AwayFlag = true;
				}
			}
		}
		else
		{
			AddUFO_Y += 8.0f;
			AddUFO_Z -= 0.5f;
		}
	}
}
//=============================
// ��̌��ݕ`��ty���擾
//=============================
float GetSky_ty(void)
{
	return ty;
}
//=============================
// UFO���̕`��
//=============================
void DrawModel()
{
	//Direct3DDevice�̎擾
	LPDIRECT3DDEVICE9 p_D3DDevice = GetD3DDevice();

	//���[���h�g�����X�t�H�[��(��΍��W�ϊ�)
	D3DXMatrixIdentity(&g_mtxWorldSky);
	
	if (AirplaneFlag)
	{
		//���[���h�}�g���N�X����ēK�p
		p_D3DDevice->SetTransform(D3DTS_WORLD, MakeWorldMatrix(&g_SkyModel[0], &g_mtxWorldModel));

		//�}�e���A���̐������J��Ԃ�
		for (DWORD j = 0; j < g_SkyModel[0].dwNumMaterials; j++)
		{
			//�}�e���A���Z�b�g
			p_D3DDevice->SetMaterial(&g_SkyModel[0].pMaterials[j]);
			//�e�N�X�`���Z�b�g
			p_D3DDevice->SetTexture(0, g_SkyModel[0].pTextures[j]);
			//���ߓx�ݒ�
			g_SkyModel[0].pMaterials[j].Diffuse.a = 1.0f;
			//������`��
			g_SkyModel[0].pMesh->DrawSubset(j);
		}
	}
	if (UFOFlag)
	{
		//���[���h�}�g���N�X����ēK�p
		p_D3DDevice->SetTransform(D3DTS_WORLD, MakeWorldMatrix(&g_SkyModel[1], &g_mtxWorldModel));

		//�}�e���A���̐������J��Ԃ�
		for (DWORD j = 0; j < g_SkyModel[1].dwNumMaterials; j++)
		{
			//�}�e���A���Z�b�g
			p_D3DDevice->SetMaterial(&g_SkyModel[1].pMaterials[j]);
			//�e�N�X�`���Z�b�g
			p_D3DDevice->SetTexture(0, g_SkyModel[1].pTextures[j]);
			//���ߓx�ݒ�
			g_SkyModel[1].pMaterials[j].Diffuse.a = 1.0f;
			//������`��
			g_SkyModel[1].pMesh->DrawSubset(j);
		}
	}
	if (MoonFlag)
	{
		//���[���h�}�g���N�X����ēK�p
		p_D3DDevice->SetTransform(D3DTS_WORLD, MakeWorldMatrix(&g_SkyModel[2], &g_mtxWorldModel));

		//�}�e���A���̐������J��Ԃ�
		for (DWORD j = 0; j < g_SkyModel[2].dwNumMaterials; j++)
		{
			//�}�e���A���Z�b�g
			p_D3DDevice->SetMaterial(&g_SkyModel[2].pMaterials[j]);
			//�e�N�X�`���Z�b�g
			p_D3DDevice->SetTexture(0, g_SkyModel[2].pTextures[j]);
			//���ߓx�ݒ�
			g_SkyModel[2].pMaterials[j].Diffuse.a = 1.0f;
			//������`��
			g_SkyModel[2].pMesh->DrawSubset(j);
		}
	}
	
}

//���W�Ƃ��Z�b�g
void SetModel(int ModelNum,D3DXVECTOR3 Pos, D3DXVECTOR3 Rot,D3DXVECTOR3 Scale)
{
	g_SkyModel[ModelNum].Pos = Pos;
	g_SkyModel[ModelNum].Rot = Rot;
	g_SkyModel[ModelNum].Scale = Scale;
}
void PlusModelPos(int ModelNum, float x,float y,float z)
{
	g_SkyModel[ModelNum].Pos.x += x;
	g_SkyModel[ModelNum].Pos.y += y;
	g_SkyModel[ModelNum].Pos.z += z;
}
void PlusModelRot(int ModelNum, float x, float y, float z)
{
	g_SkyModel[ModelNum].Rot.x += x;
	g_SkyModel[ModelNum].Rot.y += y;
	g_SkyModel[ModelNum].Rot.z += z;
}
bool GetMoonFlag()
{
	return MoonFlag;
}
float GetMoonPos_y()
{
	return g_SkyModel[2].Pos.y;
}

void InitUFO(LPDIRECT3DDEVICE9 pDevice)
{
	// �I�u�W�F�N�g�̒��_�o�b�t�@�𐶐�
	if (FAILED(pDevice->CreateVertexBuffer
	(
		sizeof(VERTEX_3D) * NUM_VERTEX,	// ���_�f�[�^�p�Ɋm�ۂ���o�b�t�@�T�C�Y(�o�C�g�P��)
		D3DUSAGE_WRITEONLY,			// ���_�o�b�t�@�̎g�p�@�@
		FVF_VERTEX_3D,				// �g�p���钸�_�t�H�[�}�b�g
		D3DPOOL_MANAGED,			// ���\�[�X�̃o�b�t�@��ێ����郁�����N���X���w��
		&g_pVtxBuffUFO,				// ���_�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^
		NULL)))						// NULL�ɐݒ�
	{
	}
	{//���_�o�b�t�@�̒��g�𖄂߂�
		VERTEX_3D *pVtx;

		// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
		g_pVtxBuffUFO->Lock(0, 0, (void**)&pVtx, 0);

		// ���_���W�̐ݒ�
		pVtx[0].pos = D3DXVECTOR3(-100.0f / 2,  50.0f / 2, 0.0f);
		pVtx[1].pos = D3DXVECTOR3( 100.0f / 2,  50.0f / 2, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-100.0f / 2, -50.0f / 2, 0.0f);
		pVtx[3].pos = D3DXVECTOR3( 100.0f / 2, -50.0f / 2, 0.0f); 

		// �@���̐ݒ�
		pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

		// ���_�J���[�̐ݒ�
		pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// �e�N�X�`�����W�̐ݒ�
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		// ���_�f�[�^���A�����b�N����
		g_pVtxBuffUFO->Unlock();
	}

	//UFO�̏����ݒ�===================================================
	g_posUFO = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_sclUFO = D3DXVECTOR3(0.2f, 0.2f, 0.2f);
	g_rotUFO = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));
}
void DrawUFO()
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();
	D3DXMATRIX mtxView, mtxScale, mtxTranslate, mtxRot;
	//�A���t�@�e�X�g��L����
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	//���C�e�B���O�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	//���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&g_mtxWorldUFO);

	//�r���[�}�g���b�N�X���擾
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	//POLYGON�𐳖ʂɌ�����
	#if 1
		//�t�s������߂�
		D3DXMatrixInverse(&g_mtxWorldUFO, NULL, &mtxView);
		g_mtxWorldUFO._41 = 0.0f;
		g_mtxWorldUFO._42 = 0.0f;
		g_mtxWorldUFO._43 = 0.0f;
#else
		g_mtxWorldUFO._11 = mtxViex._11;
		g_mtxWorldUFO._12 = mtxViex._21;
		g_mtxWorldUFO._13 = mtxViex._31;
		g_mtxWorldUFO._21 = mtxViex._12;
		g_mtxWorldUFO._22 = mtxViex._22;
		g_mtxWorldUFO._23 = mtxViex._32;
		g_mtxWorldUFO._31 = mtxViex._13;
		g_mtxWorldUFO._32 = mtxViex._23;
		g_mtxWorldUFO._33 = mtxViex._33;
#endif

	//�X�P�[���𔽉f
	D3DXMatrixScaling(&mtxScale, g_sclUFO.x, g_sclUFO.y, g_sclUFO.z);
	D3DXMatrixMultiply(&g_mtxWorldUFO, &g_mtxWorldUFO, &mtxScale);

	//�ړ��𔽉f
	D3DXMatrixTranslation(&mtxTranslate, g_posUFO.x, g_posUFO.y, g_posUFO.z);
	D3DXMatrixMultiply(&g_mtxWorldUFO, &g_mtxWorldUFO, &mtxTranslate);

	//��]�𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_rotUFO.y, g_rotUFO.z, g_rotUFO.x);
	D3DXMatrixMultiply(&g_mtxWorldUFO, &g_mtxWorldUFO, &mtxRot);

	// ���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldUFO);

	//���_�o�b�t�@���f�o�C�X�̃f�[�^�X�g���[���Ƀo�C���h
	pDevice->SetStreamSource(0, g_pVtxBuffUFO, 0, sizeof(VERTEX_3D));

	//���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_3D);

	D3DMATERIAL9 mat = {};

	mat.Diffuse.r = 1.0f;
	mat.Diffuse.g = 1.0f;
	mat.Diffuse.b = 1.0f;
	mat.Diffuse.a = 1.0f;

	mat.Ambient.r = 1.0f;
	mat.Ambient.g = 1.0f;
	mat.Ambient.b = 1.0f;
	mat.Ambient.a = 1.0f;

	//�}�e���A���Z�b�g
	pDevice->SetMaterial(&mat);

	//�e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, Texture_GetTexture(TEXTURE_INDEX_UFO));

	//POLYGON�̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 12);

	//���C�e�B���O��L���ɂ���
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	//�A���t�@�e�X�g�𖳌���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

}