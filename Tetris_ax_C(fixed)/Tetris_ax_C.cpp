//Tetris_ax_C.cpp
//Code by ax_pokl

#include <fstream>
using namespace std;
//��������
inline int Max(int r1,int r2){if(r1>r2)return r1;else return r2;}
inline int Abs(int i){if(i<0)return -i;else return i;}



//�������ͽṹ
struct block
{
	//��������
	int Kind;
	//������ת
	int Rot;
	//���������
	int PosX;
	//����������
	int PosY;
};

//���鳣��
const int PieceBitmap[7][8][4] =
{
	{{0,6,6,0},{0,6,6,0},{0,6,6,0},{0,6,6,0},{0,6,6,0},{0,6,6,0},{0,6,6,0},{0,6,6,0}},
	{{0,0,15,0},{2,2,2,2},{0,0,15,0},{2,2,2,2},{0,0,15,0},{2,2,2,2},{0,0,15,0},{2,2,2,2}},
	{{0,6,3,0},{0,1,3,2},{0,6,3,0},{0,1,3,2},{0,6,3,0},{0,1,3,2},{0,6,3,0},{0,1,3,2}},
	{{0,3,6,0},{0,2,3,1},{0,3,6,0},{0,2,3,1},{0,3,6,0},{0,2,3,1},{0,3,6,0},{0,2,3,1}},
	{{0,4,7,0},{0,3,2,2},{0,0,7,1},{0,2,2,6},{0,4,7,0},{0,3,2,2},{0,0,7,1},{0,2,2,6}},
	{{0,1,7,0},{0,2,2,3},{0,0,7,4},{0,6,2,2},{0,1,7,0},{0,2,2,3},{0,0,7,4},{0,6,2,2}},
	{{0,2,7,0},{0,2,3,2},{0,0,7,2},{0,2,6,2},{0,2,7,0},{0,2,3,2},{0,0,7,2},{0,2,6,2}}
};

//���������ת
const int PieceRotMax[8]={0,0,1,1,1,3,3,3};

//������
const int DepthMax = 5;
//�����
const int WidthMax = 32;
//���߶�
const int HeightMax = 32;

//ȫ�ּ�������
int i, j, k, l, m, n;

//���
int Depth = 1;
//�¿���
int Guess = 0;
//���
int BoardWidth = 10;
//�߶�
int BoardHeight = 20;
//��������
int Board[HeightMax][DepthMax];

//����
block Piece[DepthMax + 1];
//���Է���
block PieceTry[DepthMax + 1];

//����
int LineFull = 0;
//����
int LineNull = 0;

//��ǰ������
int LandHeight[DepthMax + 1];
//������
int EraseCount[DepthMax + 1];
//�б任
int ColTrans = 0;
//�б任
int RowTrans = 0;
//�任��
int LineTrans = 0;
//����
int HoleNum[HeightMax];
//����
int HoleCount = 0;
//������
int HoleLine = 0;
//�޶���
int LineCover = 0;
//�޶������
int LineCoverCheck = 0;
//�ж���
int LineHole = 0;
//����ж�������
int HoleTop = 0;
//����ж���
int LineHoleTop = 0;
//����ж������
int LineCheck = 0;
//��߶��Ϸ�����
int HolePiece = 0;
//����
int HoleDepth = 0;
//����
int WellNum[HeightMax];
//����
int WellDepth = 0;
//������ƽ����ȱ
int Middle = 0;
//��ֵ���
int result = 0;

//������ת
int BestRot = 0;
//�����ƶ�
int BestPos = 0;
//���ź�����
int BestPosX = 0;

//����
void Erase(int DepthCur)
{
	//��ʼ��д��������
	j = 1;
	//�����ȡ�������߶�
	for (i = 1; i <= BoardHeight; i++)
	{
		//�����ȡ�в�Ϊ����
		if (Board[i][DepthCur] != LineFull)
		{
			//д�����
			Board[j][DepthCur] = Board[i][DepthCur];
			//д����������һ
			j++;
		}
	}
	//����������
	EraseCount[DepthCur] = i - j;
}

//�̶�����
void Fix(int DepthCur)
{
	//����߶�
	for (j = 1; j <= BoardHeight; j++)
	{
		//���õ�ǰ�������Ϊǰһ�������
		Board[j][DepthCur] = Board[j][DepthCur - 1];
	}
	//���鷽�鳣���и߶�
	for (j = 0; j <= 3; j++)
	{
		//�������û�е�����
		if (PieceTry[DepthCur].PosY + j - 2 >= 0)
		{
			//��ǰ�кͷ��鳣���н��л�����
			Board[PieceTry[DepthCur].PosY+j-2][DepthCur]|=
				(PieceBitmap[PieceTry[DepthCur].Kind-1][PieceTry[DepthCur].Rot-1][j]<<
				(BoardWidth-PieceTry[DepthCur].PosX+1));
		}
	}
}

//�ж��ص�
bool Overlap(int DepthCur)
{
	//���鷽�鳣���и߶�
	for (j = 0; j <= 3; j++)
	{
		//�������û�е�����
		if (PieceTry[DepthCur].PosY + j - 2 >= 0)
		{
			//�����ǰ�кͷ��鳣���н���������Ϊ����
			if((Board[PieceTry[DepthCur].PosY+j-2][DepthCur-1]&
				(PieceBitmap[PieceTry[DepthCur].Kind-1][PieceTry[DepthCur].Rot-1][j]<<
				(BoardWidth-PieceTry[DepthCur].PosX+1)))!=0)
			{
				//������
				return true;
			}
		}
	}
	//���ؼ�
	return false;
}

//��������ص������
int Overlapk(int DepthCur)
{
	n = 0;
	for (m = 1; m <= 7; m++)
	{
		//���鷽�鳣���и߶�
		for (j = 0; j <= 3; j++)
		{
			//�������û�е�����
			if (BoardHeight - 1+j - 2 >= 0)
			{
				//�����ǰ�кͷ��鳣���н���������Ϊ����
				if((Board[BoardHeight-1+j-2][DepthCur]&
					(PieceBitmap[m-1][1-1][j]<<
					(BoardWidth-BoardWidth/2)))!=0)
				{
					n++;
				}
			}
		}
	}
	return n;
}

//��ֵ����
int Evaluate(int DepthCur)
{
	//���õ�ǰ������
	LandHeight[DepthCur] = PieceTry[DepthCur].PosY;
	//��ʼ���б任
	ColTrans = 0;
	//����߶�
	for (j = 1; j <= BoardHeight; j++)
	{
		//�Ը��к͸�������һ������������
		LineTrans = (Board[j][DepthCur] ^ (Board[j][DepthCur] >> 1)) >> 1;
		//������
		for (i = 0; i <= BoardWidth; i++)
		{
			//�������Ϊ�棨���ҷ��鲻ͬ�����б任��һ
			ColTrans += LineTrans &1;
			//�任������
			LineTrans >>= 1;
		}
	}
	//��ʼ���б任
	RowTrans = 0;
	for (j = 0; j <= BoardHeight - 1; j++)
	{
		//�Ը��к���һ�н����������
		LineTrans = (Board[j][DepthCur] ^ Board[j + 1][DepthCur]) >> 2;
		//������
		for (i = 1; i <= BoardWidth; i++)
		{
			//�������Ϊ�棨���·��鲻ͬ�����б任��һ
			RowTrans += LineTrans &1;
			//�任������
			LineTrans >>= 1;
		}
	}
	//��ʼ������
	HoleCount = 0;
	//��ʼ��������
	HoleLine = 0;
	//��ʼ����߶�����
	HoleTop = 0;
	//��ʼ����߶��Ϸ�����
	HolePiece = 0;
	//��ʼ������
	HoleDepth = 0;
	//��ʼ������
	WellDepth = 0;
	//������
	for (i = 1; i <= BoardWidth; i++)
	{
		//��ʼ������
		HoleNum[i] = 0;
		//��ʼ������
		WellNum[i] = 0;
	}
	//��ʼ���޶���
	LineCover = LineNull;
	//��������߶�
	for (j = BoardHeight; j >= 1; j--)
	{
		//����ǰ�޶��к͵�ǰ�н��л�����
		LineCover |= Board[j][DepthCur];
		//�Ե�ǰ�޶��к͵�ǰ�н���������㣨���Ϊ���У�
		LineHole = (LineCover ^ Board[j][DepthCur]) >> 2;
		//��ǰ���ж�
		if (LineHole != 0)
		{
			//��������һ
			HoleLine++;
			//�����ǰ��δ�ж���
			if (HoleTop == 0)
			{
				//��������ж�������
				HoleTop = j;
				//��������ж��ж�״̬
				LineHoleTop = LineHole;
			}
		}
		//��ʼ����������
		LineCoverCheck = LineCover >> 1;
		//����������
		for (i = BoardWidth; i >= 1; i--)
		{
			//�����������Ϊ��
			if (LineHole &1 == 1)
			{
				//������һ
				HoleCount++;
				//���м�һ
				HoleNum[i]++;
			}
			//����������ҷǶ�
			else
			{
				//��������
				HoleNum[i] = 0;
			}
			//���ö���Ӷ���
			HoleDepth += HoleNum[i];
			//��������
			LineHole >>= 1;
			//�����������Ϊ��
			if (((LineCoverCheck ^ 5) &7) == 0)
			{
				//���м�һ
				WellNum[i]++;
				//���þ���Ӿ���
				WellDepth += WellNum[i];
			}
			//��������
			LineCoverCheck >>= 1;
		}
	}
	//�������ж���������Ϊ��
	if (HoleTop != 0)
	{
		//������ж�����һ�п�ʼ��������
		for (j = HoleTop + 1; j <= BoardHeight; j++)
		{
			//����ʱ��Ϊ����ж��к͵�ǰ�е����任�����ж��еķ�����������
			LineCheck = LineHoleTop &(Board[j][DepthCur] >> 2);
			//����ж���ȫ��û�з���������ѭ��
			if (LineCheck == 0)
			{
				break;
			}
			//������
			for (i = 1; i <= BoardWidth; i++)
			{
				//�������Ϊ����������ж����Ϸ��������Ӹ�������
				HolePiece += (LineCheck &1) *j;
				//��ʱ������
				LineCheck >>= 1;
			}
		}
	}
	//����������ƽ����ȱ����
	Middle = Abs(PieceTry[DepthCur].PosX *2-BoardWidth);
	//���ع�ֵ���
	result = 
	  -ColTrans*80
	  -RowTrans*80
	  -HoleCount*60
	  -HoleLine*380
	  -WellDepth*100
	  -HoleDepth*40
	  -HolePiece*5//100/BoardHeight
	 + Middle * 2;
	for (i = 1; i <= DepthCur; i++)
	{
		result +=
		  -LandHeight[i]*1750/(BoardHeight*DepthCur)
		  +EraseCount[i]*60/DepthCur;
	}
	result += Overlapk(DepthCur);
	/*
	ofstream setting_file_out;
	setting_file_out.open("o2.txt",ios::app);
	setting_file_out<<Piece[DepthCur].Kind<<" ";
	setting_file_out<<Piece[DepthCur].Rot<<" ";
	setting_file_out<<Piece[DepthCur].PosX<<" ";
	setting_file_out<<Piece[DepthCur].PosY<<" ";
	setting_file_out<<PieceTry[DepthCur].Kind<<" ";
	setting_file_out<<PieceTry[DepthCur].Rot<<" ";
	setting_file_out<<PieceTry[DepthCur].PosX<<" ";
	setting_file_out<<PieceTry[DepthCur].PosY<<" ";
	setting_file_out<<ColTrans<<" ";
	setting_file_out<<RowTrans<<" ";
	setting_file_out<<HoleCount<<" ";
	setting_file_out<<HoleLine<<" ";
	setting_file_out<<WellDepth<<" ";
	setting_file_out<<HoleDepth<<" ";
	setting_file_out<<HolePiece<<" ";
	setting_file_out<<-Middle<<" ";
	for (i = 1; i <= DepthCur; i++)
	{
	setting_file_out<<LandHeight[i]<<" ";
	setting_file_out<<-EraseCount[i]<<" ";
	}
	setting_file_out<<-result<<endl;
	setting_file_out.close();
	 */
	return result;
}

//��ǰ����
int AIGo(int DepthCur);

//AI���麯��
int Calculate(int DepthCur)
{
	//��ʼ�����
	int ResultMax =  - 0x7FFFFFFF;
	//��ʼ����ʱ���
	int Result = 0;
	//��ʼ�����������ת
	int Rot = 0;
	//��ʼ�������������
	int Left = 0;
	//��ʼ�������������
	int Right = 0;
	//�����������תС�ڷ��������ת����ʱ��
	while (Rot < PieceRotMax[PieceTry[DepthCur].Kind])
	{
		//���ӳ��Է�����ת
		PieceTry[DepthCur].Rot++;
		//����ص�������ѭ��
		if (Overlap(DepthCur))
		{
			break;
		}
		//�������ӷ��������ת
		else
		{
			Rot++;
		}
	}
	//���鷽�������ת
	for (int Roti = Piece[DepthCur].Rot; Roti <= Piece[DepthCur].Rot + Rot; Roti++)
	{
		//���ó��Է�����ת
		PieceTry[DepthCur].Rot = Roti;
		//���ó��Է��������
		PieceTry[DepthCur].PosX = Piece[DepthCur].PosX;
		//�跽���������Ϊ1
		Left = 1;
		//��
		do
		{
			//����������Ƽ�һ
			Left--;
			//���Է���������һ
			PieceTry[DepthCur].PosX--;
		}
		//ֱ���ص�
		while (!Overlap(DepthCur));
		//���ó��Է��������
		PieceTry[DepthCur].PosX = Piece[DepthCur].PosX;
		//�跽���������Ϊ-1
		Right =  - 1;
		//��
		do
		{
			//����������Ƽ�һ
			Right++;
			//���Է���������һ
			PieceTry[DepthCur].PosX++;
		}
		//ֱ���ص�
		while (!Overlap(DepthCur));
		//������������������
		for (int PosXi = Piece[DepthCur].PosX + Left; PosXi <= Piece[DepthCur].PosX + Right; PosXi++)
		{
			//���ó��Է��������Ϊ��������
			PieceTry[DepthCur].PosX = PosXi;
			//��
			do
			{
				//���Է����������һ
				PieceTry[DepthCur].PosY--;
			}
			//ֱ���ص�
			while (!Overlap(DepthCur));
			//���Է����������һ
			PieceTry[DepthCur].PosY++;
			//�̶�����
			Fix(DepthCur);
			//����
			Erase(DepthCur);
			//��ȡ��ֵ���
			Result = Evaluate(DepthCur);
			//�����ǰ���С�����
			if (DepthCur < Depth)
			{
				//��������
				Result = AIGo(DepthCur + 1);
			}
			//�������Ծɴ��������
			if (Result > ResultMax)
			{
				//���������
				ResultMax = Result;
				//�����ǰ�����1
				if (DepthCur == 1)
				{
					//����������ת
					BestRot = PieceTry[DepthCur].Rot - Piece[DepthCur].Rot;
					//���������ƶ�
					BestPos = PieceTry[DepthCur].PosX - Piece[DepthCur].PosX;
					//�������ź�����
					BestPosX = PieceTry[DepthCur].PosX;
				}
			}
			//���ó��Է���������
			PieceTry[DepthCur].PosY = Piece[DepthCur].PosY;

			/*
			ofstream setting_file_out;
			setting_file_out.open("o2.txt",ios::app);
			setting_file_out<<BestRot<<" ";
			setting_file_out<<BestPosX<<" ";
			setting_file_out<<Result<<" ";
			setting_file_out<<ResultMax<<" ";
			setting_file_out<<endl;
			setting_file_out.close();
			 */

		}
	}
	//���ؽ��
	return ResultMax;
}

//AI������
int AIGo(int DepthCur)
{
	//��ʼ�����
	int Result = 0;
	//��ʼ�����Է���
	PieceTry[DepthCur].Kind = Piece[DepthCur].Kind;
	PieceTry[DepthCur].Rot = Piece[DepthCur].Rot;
	PieceTry[DepthCur].PosX = Piece[DepthCur].PosX;
	PieceTry[DepthCur].PosY = Piece[DepthCur].PosY;
	//�����������Ϊ��
	if (PieceTry[DepthCur].Kind == 0)
	{
		//����7�ַ���
		for (int Kindi = 1; Kindi <= 7; Kindi++)
		{
			//���³�ʼ�����Է���
			PieceTry[DepthCur].Kind = Kindi;
			PieceTry[DepthCur].Rot = 1;
			PieceTry[DepthCur].PosX = BoardWidth / 2;
			PieceTry[DepthCur].PosY = BoardHeight - 1;
			//���ӽ��
			Result += Calculate(DepthCur);
		}
		//ȡ���Ϊ���ӽ��ƽ��ֵ
		Result /= 7;
	}
	//����������಻Ϊ��
	else
	{
		//ֱ��ȡ���
		Result = Calculate(DepthCur);
	}
	//���ؽ��
	return Result;
}

//��ʼ������
void InitialBoard()
{
	//��������
	LineFull = 0xFFFFFFFF;
	//���ÿ���
	LineNull = LineFull >> (BoardWidth + 2) << (BoardWidth + 2) | 3;
	//�������
	for (k = 0; k < DepthMax; k++)
	{
		//���õ���Ϊ����
		Board[0][k] = LineFull;
		//��������ռ�Ϊ����
		for (j = 1; j <= BoardHeight + 1; j++)
		{
			Board[j][k] = LineNull;
		}
		//�����컨��Ϊ����
		Board[BoardHeight + 2][k] = LineFull;
	}
}

//��ʼ������
void InitialPiece()
{
	//�������
	for (k = 1; k <= DepthMax; k++)
	{
		//��������Ϊ0
		Piece[k].Kind = 0;
		//������תΪ1
		Piece[k].Rot = 1;
		//���ú��������
		Piece[k].PosX = BoardWidth / 2;
		//�������������
		Piece[k].PosY = BoardHeight - 1;
	}
}

//��ʼ��AI
void InitialAI()
{
	//��ʼ������
	InitialBoard();
	//��ʼ������
	InitialPiece();
}

//AI��DLL���
extern "C" _declspec(dllexport) int __stdcall AI(
		int boardW, int boardH,
		char board[],
		char curPiece,
		int curX,
		int curY,
		int curR,
		char nextPiece,
		int* bestX,
		int* bestRot)
{
	//���ÿ��
	BoardWidth = boardW;
	//���ø߶�
	BoardHeight = boardH;
	//��ʼ�����漰����
	InitialAI();
	//��������
	k = 0;
	//����߶�
	for (j = 1; j <= BoardHeight; j++)
	{
		//��ʼ������
		Board[j][0] = 0;
		//������
		for (i = 1; i <= BoardWidth; i++)
		{
			//���������������Ԫ�ز�Ϊ'0'
			if (board[k] != '0')
			//�Ը�����1���л�����
			{
				Board[j][0] |= 1;
			}
			//��������
			Board[j][0] <<= 1;
			//��ȡ��һ����Ԫ��
			k++;
		}
		//�������ƣ�������У�
		Board[j][0] <<= 1;
		//��������н��л����㣨�ǽ�ڣ�
		Board[j][0] |= LineNull;
	}
	//���õ�ǰ��������
	switch (curPiece)
	{
		case 'O':
			Piece[1].Kind = 1;
			break;
		case 'I':
			Piece[1].Kind = 2;
			break;
		case 'S':
			Piece[1].Kind = 3;
			break;
		case 'Z':
			Piece[1].Kind = 4;
			break;
		case 'L':
			Piece[1].Kind = 5;
			break;
		case 'J':
			Piece[1].Kind = 6;
			break;
		case 'T':
			Piece[1].Kind = 7;
			break;
		default:
			Piece[1].Kind = 0;
	}
	//�������
	if (!Piece[1].Kind == 0)
	{
		Depth = 1+Guess;
	}
	//���õ�ǰ����λ�ü�״̬
	Piece[1].PosX = curX;
	Piece[1].PosY = curY;
	Piece[1].Rot = curR;
	//������һ�鷽������
	switch (nextPiece)
	{
		case 'O':
			Piece[2].Kind = 1;
			break;
		case 'I':
			Piece[2].Kind = 2;
			break;
		case 'S':
			Piece[2].Kind = 3;
			break;
		case 'Z':
			Piece[2].Kind = 4;
			break;
		case 'L':
			Piece[2].Kind = 5;
			break;
		case 'J':
			Piece[2].Kind = 6;
			break;
		case 'T':
			Piece[2].Kind = 7;
			break;
		default:
			Piece[2].Kind = 0;
	}
	//�������
	if (Piece[2].Kind != 0)
	{
		Depth = 2+Guess;
	}
	//AI������
	AIGo(1);
	//���÷��ؽ��
	*bestX = BestPosX;
	*bestRot = BestRot + 1;
	//����0
	return 0;
}

//AI����DLL���
extern "C" _declspec(dllexport)char *__stdcall Name()
{
	//����AI����
	return "Tetris_ax_C";
}
