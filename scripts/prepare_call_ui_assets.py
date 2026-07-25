#!/usr/bin/env python3

import argparse
import shutil
import struct
from pathlib import Path

from PIL import Image


STATE_NAMES = (
    "idle",
    "connecting",
    "listening",
    "thinking",
    "speaking",
    "ended",
)

EFFECT_GROUPS = {
    "connecting_spinner": "c",
    "input_wave": "i",
    "thinking_orbit": "t",
    "output_wave": "o",
}

EMOTIONS = {
    "sparkle": "spark",
    "heart": "heart",
    "question": "question",
    "exclamation": "exclaim",
    "tear": "tear",
    "angry": "angry",
    "zzz": "zzz",
}


def write_rgb565a8(source: Path, destination: Path) -> None:
    image = Image.open(source).convert("RGBA")
    color = bytearray()
    alpha = bytearray()
    for red, green, blue, opacity in image.get_flattened_data():
        rgb565 = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3)
        color.extend(struct.pack("<H", rgb565))
        alpha.append(opacity)
    destination.write_bytes(color + alpha)


def main() -> None:
    parser = argparse.ArgumentParser(description="Prepare XiaoZhi call UI assets for LVGL")
    parser.add_argument("--source", required=True, type=Path)
    parser.add_argument("--output", required=True, type=Path)
    args = parser.parse_args()

    source = args.source.resolve()
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=True)
    for existing in output.glob("call_*"):
        if existing.is_file():
            existing.unlink()

    for state in STATE_NAMES:
        src = source / "runtime_assets" / "rgb565_le" / f"call_{state}_284x240.rgb565le"
        shutil.copyfile(src, output / f"call_{state}.r565")

    write_rgb565a8(
        source / "runtime_assets" / "overlays" / "caption_panel_blank_149x30.png",
        output / "call_caption.r5a8",
    )

    for emotion, short_name in EMOTIONS.items():
        write_rgb565a8(
            source / "runtime_assets" / "emotion_overlays" / f"emotion_{emotion}_36.png",
            output / f"call_em_{short_name}.r5a8",
        )

    for source_prefix, output_prefix in EFFECT_GROUPS.items():
        for frame in range(1, 9):
            matches = list((source / "runtime_assets" / "effects").glob(
                f"{source_prefix}_{frame:02d}_*.png"
            ))
            if len(matches) != 1:
                raise RuntimeError(f"Expected one frame for {source_prefix} {frame:02d}")
            write_rgb565a8(
                matches[0], output / f"call_fx_{output_prefix}_{frame:02d}.r5a8"
            )

    print(f"Prepared {len(list(output.iterdir()))} call UI assets in {output}")


if __name__ == "__main__":
    main()
