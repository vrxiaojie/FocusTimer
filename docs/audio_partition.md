# Flash 音频分区使用说明

## 1) 准备 WAV
要求：PCM、16-bit、单声道或双声道、标准44字节头。

推荐参数（使用 [`MP3toWAV.html`](MP3toWAV.html) 工具）：
- 采样率：**22050 Hz**
- 采样位数：**16-bit PCM**
- 声道：**单声道 (Mono)**

3 秒单声道 22050 Hz 16-bit 约 **129 KB**，256K 分区足够。

## 2) 转为分区 bin（本质为校验+原样输出）
```bash
python3 tools/wav_to_partition_bin.py --input assets/timeout.wav --output assets/timeout.bin
```

## 3) 烧录到 `audio` 分区
```bash
idf.py -p /dev/ttyUSB0 partition-table-flash
idf.py -p /dev/ttyUSB0 flash
parttool.py --port /dev/ttyUSB0 write_partition --partition-name audio --input assets/timeout.bin
```

> 若你已完整 `idf.py flash`，前两步可按需执行；`write_partition` 用于单独更新音频。

## 4) 代码调用
倒计时结束时会调用：
- `max98357_play_wav_from_partition(&handle, "audio", 4000)`

表示从 `audio` 分区播放最多4秒提示音。

## 5) 分区大小
[`partitions.csv`](partitions.csv) 中 `audio` 分区为 **256K**
