library tetris_lib;

uses display;

{$I tetris_def.inc}

{$I tetris_pce.inc}

{$I tetris_ai.inc}

var
  i, j, k: longint;
const
  guess = 0;

  //AI主DLL入口
function AI(boardW, boardH: longint; board: PChar; curPiece: char;
  curX, curY, curR: longint; nextPiece: char;
  bestX, bestRotation: pinteger): longint; stdcall;
begin
  //设置宽度
  wbd := boardW;
  //设置高度
  hbd := boardH;
  //初始化盘面及方块
  ainew2();
  //设置盘面
  k := 0;
  //厉遍高度
  for j := 1 to hbd do
  begin
    //初始化该行
    aibd2[j, 0] := 0;
    //厉遍宽度
    for i := 1 to wbd do
    begin
      //如果传入盘面数组元素不为'0'
      if board[k] <> #48 then
        //对该行与1进行或运算
        aibd2[j, 0] := aibd2[j, 0] or 1;
      //该行左移
      aibd2[j, 0] := aibd2[j, 0] shl 1;
      //获取下一数组元素
      k := k + 1;
    end;
    //该行左移（对齐空行）
    //该行与空行进行或运算（填补墙壁）
    aibd2[j, 0] := (aibd2[j, 0] shl 1) or bdcnull;
  end;
  //设置深度
  aidepthc := 1 + guess;
  for k := 1 to depthm do
    with aiknext[k] do
    begin
      knd := 0;
      sit := 1;
      posx := wbd div 2;
      posy := hbd - 1;
    end;
  //设置当前方块种类
  with aiknext[1] do
  begin
    case curPiece of
      'O':
        knd := 1;
      'I':
        knd := 2;
      'S':
        knd := 3;
      'Z':
        knd := 4;
      'L':
        knd := 5;
      'J':
        knd := 6;
      'T':
        knd := 7;
      else
        knd := 0;
    end;
    //设置当前方块位置及状态
    sit  := curR;
    posx := curX;
    posy := curY;
  end;
  //设置下一块方块种类
  with aiknext[2] do
  begin
    case nextPiece of
      'O':
        knd := 1;
      'I':
        knd := 2;
      'S':
        knd := 3;
      'Z':
        knd := 4;
      'L':
        knd := 5;
      'J':
        knd := 6;
      'T':
        knd := 7;
      else
        knd := 0;
    end;
    //设置深度
    if not (knd = 0) then
      aidepthc := 2 + guess;
    sit  := 1;
    posx := wbd div 2;
    posy := hbd - 1;
  end;
  //AI主函数
  aigo(1);
  //设置返回结果
  bestX^ := airposb;
  bestRotation^ := airsit + 1;
end;

//AI名称DLL入口
function Name(): PChar;
begin
  //返回AI名称
  Name := 'Tetris_ax';
end;

exports AI;

exports name;

end.
