//方块类型结构
type
  aisq = record
    knd, sit, posx, posy: shortint;
  end;

  //方块最大旋转
const
  aiaksit: array[1..7] of shortint = (0, 1, 1, 1, 3, 3, 3);

  //全局计数变量
var
  aii, aij, aik, ail, aim, ain: longint;
  aisi, aisj, aisk: longint;

  //深度
var
  aidepthc: shortint = 1;
  //盘面数组
var
  aibd2: array[0..maxhbd + 1, 0..depthm] of longword;
  //方块
  aiknext, aik2next: array[1..depthm] of aisq;

var
  airsit, airpos, airposb: shortint;
  //满行空行
var
  bdcfull, bdcnull: longword;

var
  aihcur, aies: array[1..depthm] of longint;
  aimid: longint;
  aitw, aith: longint;
  aihon, aihol, aihod, aihoh: longint;
  aiholln, ailln: longint;
  ailln3, ailln4: boolean;
  aill, aiho: array[0..maxwbd + 1] of shortint;
  aiel: longint;
  aitq, aitq1, aitq2, aitq3, aitq4, aitq5: longword;

  //消行
procedure aierase2(aidepth: shortint);
begin
  //初始化写入行行数
  aij := 1;
  //厉遍读取行行数高度
  for aii := 1 to hbd do
    //如果读取行不为满行
    if not (aibd2[aii, aidepth] = bdcfull) then
    begin
      //写入该行
      aibd2[aij, aidepth] := aibd2[aii, aidepth];
      //写入行行数加一
      aij := aij + 1;
    end;
  //设置消行数
  aies[aidepth] := aii - aij + 1;
end;

//固定方块
procedure ainext2(aidepth: shortint);
begin
  //厉遍高度
  for aisj := 1 to hbd do
    //重置当前深度盘面为前一深度盘面
    aibd2[aisj, aidepth] := aibd2[aisj, aidepth - 1];
  //厉遍方块常量行高度
  for aisj := -2 to 1 do
  begin
    //如果行数没有到底线
    if (aik2next[aidepth].posy + aisj) >= 0 then
      //当前行和方块常量行进行或运算
      aibd2[aik2next[aidepth].posy + aisj, aidepth] :=
        aibd2[aik2next[aidepth].posy + aisj, aidepth] or
        (pce2[aik2next[aidepth].knd, aik2next[aidepth].sit,
        aisj] shl (wbd - aik2next[aidepth].posx + 1));
  end;
  aierase2(aidepth);
end;

//判断重叠
function aistick2(aidepth: shortint): boolean;
begin
  aistick2 := true;
  //厉遍方块常量行高度
  for aisj := -2 to 1 do
    //如果行数没有到底线
    if (aik2next[aidepth].posy + aisj) >= 0 then
      //如果当前行和方块常量行进行与运算为不零
      if (aibd2[aik2next[aidepth].posy + aisj, aidepth - 1] and
        (pce2[aik2next[aidepth].knd, aik2next[aidepth].sit,
        aisj] shl (wbd - aik2next[aidepth].posx + 1))) <> 0 then
        //返回假
        aistick2 := false;
end;

//计算可能重叠块块数
function aistick2k(aidepth: shortint): shortint;
begin
  aistick2k := 0;
  //厉遍七中方块
  for aim := 1 to 7 do
  begin
    //厉遍方块常量行高度
    for aisj := -2 to 1 do
      //如果行数没有到底线
      if (hbd - 1 + aisj) >= 0 then
        //如果当前行和方块常量行进行与运算为不零
        if (aibd2[hbd - 1 + aisj, aidepth] and
          (pce2[aim, 1,
          aisj] shl (wbd - wbd div 2))) <> 0 then
          //返回假
          aistick2k := aistick2k + 1;
  end;
end;

//估值函数
function aieval2(aidepth: shortint): longint;
begin
  //设置当前块行数
  aihcur[aidepth] := aik2next[aidepth].posy;
  //初始化列变换
  aitw := 0;
  //厉遍高度
  for aij := 1 to hbd do
  begin
    //对该行和该行右移一格进行异或运算
    aitq := (aibd2[aij, aidepth] xor (aibd2[aij, aidepth] shr 1)) shr 1;
    //厉遍宽度
    for aii := 0 to wbd do
    begin
      //如果最右为真（左右方块不同）则列变换加一
      aitw := aitw + (aitq and 1);
      //变换行右移
      aitq := aitq shr 1;
    end;
  end;
  //初始化行变换
  aith := 0;
  for aij := 0 to hbd - 1 do
  begin
    //对该行和上一行进行异或运算
    aitq := (aibd2[aij, aidepth] xor aibd2[aij + 1, aidepth]) shr 2;
    //厉遍宽度
    for aii := 1 to wbd do
    begin
      //如果最右为真（上下方块不同）则行变换加一
      aith := aith + (aitq and 1);
      //变换行右移
      aitq := aitq shr 1;
    end;
  end;
  //初始化洞数
  aihon := 0;
  //初始化洞行数
  aihol := 0;
  //初始化最高洞行数
  aihod := 0;
  //初始化最高洞上方块数
  aihoh := 0;
  //初始化洞深
  aiholln := 0;
  //初始化井深
  ailln := 0;
  ailln3 := false;
  ailln4 := false;
  //厉遍宽度
  for aii := wbd downto 1 do
  begin
    //初始化洞列
    aiho[aii] := 0;
    //初始化井列
    aill[aii] := 0;
  end;
  //初始化无洞行
  aitq2 := bdcnull;
  //反向厉遍高度
  for aij := hbd downto 1 do
  begin
    //对先前无洞行和当前行进行或运算
    aitq2 := aitq2 or aibd2[aij, aidepth];
    //对当前无洞行和当前行进行异或运算（结果为洞行）
    aitq1 := (aitq2 xor aibd2[aij, aidepth]) shr 2;
    //当前行有洞
    if not (aitq1 = 0) then
    begin
      //洞行数加一
      aihol := aihol + 1;
      //如果先前尚未有洞行
      if (aihod = 0) then
      begin
        //设置最高有洞行行数
        aihod := aij;
        //保存最高有洞行洞状态
        aitq4 := aitq1;
      end
    end;
    //初始化井行右移
    aitq3 := aitq2 shr 1;
    //反向厉遍宽度
    for aii := wbd downto 1 do
    begin
      //如果洞行最右为洞
      if aitq1 and 1 = 1 then
      begin
        //洞数加一
        aihon := aihon + 1;
        //洞列加一
        aiho[aii] := aiho[aii] + 1;
      end
      //如果洞行最右非洞
      else
      begin
        //洞列清零
        aiho[aii] := 0;
      end;
      //设置洞深加洞列
      aiholln := aiholln + aiho[aii];
      //洞行右移
      aitq1 := aitq1 shr 1;
      //如果井行最右为井
      if ((aitq3 xor 5) and 7) = 0 then
      begin
        //井列加一
        aill[aii] := aill[aii] + 1;
        //设置井深加井列
        if aill[aii] = 3 then ailln3 := true;
        if aill[aii] = 4 then ailln4 := true;
        if aill[aii] > 4 then aill[aii] := aill[aii] - 2;
        ailln := ailln + aill[aii];
      end;
      //井行右移
      aitq3 := aitq3 shr 1;
    end;
  end;
  //如果最高有洞行行数不为零
  if not (aihod = 0) then
    //从最高有洞行上一行开始往上厉遍
    for aij := aihod + 1 to hbd do
    begin
      //设临时行为最高有洞行和当前行的异或变换（把有洞列的方块异或出来）
      aitq5 := aitq4 and (aibd2[aij, aidepth] shr 2);
      //如果有洞列全部没有方块则跳出循环
      if aitq5 = 0 then
        Break;
      //厉遍宽度
      for aii := 1 to wbd do
      begin
        //如果最右为方块则最高有洞行上方块数增加该行行数
        aihoh := aihoh + (aitq5 and 1) * aij;
        //临时行右移
        aitq5 := aitq5 shr 1;
      end;
    end;
  aiel := 0;
  if ln34 then
  begin
    if ailln3 then ailln := ailln - 6;
	if ailln4 then ailln := ailln - 4;
      ailln := 0;
    if aies[1] <= 2 then
      aiel := aies[1]
    else
      aiel := -aies[1];
  end;
  //设置左中右平衡破缺参数
  aimid := -Abs(aik2next[aidepth].posx * 2 - wbd);
  //返回估值结果
  aieval2 := aitw * 80 + aith * 80 + aihon * 60 + aihol * 380 +
    ailln * 100 + aiholln * 40 + aihoh * 5 + aimid * 2 + aiel * 600;
  for ail := 1 to aidepthc do
    aieval2 := aieval2 + (aihcur[ail] * 1750 div (hbd * aidepthc) -
      aies[ail] * 600 div aidepthc);
  aieval2 := aieval2 + aistick2k(aidepth) * 50000;
//with aik2next[aidepth] do writeln(aidepth:5,sit:5,posx:5,aieval2:5);
//writeln(aitw:5,aith:5,aihon:5,aihol:5,ailln:5,aiholln:5,aihoh:5,aimid:5,aiel:5,aihcur[1]:5,aihcur[2]:5,aies[1]:5,aies[2]:5);
//writeln(aitw*80:5,aith*80:5,aihon*60:5,aihol*380:5,ailln*100:5,aiholln*40:5,aihoh*5:5,aimid*2:5,aiel,aihcur[1]*1750 div(hbd*aidepthc):5,aihcur[2]*1750 div(hbd*aidepthc):5,aies[1]*600 div aidepthc:5,aies[2]*600 div aidepthc:5);
{
if aies[2]+aies[1]>0 then
  begin
 writeln(aies[1],#9,aies[2],#9,aidepth);
with aik2next[1] do writeln(#9,ailln,#9,knd,#9,sit,#9,posx,#9);
with aik2next[2] do writeln(#9,ailln,#9,knd,#9,sit,#9,posx,#9);
end;
}
//writeln();
end;

//超前引用
function aigo(aidepth: shortint): longint; forward;

  //AI厉遍函数
function aicalc(aidepth: shortint): longint;
  //初始化结果
var
  airesm: longint = $7FFFFFFF;
  //初始化临时结果
var
  airesc, airese, airesr: longint;
  //初始化方块最大移动
var
  aimsit, aimposl, aimposr: shortint;
var
  aictsit, aictposx: shortint;
begin
  //writeln();
  aimsit := 0;
  //当方块最大旋转小于方块最大旋转常量时做
  while aimsit < aiaksit[aik2next[aidepth].knd] do
  begin
    //增加尝试方块旋转
    aik2next[aidepth].sit := aik2next[aidepth].sit + 1;
    //如果重叠则跳出循环
    if not (aistick2(aidepth)) then
      Break
      //否则增加方块最大旋转
    else
      aimsit := aimsit + 1;
  end;
  for aictsit := aiknext[aidepth].sit to aiknext[aidepth].sit + aimsit do
  begin
    //重置尝试方块旋转
    aik2next[aidepth].sit := aictsit;
    //重置尝试方块横坐标
    aik2next[aidepth].posx := aiknext[aidepth].posx;
    //设方块最大左移为0
    aimposl := 0;
    //做
    repeat
      //方块最大左移减一
      aimposl := aimposl - 1;
      //尝试方块横坐标减一
      aik2next[aidepth].posx := aik2next[aidepth].posx - 1;
      //直到重叠
    until not (aistick2(aidepth));
    aimposl := aimposl + 1;
    //重置尝试方块横坐标
    aik2next[aidepth].posx := aiknext[aidepth].posx;
    //设方块最大右移为0
    aimposr := 0;
    //做
    repeat
      //方块最大右移加一
      aimposr := aimposr + 1;
      //尝试方块横坐标加一
      aik2next[aidepth].posx := aik2next[aidepth].posx + 1;
      //直到重叠
    until not (aistick2(aidepth));
    aimposr := aimposr - 1;
    //从最左到最右厉遍坐标
    for aictposx := aiknext[aidepth].posx + aimposl to aiknext[aidepth].posx +
      aimposr do
    begin
      //重置尝试方块横坐标为厉遍坐标
      aik2next[aidepth].posx := aictposx;
      //做
      repeat
        //尝试方块纵坐标减一
        aik2next[aidepth].posy := aik2next[aidepth].posy - 1;
        //直到重叠
      until not (aistick2(aidepth));
      //尝试方块纵坐标加一
      aik2next[aidepth].posy := aik2next[aidepth].posy + 1;
      //如果仍旧没有重叠（没死）
      if (aistick2(aidepth)) then
      begin
        //固定方块
        //消行
        ainext2(aidepth);
        //获取估值结果
        airesr := aieval2(aidepth);
        //如果当前深度小于深度
        if aidepth < aidepthc then
          //迭代深入
          airesr := aigo(aidepth + 1);
        //如果结果仍旧大于最大结果
        if airesr < airesm then
        begin
          //更新最大结果
          airesm := airesr;
          //如果当前深度是1
          if aidepth = 1 then
          begin
            //更新最优旋转
            airsit := aik2next[aidepth].sit - aiknext[aidepth].sit;
            //更新最优移动
            airpos := aik2next[aidepth].posx - aiknext[aidepth].posx;
            //更新最优横坐标
            airposb := aik2next[aidepth].posx;
          end;
        end;
      end;
      //重置尝试方块纵坐标
      aik2next[aidepth].posy := aiknext[aidepth].posy;
    end;
  end;
  //返回结果
  aicalc := airesm;
end;

//AI主函数
function aigo(aidepth: shortint): longint;
var
  aigoi: longint;
begin
  //初始化结果
  aigo := 0;
  //初始化尝试方块
  aik2next[aidepth] := aiknext[aidepth];
//if aidepth=2 then aik2next[aidepth].knd:=2;
  //如果方块种类为零
  if aik2next[aidepth].knd = 0 then
  begin
    //厉遍7种方块
    for aigoi := 1 to 7 do
    begin
      //重新初始化尝试方块
      aik2next[aidepth].knd := aigoi;
      aik2next[aidepth].sit := 1;
      aik2next[aidepth].posx := wbd div 2 + 1;
      aik2next[aidepth].posy := hbd - 1;
      //叠加结果
      aigo := aigo + aicalc(aidepth);
    end;
    //取结果为叠加结果平均值
    aigo := aigo div 7;
  end
  //如果方块种类不为零
  else
    //直接取结果
    aigo := aicalc(aidepth);
end;

//初始化盘面
procedure ainew2();
begin
  //设置满行
  bdcfull := $FFFFFFFF;
  //设置空行
  bdcnull := ((bdcfull shr (wbd + 2)) shl (wbd + 2)) or %11;
  //厉遍深度
  for aik := 0 to depthm do
    for aij := 0 to hbd + 2 do
      //设置底线为满行
      //设置天花板为满行
      aibd2[aij, aik] := bdcfull;
  for aik := 0 to depthm do
    for aij := 1 to hbd + 1 do
      //设置盘面空间为空行
      aibd2[aij, aik] := bdcnull;
end;
