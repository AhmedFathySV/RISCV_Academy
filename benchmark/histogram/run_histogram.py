import argparse
import os
import subprocess
from pathlib import Path


def find_gem5_home(project_root):
    gem5_home = os.environ.get("GEM5_HOME") or os.environ.get("XIANG_GEM5")
    if gem5_home:
        return Path(gem5_home).expanduser()

    cache_file = project_root / "build" / "CMakeCache.txt"
    if cache_file.is_file():
        prefix = "GEM5_HOME:PATH="
        for line in cache_file.read_text().splitlines():
            if line.startswith(prefix):
                return Path(line[len(prefix):]).expanduser()

    raise FileNotFoundError(
        "Set GEM5_HOME or XIANG_GEM5, or configure the project with CMake first."
    )


def main():
    project_root = Path(__file__).resolve().parents[2]
    parser = argparse.ArgumentParser(
        description="Run the RISC-V histogram ELF with the Xiangshan gem5 SE config."
    )
    parser.add_argument("--gem5-home", help="XiangShan gem5 checkout")
    parser.add_argument(
        "--binary",
        default=os.environ.get(
            "HISTOGRAM_BINARY", str(project_root / "build" / "histogram-riscv")
        ),
        help="Static RISC-V Linux histogram executable",
    )
    parser.add_argument(
        "--outdir",
        default=str(project_root / "build" / "histogram-xs-m5out"),
        help="gem5 output directory",
    )
    args = parser.parse_args()

    gem5_home = Path(args.gem5_home).expanduser() if args.gem5_home else find_gem5_home(project_root)
    gem5_binary = gem5_home / "build" / "RISCV" / "gem5.opt"
    se_config = gem5_home / "configs" / "example" / "se.py"
    histogram_binary = Path(args.binary).expanduser().resolve()
    output_dir = Path(args.outdir).expanduser().resolve()

    for required_file in (gem5_binary, se_config, histogram_binary):
        if not required_file.is_file():
            parser.error("Required file not found: {}".format(required_file))

    command = [
        str(gem5_binary),
        "-d",
        str(output_dir),
        str(se_config),
        "--cpu-type=DerivO3CPU",
        "--cmd={}".format(histogram_binary),
    ]
    return subprocess.run(command, cwd=project_root).returncode


if __name__ == "__main__":
    raise SystemExit(main())