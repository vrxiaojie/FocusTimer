#!/usr/bin/env python3
"""
将 WAV 文件原样复制为可烧录 bin，并校验基础 WAV 头。
用法:
  python3 tools/wav_to_partition_bin.py --input assets/timeout.wav --output assets/timeout.bin
"""

import argparse
import pathlib
import struct
import sys


def validate_wav_header(data: bytes) -> None:
    if len(data) < 44:
        raise ValueError("WAV 文件过小，少于44字节")

    if data[0:4] != b"RIFF" or data[8:12] != b"WAVE":
        raise ValueError("不是合法 RIFF/WAVE 文件")

    if data[12:16] != b"fmt ":
        raise ValueError("当前工具仅支持标准44字节头(12字节后即fmt chunk)")

    audio_format = struct.unpack_from("<H", data, 20)[0]
    channels = struct.unpack_from("<H", data, 22)[0]
    sample_rate = struct.unpack_from("<I", data, 24)[0]
    bits_per_sample = struct.unpack_from("<H", data, 34)[0]
    data_tag = data[36:40]

    if audio_format != 1:
        raise ValueError(f"仅支持PCM编码，当前audio_format={audio_format}")
    if channels not in (1, 2):
        raise ValueError(f"仅支持1或2声道，当前channels={channels}")
    if bits_per_sample != 16:
        raise ValueError(f"仅支持16-bit，当前bits_per_sample={bits_per_sample}")
    if data_tag != b"data":
        raise ValueError("当前工具仅支持标准44字节头(36字节处为data chunk)")

    print(f"WAV校验通过: channels={channels}, sample_rate={sample_rate}, bits={bits_per_sample}")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", required=True, help="输入 wav 文件")
    parser.add_argument("--output", required=True, help="输出 bin 文件")
    args = parser.parse_args()

    in_path = pathlib.Path(args.input)
    out_path = pathlib.Path(args.output)

    raw = in_path.read_bytes()
    validate_wav_header(raw)

    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_bytes(raw)

    print(f"输出成功: {out_path} ({len(raw)} bytes)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
