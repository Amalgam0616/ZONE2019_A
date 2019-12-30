#include "myDirect3D.h"
#include "text.h"

// �O���[�o���ϐ��錾 ===================================================================

//�t�H���g���X�g
static const LPCSTR g_FontList[FONT_INDEX_MAX] =
{
	"Terminal",	//�e�X�g�p
};

// �֐��錾 ============================================================================

//�t�H���g�����֐�
HRESULT Text_CriateFont(LPD3DXFONT *p_Font,int w, int h, FONT_INDEX index, bool isItalic)
{
	//D3DDevice�̎擾
	LPDIRECT3DDEVICE9 p_D3DDevice = GetD3DDevice();

	//�t�H���g�̐���
	if (FAILED(D3DXCreateFontA(p_D3DDevice,			//Direct3D�f�o�C�X
		h, w,										//�����̍����A��
		FW_HEAVY, 1,								//�t�H���g�X�^�C���A�~�b�v�}�b�v�̃��x��(?)
		isItalic,									//�Α�(�C�^���b�N)�ɂ��邩�ǂ���
		SHIFTJIS_CHARSET,							//�����Z�b�g
		OUT_TT_ONLY_PRECIS, ANTIALIASED_QUALITY,	//�o�͐��x�A�o�͕i��
		FF_DONTCARE,								//�t�H���g�s�b�`�ƃt�@�~��
		g_FontList[index], p_Font)))				//�t�H���g���A�t�H���g�|�C���^
	{
		return E_FAIL;
	}

	return S_OK;
}

//�I���֐��̗\�肾�������ǁA�g���ꏊ�ł�������������̂ł��������c���Ƃ��܂�
void UninitText()
{
	//�t�H���g�|�C���^�̉��
	//SAFE_RELEASE(LPD3DXFONT�^�̃t�H���g�|�C���^);
}

//�e�L�X�g�`��
void TextDraw(LPD3DXFONT p_Font,const char *text, int X, int Y, FONT_INDEX index, D3DXCOLOR color)
{
	RECT rect = { X,Y,0,0 };	//�`��ʒu�̍��W�ݒ�
	
	p_Font->DrawTextA(NULL,		//�X�v���C�g�|�C���^(NULL�w���)
		text,					//�`�敶��
		-1,						//������(-1�w��őS���ɂȂ�)
		&rect,					//�`��͈�
		DT_LEFT | DT_NOCLIP,	//�t�H�[�}�b�g
		color);					//�F
}
