#!/usr/bin/env python3

import argparse
import struct
from pathlib import Path

from PIL import Image


def write_rgb565(source: Path, destination: Path, with_alpha: bool) -> None:
    image = Image.open(source).convert("RGBA")
    color = bytearray()
    alpha = bytearray()
    for red, green, blue, opacity in image.get_flattened_data():
        rgb565 = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3)
        color.extend(struct.pack("<H", rgb565))
        if with_alpha:
            alpha.append(opacity)
    destination.write_bytes(color + alpha)


def main() -> None:
    parser = argparse.ArgumentParser(description="Prepare XiaoZhi alarm UI assets for LVGL")
    parser.add_argument("--source", required=True, type=Path)
    parser.add_argument("--output", required=True, type=Path)
    args = parser.parse_args()

    source = args.source.resolve()
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=True)

    write_rgb565(
        source / "assets" / "shell" / "alarm_screen_shell_284x240.png",
        output / "alarm_shell.r565",
        False,
    )
    write_rgb565(
        source / "assets" / "controls" / "toggle_on_leaf.png",
        output / "alarm_on.r5a8",
        True,
    )
    write_rgb565(
        source / "assets" / "controls" / "toggle_off_clover.png",
        output / "alarm_off.r5a8",
        True,
    )

    print(f"Prepared alarm UI assets in {output}")


if __name__ == "__main__":
    main()
