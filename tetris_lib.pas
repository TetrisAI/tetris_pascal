library tetris_lib;

//uses display;

{$I tetris_def.inc}

{$I tetris_pce.inc}

{$I tetris_ai.inc}

var
  i, j, k: longint;
const
  guess = 0;

  //AI��DLL���
function AI(boardW, boardH: longint; board: PChar; curPiece: char;
  curX, curY, curR: longint; nextPiece: char;
  bestX, bestRotation: pinteger): longint; stdcall;
begin
  //���ÿ��
  wbd := boardW;
  //���ø߶�
  hbd := boardH;
  //��ʼ�����漰����
  ainew2();
  //��������
  k := 0;
  //����߶�
  for j := 1 to hbd do
  begin
    //��ʼ������
    aibd2[j, 0] := 0;
    //������
    for i := 1 to wbd do
    begin
      //���������������Ԫ�ز�Ϊ'0'
      if board[k] <> #48 then
        //�Ը�����1���л�����
        aibd2[j, 0] := aibd2[j, 0] or 1;
      //��������
      aibd2[j, 0] := aibd2[j, 0] shl 1;
      //��ȡ��һ����Ԫ��
      k := k + 1;
    end;
    //�������ƣ�������У�
    //��������н��л����㣨�ǽ�ڣ�
    aibd2[j, 0] := (aibd2[j, 0] shl 1) or bdcnull;
  end;
  //�������
  aidepthc := 1 + guess;
  for k := 1 to depthm do
    with aiknext[k] do
    begin
      knd := 0;
      sit := 1;
      posx := wbd div 2;
      posy := hbd - 1;
    end;
  //���õ�ǰ��������
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
    //���õ�ǰ����λ�ü�״̬
    sit  := curR;
    posx := curX;
    posy := curY;
  end;
  //������һ�鷽������
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
    //�������
    if not (knd = 0) then
      aidepthc := 2 + guess;
    sit  := 1;
    posx := wbd div 2+1;
    posy := hbd - 1;
  end;
  //AI������
  aigo(1);
  //���÷��ؽ��
  bestX^ := airposb;
  bestRotation^ := airsit + 1;
end;

//AI����DLL���
function Name(): PChar;
begin
  //����AI����
  Name := 'Tetris_ax';
end;

exports AI;

exports Name;

end.
