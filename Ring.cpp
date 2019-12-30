#include "Ring.h"
#include "field.h"
#include "myDirect3D.h"
#include "input.h"
#include "texture.h"
#include "debugproc.h"
#include "Xfile.h"
#include "sprite.h"

//���f���̐�
#define RING_MONEL_NUM		(3)

// �ړ���
#define	MODEL_MOVE		(1.0f)

//���f���̐����p��
XMODEL g_Ring_model[1] = {};

//���[���h�}�g���N�X�͊�{1�A�󋵂ɉ�����(�̂Ǝ��"���S��"�ʁX�ɓ�����������)���₵�Ďg���Ă�
D3DXMATRIXA16 g_mtxRingWorld;

HRESULT InitRing()
{
	//�����O�̃��f����ǂݍ���ł݂�
	if (FAILED(LoadXFile(&g_Ring_model[0], XFILE_INDEX_RING)))
	{
		return E_FAIL;
	}

	//���W�A��]�A�g�k�@������
	RingPosReset();

	return S_OK;
}

//�I�����������
void UninitRing()
{
	//���b�V���������
	for (int i = 0; i < 1; i++)
	{
		SAFE_RELEASE(g_Ring_model[i].pMesh);
	}
}

void UpdateRing()
{

}

void DrawRing()
{
	//Direct3DDevice�̎擾
	LPDIRECT3DDEVICE9 p_D3DDevice = GetD3DDevice();

	//���[���h�g�����X�t�H�[��(��΍��W�ϊ�)
	D3DXMatrixIdentity(&g_mtxRingWorld);

	//���f�����J��Ԃ�
	for (int j = 0; j < RING_MONEL_NUM; j++)
	{
		//���[���h�}�g���N�X����ēK�p
		p_D3DDevice->SetTransform(D3DTS_WORLD, MakeWorldMatrix(&g_Ring_model[j], &g_mtxRingWorld));

		//�}�e���A���̐������J��Ԃ�
		for (DWORD i = 0; i < g_Ring_model[j].dwNumMaterials; i++)
		{
			//�}�e���A���Z�b�g
			p_D3DDevice->SetMaterial(&g_Ring_model[j].pMaterials[i]);
			//�e�N�X�`���Z�b�g
			p_D3DDevice->SetTexture(0, g_Ring_model[j].pTextures[i]);
			//���ߓx�ݒ�
			g_Ring_model[j].pMaterials[i].Diffuse.a = 1.0f;
			//������`��
			g_Ring_model[j].pMesh->DrawSubset(i);
		}
	}
}

//���W�Ƃ����Z�b�g
void RingPosReset()
{
	//�����OA
	g_Ring_model[0].Pos = D3DXVECTOR3(2.0f, 0.0f, 10.0f);
	g_Ring_model[0].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));
	g_Ring_model[0].Scale = D3DXVECTOR3(1.0f,1.0f, 1.0f);
}

