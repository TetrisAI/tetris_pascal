{$APPTYPE GUI}
program tetris;
//ax_pokl����

uses windows, display, math;

function r2s(r: real): string;
var
  i1, i2: longint;
begin
  i1 := Trunc(r);
  i2 := Trunc((r - i1) * 10);
  r2s := i2s(i1) + '.' + i2s(i2);
end;

  {$I tetris_sound.inc}
  {$I tetris_pce.inc}
  {$I tetris_def.inc}
  {$I tetris_ai.inc}

const
  ck: array[-1..8] of longword = ($000000,
    $1F1F1F, $7F7F7F, $7F7FFF, $7FFF7F,
    $FF7F7F, $7FFFFF, $FFFF7F, $FF7FFF,
    $BFBFBF);
  //����ɫ��7�ַ�����ɫ

var
  sz: shortint;
  //ÿ���С
var
  i, j, k, l, m, n: longint;
  b: boolean;
  si, sj, sk: longint;

var
  ksum, Ln, sum, sln: longint;
  //��������������������������
  lv, aln: real;
  //�ȼ���ƽ������
var
  stickb: boolean;
  //�ƶ��ɹ���ʶ
var
  gamet, stept, lastt: longword;
  //�¾�ʱ�䣬����ʱ�䣬����ʱ�䣬AIʱ��
  downt, leftt, rightt: longword;
  oldt, framet:longword;
  aps: real;
  //����

const
  base = 2;
var
  key: longword;
  //����
var
  aion: shortint = 0;
  //����AI����

var
  pause: boolean = false;
  ksz: boolean = false;
  bmpb: boolean = true;
  highb: shortint = 0;
  winb: boolean = false;

var
  bmp: array[1..8] of pbitmap;

procedure getk();
  //�·���
begin
  ksum := ksum + 1;
  //���������һ
  for i := 1 to depthm - 1 do
    aiknext[i] := aiknext[i + 1];
  //��������ǰ��
  if ksz then
    aiknext[depthm].knd := Random(2) + 3
  else
    aiknext[depthm].knd := Random(7) + 1;
  aiknext[depthm].sit := 1;
  aiknext[depthm].posx := wbd div 2 + 1;
  aiknext[depthm].posy := hbd - 1;
  //��ӷ��鵽��������
  lv := logn(base, Abs(Ln) + base);
  //����ȼ�
  stept := Round(1000 / lv);
  //���µ�������ʱ��
  for sj := 1 to hbd do
  begin
    aibd2[sj, 0] := 0;
    for si := 1 to wbd do
    begin
      if bd[si, sj] > 0 then
        aibd2[sj, 0] := aibd2[sj, 0] or 1;
      aibd2[sj, 0] := aibd2[sj, 0] shl 1;
    end;
    aibd2[sj, 0] := (aibd2[sj, 0] shl 1) or bdcnull;
    //��������AI����
  end;
end;

procedure drawbd(); forward;

procedure New();
  //��ʼ��
begin
  for i := -1 to wbd + 1 do
    for j := -1 to hbd + 1 do
      bd[i, j] := 1;
  for i := 1 to wbd do
    for j := 1 to hbd + 1 do
      bd[i, j] := 0;
  //��ʼ������
  for l := 1 to depthm do
    getk();
  //��ȡ��һ�鷽��
  sum := sum + 1;
  //������һ
  sln := sln + Ln;
  //���������ܺ�
  aln := sln / sum;
  //����ƽ������
  Ln := 0;
  ksum := 0;
  //��������������
  gamet := gettime;
  //�ؼ��¾�ʱ��
  stept := 1000;
  //���赥��ʱ��
  aps := 0;
  lv := 1;
  playsound('start');
  if aion = 0 then
    replaymusic();
end;

procedure Clear();
begin
  sum := 0;
  sln := 0;
  aln := 0;
  New();
end;

function dead(): boolean;
  //�ж�����
begin
  with aiknext[1] do
  begin
    if sit = 5 then
      sit := 1;
    dead := false;
    for si := -2 to 1 do
      for sj := -2 to 1 do
        if pce[knd, sit, - 1 - sj, si] then
          if not (bd[posx + si, posy + sj] = 0) then
            dead := true;
  end;
end;

function Erase(): shortint;
  //����
begin
  j := 1;
  for i := 1 to hbd + 1 do
  begin
    b := true;
    for k := 1 to wbd do
      if bd[k, i] = 0 then
        b := false;
    if not (b) then
    begin
      for k := 1 to wbd do
        bd[k, j] := bd[k, i];
      j := j + 1;
    end;
  end;
  Erase := i - j + 1;
  if aion = 0 then
  begin
    if Erase > 0 then
      playsound('erase');
    if Erase > 2 then
      playsound('erase2');
  end;                        {
  if erase<=2 then erase:=-erase
  else erase:=erase-1;         }
end;

procedure High();
begin
  for i := 1 to wbd do
  begin
    for j := hbd downto 2 do
      bd[i, j] := bd[i, j - 1];
    bd[i, 1] := ((i + highb) mod 2) * 8;
  end;
  highb := 1 - highb;
end;

procedure junk();
begin
  for i := 1 to wbd do
  begin
    for j := hbd downto 2 do
      bd[i, j] := bd[i, j - 1];
    bd[i, 1] := Random(2) * 8;
  end;
end;

procedure Next();
  //��һ��
begin
  with aiknext[1] do
    for si := -2 to 1 do
      for sj := -2 to 1 do
        if pce[knd, sit, - 1 - sj, si] then
          bd[posx + si, posy + sj] := knd;
  //�̶����鵽����
  Ln := Ln + Erase();
  //���в���������
  getk();
  //��ȡ�·���
  aps := 1000 / (((gettime - gamet + 1) / ksum));
  //��������
end;

procedure stick(knd, sit, posx, posy: shortint);
  //�ж��Ƿ������루�ƶ�����ת��
begin
  stickb := true;
  for si := -2 to 1 do
    for sj := -2 to 1 do
      if pce[knd, sit, - 1 - sj, si] then
        if not (bd[posx + si, posy + sj] = 0) then
          stickb := false;
  if stickb = true then
  //������������ƶ�
  begin
    aiknext[1].knd  := knd;
    aiknext[1].sit  := sit;
    aiknext[1].posx := posx;
    aiknext[1].posy := posy;
  end;
end;

procedure Left();
  //����һ��
begin
  with aiknext[1] do
    stick(knd, sit, posx - 1, posy);
  leftt := gettime;
end;

procedure Right();
  //����һ��
begin
  with aiknext[1] do
    stick(knd, sit, posx + 1, posy);
  rightt := gettime;
end;

procedure up();
  //��תһ��
begin
  with aiknext[1] do
    stick(knd, sit mod 4 + 1, posx, posy);
end;

procedure up2();
  //��תһ��
begin
  with aiknext[1] do
    stick(knd, (sit + 2) mod 4 + 1, posx, posy);
end;

procedure down2();
  //����һ��
begin
  with aiknext[1] do
    stick(knd, sit, posx, posy - 1);
  downt := gettime;
  //��������ʱ��
end;

procedure down();
  //����һ�񲢹̶�
begin
  down2();
  if not (stickb) and not (dead) then
    Next();
  //����ƶ�ʧ������һ��
end;

procedure space();
  //���䵽��
begin
  repeat
    down();
  until not (stickb);
end;

procedure ai();
  //AI�ƶ�
begin
  aigo(1);
  //AI����
  while airsit > 0 do
  begin
    up();
    airsit := airsit - 1;
  end;
  //AI��ת
  while airpos < 0 do
  begin
    Left();
    airpos := airpos + 1;
  end;
  //AI����
  while airpos > 0 do
  begin
    Right();
    airpos := airpos - 1;
  end;
  //AI����
  space();
end;

procedure loadbmps();
begin
  bmp[1] := loadbmp('bmps\0.bmp');
  bmp[2] := loadbmp('bmps\1.bmp');
  bmp[3] := loadbmp('bmps\4.bmp');
  bmp[4] := loadbmp('bmps\6.bmp');
  bmp[5] := loadbmp('bmps\2.bmp');
  bmp[6] := loadbmp('bmps\5.bmp');
  bmp[7] := loadbmp('bmps\7.bmp');
  bmp[8] := loadbmp('bmps\8.bmp');
end;

procedure drawk(x, y: shortint; c: shortint);
  //���Ʒ���
begin
  if (c <= 0) then
    bar(sz * (x - 1), sz * (hbd - y), sz - 1, sz - 1, ck[c], ck[c])
  else if bmpb then
    drawbmp(bmp[c], 0, 0, 16, 16, sz * (x - 1), sz * (hbd - y), sz, sz)
  else
    bar(sz * (x - 1), sz * (hbd - y), sz - 1, sz - 1, white, ck[c]);
end;

procedure drawt(s: string; Pos: shortint; c: longword);
begin
  bar(wbd * sz, (Pos - 1) * sz, 6 * sz, sz);
  drawtextxy(s, Round((wbd + 3 - Length(s) / 4) * sz), (Pos - 1) * sz, c, black);
end;

procedure drawbd();
  //��������
begin
  display.Clear();
  for i := 1 to wbd do
    for j := 1 to hbd do
      bddraw[i, j] := bd[i, j];
  with aiknext[1] do
    for i := -2 to 1 do
      for j := -2 to 1 do
        if pce[knd, sit, - 1 - j, i] then
          bddraw[posx + i, posy + j] := knd;
  //�̶������еķ���
  for i := 1 to wbd do
    for j := 1 to hbd do
      drawk(i, j, bddraw[i, j]);
  //�������淽��
  for k := 2 to depthm do
    with aiknext[k] do
    begin
      for i := -2 to 1 do
        for j := -1 to 1 do
          if pce[knd, sit, - 1 - j, i] then
            drawk(wbd + 4 + i, hbd - 2 + j - k * 3, knd)
      else
        drawk(wbd + 4 + i, hbd - 2 + j - k * 3, - 1);
      //����next����
      drawt('next ' + i2s(k - 1), 2 + k * 3, ck[knd]);
      //����next����
    end;
  drawt('Game ' + i2s(sum), 1, ck[sum mod 7 + 1]);
  drawt('Score ' + r2s(aln), 2, ck[sum mod 7 + 1]);
  drawt('Level ' + r2s(lv), 3, ck[Trunc(lv) mod 7 + 1]);
  drawt('APS ' + r2s(aps), 4, ck[Trunc(aps * 10) mod 7 + 1]);
  drawt('Line ' + i2s(Abs(Ln)), 5, ck[Abs(Ln) mod 7 + 1]);
  drawt('Depth ' + i2s(aidepthc), 6, ck[aidepthc]);
  if pause then
    drawt('Paused', 7, ck[5])
  else
    drawt('Paused', 7, black);
  freshwin();
end;

procedure SetSize(size: shortint);
begin
  winb := false;
  sz := size;
  if not(IsWin()) then
    CreateWin((wbd + 6) * sz, hbd * sz, black, black);
  display.SetSize((wbd + 6) * sz, hbd * sz);
  //��������
  winb := true;
  settitle('Tetris ax');
  //���ô���ˢ��Ƶ��
  setfontsize(sz div 2, sz);
  //�������ִ�С
  setfontname('Consolas');
  //��������
  New();
end;

procedure setbdsize(w, h: shortint);
begin
  wbd := w;
  hbd := h;
  ainew2();
  SetSize(sz);
end;

procedure init();
  //�����ʼ��
begin
  Randomize;
  SetSize(25);
  framet:=30;
  oldt:=gettime;
  //��ʼ�������������
  loadbmps();
  ainew2();
  //AI��ʼ��
  drawbd();
  //�״λ�ͼ
end;

begin
  //������
  initsound();
  //��ʼ����Ƶ
  init();
  //��ʼ������
  repeat
  //��ѭ��
  if aion > 0 then
  //�ж�AI�Ƿ��
  begin
    ai();
    if dead then
      New();
    if aion=1 then drawbd();
  end;
  //aion
//  checkmusic();
  if isnextmsg then
    begin
    if iskey() then
    //�ж��Ƿ񰴼�
      begin
      key := getkey();
      //��ȡ����
      case key of
        k_1:
          if aion=1 then aion := 0 else aion:=1;
        k_2:
          if aion=2 then aion := 0 else aion:=2;
        else
          aion := 0;
        end;
      case key of
        k_left:
          Left();
        k_right:
          Right();
        k_down:
          down();
        k_up:
          up();
        96:
          up2();
        k_space:
          space();
        k_enter:
          if dead then
            New()
          else
            ai();
        k_3:
          aidepthc := aidepthc mod depthm + 1;
        k_4:
          soundon := not (soundon);
        k_5:
          musicon := not (musicon);
        k_9:
          bmpb := not (bmpb);
        107:
          SetSize(min(sz + 2,64));
        109:
          SetSize(max(sz - 2,1));
        187:
          SetSize(min(sz + 2,64));
        189:
          SetSize(max(sz - 2,1));
        k_C:
          Clear();
        k_H:
          High();
        k_J:
          junk();
        k_P:
          pause := not (pause);
        k_R:
          New();
        k_W:
          setbdsize(wbd, max(hbd - 1,4));
        k_A:
          setbdsize(max(wbd - 1,4), hbd);
        k_S:
          setbdsize(wbd, min(hbd + 1,64));
        k_D:
          setbdsize(min(wbd + 1,64), hbd);
        k_Z:
          ksz := not (ksz);
        k_esc:
          Halt();
        else
          if dead then
            New();
        end;
      //���ݰ���ִ�в���
      case key of
        k_left:;
        k_right:;
        k_up:
          playsound('up');
        k_down:;
        k_space:
          playsound('space');
        k_5:
          if musicon then
            replaymusic()
          else
            stopaudio(musiccur);
          else
            playsound('click');
        end;//case
      //���ݰ�����������
      end;//key
    if dead then
      playsound('lost');
    end;//msg
  if (gettime > downt + stept) and not (pause) then
  //�жϵ�ǰʱ���Ƿ��ѳ�����������ʱ��
    down();
  //ȡ��һ����Ϣ
  if aion<2 then sleep(1);
//  if aion=2 then framet:=stept else framet:=15;
  if (gettime > oldt + framet) then
    begin
    while gettime > oldt + framet do oldt:=oldt + framet;
    drawbd();
    end;
  //�ػ�����
  until iskey(k_esc) or not(iswin());
end.
