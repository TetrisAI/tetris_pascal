{$APPTYPE GUI}
program tetris;
//ax_pokl制作

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
  //背景色，7种方块颜色

var
  sz: shortint;
  //每格大小
var
  i, j, k, l, m, n: longint;
  b: boolean;
  si, sj, sk: longint;

var
  ksum, Ln, sum, sln: longint;
  //方块数，行数，局数，总行数
  lv, aln: real;
  //等级，平均行数
var
  stickb: boolean;
  //移动成功标识
var
  gamet, stept, lastt: longword;
  //新局时间，单步时间，下落时间，AI时间
  downt, leftt, rightt: longword;
  aps: real;
  //手速

const
  base = 2;
var
  key: longword;
  //按键
var
  aion: shortint = 0;
  //快速AI开关

var
  pause: boolean = false;
  ksz: boolean = false;
  bmpb: boolean = false;
  highb: shortint = 0;
  winb: boolean = false;

var
  bmp: array[1..8] of pbitmap;

procedure getk();
  //新方块
begin
  ksum := ksum + 1;
  //方块计数加一
  for i := 1 to depthm - 1 do
    aiknext[i] := aiknext[i + 1];
  //方块序列前移
  if ksz then
    aiknext[depthm].knd := Random(2) + 3
  else
    aiknext[depthm].knd := Random(7) + 1;
  aiknext[depthm].sit := 1;
  aiknext[depthm].posx := wbd div 2 + 1;
  aiknext[depthm].posy := hbd - 1;
  //添加方块到方块序列
  lv := logn(base, Ln + base);
  //计算等级
  stept := Round(1000 / lv);
  //更新单步下落时间
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
    //重新设置AI盘面
  end;
end;

procedure drawbd(); forward;

procedure New();
  //初始化
begin
  for i := -1 to wbd + 1 do
    for j := -1 to hbd + 1 do
      bd[i, j] := 1;
  for i := 1 to wbd do
    for j := 1 to hbd + 1 do
      bd[i, j] := 0;
  //初始化棋盘
  for l := 1 to depthm do
    getk();
  //获取第一块方块
  sum := sum + 1;
  //局数加一
  sln := sln + Ln;
  //更新行数总和
  aln := sln / sum;
  //计算平均行数
  Ln := 0;
  ksum := 0;
  //行数，块数清零
  gamet := gettime;
  //重计新局时间
  stept := 1000;
  //重设单步时间
  aps := 0;
  lv := 1;
  drawbd();
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
  //判定死亡
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
  //消行
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
  end;
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
  //下一步
begin
  with aiknext[1] do
    for si := -2 to 1 do
      for sj := -2 to 1 do
        if pce[knd, sit, - 1 - sj, si] then
          bd[posx + si, posy + sj] := knd;
  //固定方块到盘面
  Ln := Ln + Erase();
  //消行并更新行数
  getk();
  //获取新方块
  aps := 1000 / (((gettime - gamet + 1) / ksum));
  //计算手速
end;

procedure stick(knd, sit, posx, posy: shortint);
  //判断是否能塞入（移动或旋转）
begin
  if sit = 5 then
    sit := 1;
  stickb := true;
  for si := -2 to 1 do
    for sj := -2 to 1 do
      if pce[knd, sit, - 1 - sj, si] then
        if not (bd[posx + si, posy + sj] = 0) then
          stickb := false;
  if stickb = true then
  //如果能塞入则移动
  begin
    aiknext[1].knd := knd;
    aiknext[1].sit := sit;
    aiknext[1].posx := posx;
    aiknext[1].posy := posy;
  end;
end;

procedure Left();
  //左移一格
begin
  with aiknext[1] do
    stick(knd, sit, posx - 1, posy);
  leftt := gettime;
end;

procedure Right();
  //右移一格
begin
  with aiknext[1] do
    stick(knd, sit, posx + 1, posy);
  rightt := gettime;
end;

procedure up();
  //旋转一下
begin
  with aiknext[1] do
    stick(knd, sit + 1, posx, posy);
end;

procedure down2();
  //下落一格
begin
  with aiknext[1] do
    stick(knd, sit, posx, posy - 1);
  downt := gettime;
  //更新下落时间
end;

procedure down();
  //下落一格并固定
begin
  down2();
  if not (stickb) and not (dead) then
    Next();
  //如果移动失败则下一步
end;

procedure space();
  //下落到底
begin
  repeat
    down();
  until not (stickb);
end;

procedure ai();
  //AI移动
begin
  aigo(1);
  //AI计算
  while airsit > 0 do
  begin
    up();
    airsit := airsit - 1;
  end;
  //AI旋转
  while airpos < 0 do
  begin
    Left();
    airpos := airpos + 1;
  end;
  //AI左移
  while airpos > 0 do
  begin
    Right();
    airpos := airpos - 1;
  end;
  //AI右移
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
  //绘制方块
begin
  if (c <= 0) then
    bar(sz * (x - 1), sz * (hbd - y), sz - 1, sz - 1, ck[c], ck[c])
  else if bmpb then
    drawbmp(bmp[c], 0, 0, 16, 16, sz * (x - 1), sz * (hbd - y), sz, sz)
  else
    bar(sz * (x - 1), sz * (hbd - y), sz - 1, sz - 1, ck[c], white);
end;

procedure drawt(s: string; Pos: shortint; c: longword);
begin
  bar(wbd * sz, (Pos - 1) * sz, 6 * sz, sz);
  drawtextxy(s, Round((wbd + 3 - Length(s) / 4) * sz), (Pos - 1) * sz, c, black);
end;

procedure drawbd();
  //绘制盘面
begin
  for i := 1 to wbd do
    for j := 1 to hbd do
      bddraw[i, j] := bd[i, j];
  with aiknext[1] do
    for i := -2 to 1 do
      for j := -2 to 1 do
        if pce[knd, sit, - 1 - j, i] then
          bddraw[posx + i, posy + j] := knd;
  //固定下落中的方块
  for i := 1 to wbd do
    for j := 1 to hbd do
      drawk(i, j, bddraw[i, j]);
  //绘制盘面方块
  for k := 2 to depthm do
    with aiknext[k] do
    begin
      for i := -2 to 1 do
        for j := -1 to 1 do
          if pce[knd, sit, - 1 - j, i] then
            drawk(wbd + 4 + i, hbd - 2 + j - k * 3, knd)
      else
        drawk(wbd + 4 + i, hbd - 2 + j - k * 3, - 1);
      //绘制next区域
      drawt('next ' + i2s(k - 1), 2 + k * 3, ck[knd]);
      //绘制next文字
    end;
  drawt('Game ' + i2s(sum), 1, ck[sum mod 7 + 1]);
  drawt('Score ' + r2s(aln), 2, ck[sum mod 7 + 1]);
  drawt('Level ' + r2s(lv), 3, ck[Trunc(lv) mod 7 + 1]);
  drawt('APS ' + r2s(aps), 4, ck[Trunc(aps * 10) mod 7 + 1]);
  drawt('Line ' + i2s(Ln), 5, ck[Ln mod 7 + 1]);
  drawt('Depth ' + i2s(aidepthc), 6, ck[aidepthc]);
  if pause then
    drawt('Paused', 7, ck[5])
  else
    drawt('Paused', 7, black);
end;

procedure SetSize(size: shortint);
begin
  if winb then
    CloseWin();
  winb := false;
  sz := size;
  CreateWin((wbd + 6) * sz, hbd * sz, black, black);
  //建立窗口
  winb := true;
  settitle('Tetris ax');
  //设置标题文字
  setfresh(1);
  //设置窗口刷新频率
  setfontsize(sz div 2, sz);
  //设置文字大小
  setfontname('Consolas');
  //设置字体
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
  //程序初始化
begin
  Randomize;
  //初始化随机数发生器
  loadbmps();
  SetSize(25);
  ainew2();
  //AI初始化
  drawbd();
  //首次绘图
end;

begin
  //主程序
  initsound();
  //初始化音频
  init();
  //初始化程序
  repeat
    //主循环
    while aion > 0 do
    //判断AI是否打开
    begin
      ai();
      if dead then
        New();
      if (aion = 1) or (gettime > lastt + stept) then
      //判断是否需要绘图
      begin
        lastt := gettime;
        drawbd();
        nextmsg();
        if ismsg(15) then
        begin
          bar(0, 0, _w, _h, black);
          drawbd();
        end;
        if iskey() then
        //判断是否按键使AI关闭
        begin
          key := getkey();
          case key of
            k_1:
              aion := aion - 1;
            k_2:
              aion := -aion * 2 + 4;
            k_3:
              aidepthc := aidepthc mod depthm + 1;
            else
              aion := 0;
          end;
          //case
        end;
        //iskey
      end;
      //draw
    end;
    //aion
    if dead then
    begin
      playsound('lost');
      drawt('Press "R"', 7, ck[5]);
      waitkey();
    end;
    checkmusic();
    nextmsg();
    if iskey() then
    //判断是否按键
    begin
      key := getkey();
      //获取按键
      if key = k_down then
      //判断是否为下
      begin
        down();
        repeat
          if (gettime > downt + 20) then
          begin
            down2();
            drawbd();
          end;
          nextmsg();
        until ismsg(257) or (iskey and not (iskey(k_down)));
      end;
      if iskey() then
        key := getkey();
      //再次获取按键
      case key of
        k_left:
          Left();
        k_right:
          Right();
        k_up:
          up();
        k_space:
          space();
        k_return:
          if dead then
            New()
          else
            ai();
          k_1:
          aion := 1;
        k_2:
          aion := 2;
        k_3:
          aidepthc := aidepthc mod depthm + 1;
        k_4:
          soundon := not (soundon);
        k_5:
          musicon := not (musicon);
        k_9:
          bmpb := not (bmpb);
        107:
          SetSize(sz + 2);
        109:
          SetSize(sz - 2);
        187:
          SetSize(sz + 2);
        189:
          SetSize(sz - 2);
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
          setbdsize(wbd, hbd + 1);
        k_A:
          setbdsize(wbd - 1, hbd);
        k_S:
          setbdsize(wbd, hbd - 1);
        k_D:
          setbdsize(wbd + 1, hbd);
        k_Z:
          ksz := not (ksz);
        k_escape:
          Halt();

        //    227:drawtextxy('IME detected',0,0,red,yellow);
      end;
      //根据按键执行操作
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
      end;
      //根据按键播放音频
      drawbd();
      //重绘盘面
    end;
    if (gettime > downt + stept) and not (pause) then
    //判断当前时间是否已超过单步下落时间
    begin
      down();
      //下落
      drawbd();
      //重绘盘面
    end;
    //取下一条消息
    if ismsg(15) then
    begin
      bar(0, 0, _w, _h, black);
      drawbd();
    end;
  until false;
end.
