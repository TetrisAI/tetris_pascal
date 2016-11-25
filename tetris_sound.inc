var
  musiccur, musicpos, musiclen: longword;
var
  musicon: boolean = false;
  soundon: boolean = true;

procedure replaymusic();
begin
  if musicon then
  begin
    releaseaudio(musiccur);
    musiccur := loadaudio('wav\music' + i2s(Random(2) + 1) + '.mid');
    playaudio(musiccur);
  end;
end;

procedure checkmusic();
begin
  musicpos := getaudiopos(musiccur);
  musiclen := getaudiolen(musiccur);
  if musicpos = musiclen then
    setaudiopos(musiccur, 0);
end;

procedure initsound();
begin
  loadaudio('wav\up.wav');
  loadaudio('wav\space.wav');
  loadaudio('wav\click.wav');
  loadaudio('wav\start.wav');
  loadaudio('wav\lost.wav');
  loadaudio('wav\erase.wav');
  loadaudio('wav\erase2.wav');
end;

procedure playsound(s: string);
begin
  if soundon then
    case s of
      'up':
        SetAudioPos(1, 0);
      'space':
        SetAudioPos(2, 0);
      'click':
        SetAudioPos(3, 0);
      'start':
        SetAudioPos(4, 0);
      'lost':
        SetAudioPos(5, 0);
      'erase':
        SetAudioPos(6, 0);
      'erase2':
        SetAudioPos(7, 0);
    end;
end;
