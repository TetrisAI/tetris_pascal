//ai.cpp
//Code by ax_pokl

//内联函数
inline double Max(double r1,double r2){if(r1>r2)return r1;else return r2;}
inline int Abs(int i){if(i<0)return -i;else return i;}


//方块类型结构
struct block
{
  //方块种类
  int Kind;
  //方块旋转
  int Rot;
  //方块横坐标
  int PosX;
  //方块纵坐标
  int PosY;
};

//方块常量
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

//方块最大旋转
const int PieceRotMax[8]={0,0,1,1,1,3,3,3};

//最大深度
const int DepthMax = 5;
//最大宽度
const int WidthMax = 32;
//最大高度
const int HeightMax = 32;

//全局计数变量
int i, j, k, l, m, n;

//深度
int Depth = 1;
//猜块数
int Guess = 0;
//宽度
int BoardWidth = 10;
//高度
int BoardHeight = 20;
//盘面数组
int Board[HeightMax][DepthMax];

//方块
block Piece[DepthMax + 1];
//尝试方块
block PieceTry[DepthMax + 1];

//满行
int LineFull = 0;
//空行
int LineNull = 0;

//当前块行数
int LandHeight = 0;
//消行数
int EraseCount = 0;
//列变换
int ColTrans = 0;
//行变换
int RowTrans = 0;
//变换行
int LineTrans = 0;
//洞列
int HoleNum[HeightMax];
//洞数
int HoleCount = 0;
//洞行数
int HoleLine = 0;
//无洞行
int LineCover = 0;
//无洞检测行
int LineCoverCheck = 0;
//有洞行
int LineHole = 0;
//最高有洞行行数
int HoleTop = 0;
//最高有洞行
int LineHoleTop = 0;
//最高有洞检测行
int LineCheck = 0;
//最高洞上方块数
int HolePiece = 0;
//洞深
int HoleDepth = 0;
//井列
int WellNum[HeightMax];
//井深
int WellDepth = 0;
//左中右平衡破缺
int Middle = 0;

//最优旋转
int BestRot = 0;
//最优移动
int BestPos = 0;
//最优横坐标
int BestPosX = 0;

//消行
void Erase(int DepthCur)
{
  //初始化写入行行数
  j = 1;
  //厉遍读取行行数高度
  for (i = 1; i <= BoardHeight; i++)
  {
    //如果读取行不为满行
    if (Board[i][DepthCur] != LineFull)
    {
      //写入该行
      Board[j][DepthCur] = Board[i][DepthCur];
      //写入行行数加一
      j++;
    }
  }
  //设置消行数
  EraseCount = i - j;
}

//固定方块
void Fix(int DepthCur)
{
  //厉遍高度
  for (j = 1; j <= BoardHeight; j++)
  {
    //重置当前深度盘面为前一深度盘面
    Board[j][DepthCur] = Board[j][DepthCur - 1];
  }
  //厉遍方块常量行高度
  for (j = 0; j <= 3; j++)
  {
    //如果行数没有到底线
    if (PieceTry[DepthCur].PosY + j - 2 >= 0)
    {
      //当前行和方块常量行进行或运算
			Board[PieceTry[DepthCur].PosY+j-2][DepthCur]|=
				(PieceBitmap[PieceTry[DepthCur].Kind-1][PieceTry[DepthCur].Rot-1][j]<<
				(BoardWidth-PieceTry[DepthCur].PosX+1));
    }
  }
}

//判断重叠
bool Overlap(int DepthCur)
{
  //厉遍方块常量行高度
  for (j = 0; j <= 3; j++)
  {
    //如果行数没有到底线
    if (PieceTry[DepthCur].PosY + j - 2 >= 0)
    {
      //如果当前行和方块常量行进行与运算为不零
			if((Board[PieceTry[DepthCur].PosY+j-2][DepthCur-1]&
				(PieceBitmap[PieceTry[DepthCur].Kind-1][PieceTry[DepthCur].Rot-1][j]<<
				(BoardWidth-PieceTry[DepthCur].PosX+1)))!=0)
      {
        //返回真
        return true;
      }
    }
  }
  //返回假
  return false;
}

//估值函数
double Evaluate(int DepthCur)
{
  //设置当前块行数
  LandHeight = PieceTry[DepthCur].PosY;
  //初始化列变换
  ColTrans = 0;
  //厉遍高度
  for (j = 1; j <= BoardHeight; j++)
  {
    //对该行和该行右移一格进行异或运算
    LineTrans = (Board[j][DepthCur] ^ (Board[j][DepthCur] >> 1)) >> 1;
    //厉遍宽度
    for (i = 0; i <= BoardWidth; i++)
    {
      //如果最右为真（左右方块不同）则列变换加一
      ColTrans += LineTrans &1;
      //变换行右移
      LineTrans >>= 1;
    }
  }
  //初始化行变换
  RowTrans = 0;
  for (j = 0; j <= BoardHeight - 1; j++)
  {
    //对该行和上一行进行异或运算
    LineTrans = (Board[j][DepthCur] ^ Board[j + 1][DepthCur]) >> 2;
    //厉遍宽度
    for (i = 1; i <= BoardWidth; i++)
    {
      //如果最右为真（上下方块不同）则行变换加一
      RowTrans += LineTrans &1;
      //变换行右移
      LineTrans >>= 1;
    }
  }
  //初始化洞数
  HoleCount = 0;
  //初始化洞行数
  HoleLine = 0;
  //初始化最高洞行数
  HoleTop = 0;
  //初始化最高洞上方块数
  HolePiece = 0;
  //初始化洞深
  HoleDepth = 0;
  //初始化井深
  WellDepth = 0;
  //厉遍宽度
  for (i = 1; i <= BoardWidth; i++)
  {
    //初始化洞列
    HoleNum[i] = 0;
    //初始化井列
    WellNum[i] = 0;
  }
  //初始化无洞行
  LineCover = LineNull;
  //反向厉遍高度
  for (j = BoardHeight; j >= 1; j--)
  {
    //对先前无洞行和当前行进行或运算
    LineCover |= Board[j][DepthCur];
    //对当前无洞行和当前行进行异或运算（结果为洞行）
    LineHole = (LineCover ^ Board[j][DepthCur]) >> 2;
    //当前行有洞
    if (LineHole != 0)
    {
      //洞行数加一
      HoleLine++;
      //如果先前尚未有洞行
      if (HoleTop == 0)
      {
        //设置最高有洞行行数
        HoleTop = j;
        //保存最高有洞行洞状态
        LineHoleTop = LineHole;
      }
    }
    //初始化井行右移
    LineCoverCheck = LineCover >> 1;
    //反向厉遍宽度
    for (i = BoardWidth; i >= 1; i--)
    {
      //如果洞行最右为洞
      if (LineHole &1 == 1)
      {
        //洞数加一
        HoleCount++;
        //洞列加一
        HoleNum[i]++;
      }
      //如果洞行最右非洞
      else
      {
        //洞列清零
        HoleNum[i] = 0;
      }
      //设置洞深加洞列
      HoleDepth += HoleNum[i];
      //洞行右移
      LineHole >>= 1;
      //如果井行最右为井
      if (((LineCoverCheck ^ 5) &7) == 0)
      {
        //井列加一
        WellNum[i]++;
        //设置井深加井列
        WellDepth += WellNum[i];
      }
      //井行右移
      LineCoverCheck >>= 1;
    }
  }
  //如果最高有洞行行数不为零
  if (HoleTop != 0)
  {
    //从最高有洞行上一行开始往上厉遍
    for (j = HoleTop + 1; j <= BoardHeight; j++)
    {
      //设临时行为最高有洞行和当前行的异或变换（把有洞列的方块异或出来）
      LineCheck = LineHoleTop &(Board[j][DepthCur] >> 2);
      //如果有洞列全部没有方块则跳出循环
      if (LineCheck == 0)
      {
        break;
      }
      //厉遍宽度
      for (i = 1; i <= BoardWidth; i++)
      {
        //如果最右为方块则最高有洞行上方块数增加该行行数
        HolePiece += (LineCheck &1) *j;
        //临时行右移
        LineCheck >>= 1;
      }
    }
  }
  //设置左中右平衡破缺参数
  Middle = Abs(PieceTry[DepthCur].PosX *2-BoardWidth);
  //返回估值结果
	return 
		-LandHeight*190/BoardHeight
		+EraseCount*6
		-ColTrans*10
		-RowTrans*10
		-HoleCount*6
		-HoleLine*38
		-WellDepth*10
		-HoleDepth*4
		-HolePiece*10/BoardHeight
		+Middle*0.2;
}

//超前引用
double AIGo(int DepthCur);

//AI厉遍函数
double Calculate(int DepthCur)
{
  //初始化结果
  double ResultMax =  - 0x7FFFFFFF;
  //初始化临时结果
  double Result = 0;
  //初始化方块最大旋转
  int Rot = 0;
  //初始化方块最大左移
  int Left = 0;
  //初始化方块最大右移
  int Right = 0;
  //当方块最大旋转小于方块最大旋转常量时做
  while (Rot < PieceRotMax[PieceTry[DepthCur].Kind])
  {
    //增加尝试方块旋转
    PieceTry[DepthCur].Rot++;
    //如果重叠则跳出循环
    if (Overlap(DepthCur))
    {
      break;
    }
    //否则增加方块最大旋转
    else
    {
      Rot++;
    }
  }
  //厉遍方块最大旋转
  for (int Roti = Piece[DepthCur].Rot; Roti <= Piece[DepthCur].Rot + Rot; Roti++)
  {
    //重置尝试方块旋转
    PieceTry[DepthCur].Rot = Roti;
    //重置尝试方块横坐标
    PieceTry[DepthCur].PosX = Piece[DepthCur].PosX;
    //设方块最大左移为1
    Left = 1;
    //做
    do
    {
      //方块最大左移减一
      Left--;
      //尝试方块横坐标减一
      PieceTry[DepthCur].PosX--;
    }
    //直到重叠
    while (!Overlap(DepthCur));
    //重置尝试方块横坐标
    PieceTry[DepthCur].PosX = Piece[DepthCur].PosX;
    //设方块最大右移为-1
    Right =  - 1;
    //做
    do
    {
      //方块最大右移加一
      Right++;
      //尝试方块横坐标加一
      PieceTry[DepthCur].PosX++;
    }
    //直到重叠
    while (!Overlap(DepthCur));
    //从最左到最右厉遍坐标
    for (int PosXi = Piece[DepthCur].PosX + Left; PosXi <= Piece[DepthCur].PosX + Right; PosXi++)
    {
      //重置尝试方块横坐标为厉遍坐标
      PieceTry[DepthCur].PosX = PosXi;
      //做
      do
      {
        //尝试方块纵坐标减一
        PieceTry[DepthCur].PosY--;
      }
      //直到重叠
      while (!Overlap(DepthCur));
      //尝试方块纵坐标加一
      PieceTry[DepthCur].PosY++;
      //固定方块
      Fix(DepthCur);
      //消行
      Erase(DepthCur);
      //获取估值结果
      Result = Evaluate(DepthCur);
      //如果结果大于最大结果
      if (Result > ResultMax)
      {
        //如果当前深度小于深度
        if (DepthCur < Depth)
        {
          //迭代深入
          //Result+=AiGo(DepthCur+1);//无剪枝
          Result = Max(Result, AIGo(DepthCur + 1)); //剪枝
        }
      }
      //如果结果仍旧大于最大结果
      if (Result > ResultMax)
      {
        //更新最大结果
        ResultMax = Result;
        //如果当前深度是1
        if (DepthCur == 1)
        {
          //更新最优旋转
          BestRot = PieceTry[DepthCur].Rot - Piece[DepthCur].Rot;
          //更新最优移动
          BestPos = PieceTry[DepthCur].PosX - Piece[DepthCur].PosX;
          //更新最优横坐标
          BestPosX = PieceTry[DepthCur].PosX;
        }
      }
      //重置尝试方块纵坐标
      PieceTry[DepthCur].PosY = Piece[DepthCur].PosY;
    }
  }
  //返回结果
  return ResultMax;
}

//AI主函数
double AIGo(int DepthCur)
{
  //初始化结果
  double Result = 0;
  //初始化尝试方块
  PieceTry[DepthCur].Kind = Piece[DepthCur].Kind;
  PieceTry[DepthCur].Rot = Piece[DepthCur].Rot;
  PieceTry[DepthCur].PosX = Piece[DepthCur].PosX;
  PieceTry[DepthCur].PosY = Piece[DepthCur].PosY;
  //如果方块种类为零
  if (PieceTry[DepthCur].Kind == 0)
  {
    //厉遍7种方块
    for (int Kindi = 1; Kindi <= 7; Kindi++)
    {
      //重新初始化尝试方块
      PieceTry[DepthCur].Kind = Kindi;
      PieceTry[DepthCur].Rot = 1;
      PieceTry[DepthCur].PosX = BoardWidth / 2;
      PieceTry[DepthCur].PosY = BoardHeight - 1;
      //叠加结果
      Result += Calculate(DepthCur);
    }
    //取结果为叠加结果平均值
    Result /= 7;
  }
  //如果方块种类不为零
  else
  {
    //直接取结果
    Result = Calculate(DepthCur);
  }
  //返回结果
  return Result;
}

//初始化盘面
void InitialBoard()
{
  //设置满行
  LineFull = 0xFFFFFFFF;
  //设置空行
  LineNull = LineFull >> (BoardWidth + 2) << (BoardWidth + 2) | 3;
  //厉遍深度
  for (k = 0; k < DepthMax; k++)
  {
    //设置底线为满行
    Board[0][k] = LineFull;
    //设置盘面空间为空行
    for (j = 1; j <= BoardHeight + 1; j++)
    {
      Board[j][k] = LineNull;
    }
    //设置天花板为满行
    Board[BoardHeight + 2][k] = LineFull;
  }
}

//初始化方块
void InitialPiece()
{
  //厉遍深度
  for (k = 1; k <= DepthMax; k++)
  {
    //设置种类为0
    Piece[k].Kind = 0;
    //设置旋转为1
    Piece[k].Rot = 1;
    //设置横坐标居中
    Piece[k].PosX = BoardWidth / 2;
    //设置纵坐标居上
    Piece[k].PosY = BoardHeight - 1;
  }
}

//初始化AI
void InitialAI()
{
  //初始化盘面
  InitialBoard();
  //初始化方块
  InitialPiece();
}

//AI主DLL入口
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
  //设置宽度
  BoardWidth = boardW;
  //设置高度
  BoardHeight = boardH;
  //初始化盘面及方块
  InitialAI();
  //设置盘面
  k = 0;
  //厉遍高度
  for (j = 1; j <= BoardHeight; j++)
  {
    //初始化该行
    Board[j][0] = 0;
    //厉遍宽度
    for (i = 1; i <= BoardWidth; i++)
    {
      //如果传入盘面数组元素不为'0'
      if (board[k] != '0')
      //对该行与1进行或运算
      {
        Board[j][0] |= 1;
      }
      //该行左移
      Board[j][0] <<= 1;
      //获取下一数组元素
      k++;
    }
    //该行左移（对齐空行）
    Board[j][0] <<= 1;
    //该行与空行进行或运算（填补墙壁）
    Board[j][0] |= LineNull;
  }
  //设置当前方块种类
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
  //设置深度
  if (!Piece[1].Kind == 0)
  {
    Depth = 1+Guess;
  }
  //设置当前方块位置及状态
  Piece[1].PosX = curX;
  Piece[1].PosY = curY;
  Piece[1].Rot = curR;
  //设置下一块方块种类
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
  //设置深度
  if (Piece[2].Kind != 0)
  {
    Depth = 2+Guess;
  }
  //AI主函数
  AIGo(1);
  //设置返回结果
  *bestX = BestPosX;
  *bestRot = BestRot + 1;
  //返回0
  return 0;
}

//AI名称DLL入口
extern "C" _declspec(dllexport)char *__stdcall Name()
{
  //返回AI名称
  return "Tetris_ax_C";
}
