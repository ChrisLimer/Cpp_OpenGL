#!/usr/bin/env python3
"""
Test the C++ OpenGL + pybind module.

Usage (from repo root after building):
  uv run python test_scatter.py --n 5000 --eye 8 6 8 --center 0 0 0 --up 0 1 0
  # or load from file:
  uv run python test_scatter.py --points points.npy --eye 8 6 8 --center 0 0 0 --up 0 1 0

If the module isn't found, use --module-dir to point at your build:
  uv run python test_scatter.py --module-dir build/python
"""
from __future__ import annotations
import argparse
import sys
from pathlib import Path
import numpy as np

def add_module_path(module_dir: Path) -> None:
    if str(module_dir) not in sys.path:
        sys.path.insert(0, str(module_dir))

def try_import_pycpp(module_dir: Path | None):
    try:
        import pycpp  # type: ignore
        return pycpp
    except Exception:
        if module_dir:
            add_module_path(module_dir)
        import importlib
        try:
            return importlib.import_module("pycpp")
        except Exception as e:
            print("Could not import 'pycpp'.", file=sys.stderr)
            print("Hints:", file=sys.stderr)
            print("  • Build first: mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build .", file=sys.stderr)
            print("  • Then run from repo root or pass --module-dir build/python", file=sys.stderr)
            raise e

def parse_args():
    p = argparse.ArgumentParser(description="Render a 3D scatter plot via pycpp (GLFW + glad).")
    src = p.add_mutually_exclusive_group()
    src.add_argument("--n", type=int, default=2000, help="Generate N random points (default: 2000).")
    src.add_argument("--points", type=Path, help="Path to .npy or .csv with shape (N,3) float32/float.")

    p.add_argument("--spread", type=float, default=10.0, help="Range for generated points (uniform in [-spread/2, spread/2]).")

    p.add_argument("--eye", nargs=3, type=float, default=[8.0, 6.0, 8.0], help="Camera eye position x y z.")
    p.add_argument("--center", nargs=3, type=float, default=[0.0, 0.0, 0.0], help="Camera look-at center x y z.")
    p.add_argument("--up", nargs=3, type=float, default=[0.0, 1.0, 0.0], help="Camera up vector x y z.")

    p.add_argument("--width", type=int, default=1280, help="Window width.")
    p.add_argument("--height", type=int, default=720, help="Window height.")
    p.add_argument("--point_size", type=float, default=4.0, help="OpenGL point size.")
    p.add_argument("--color", nargs=3, type=float, default=[0.9, 0.9, 1.0], help="Point color RGB (0..1).")
    p.add_argument("--bgcolor", nargs=3, type=float, default=[0.06, 0.06, 0.09], help="Background color RGB (0..1).")

    # Try to guess build/python relative to this script if not provided
    default_build_python = (Path(__file__).resolve().parent / "build" / "python")
    p.add_argument("--module-dir", type=Path, default=default_build_python,
                   help=f"Directory containing pycpp.*.so (default: {default_build_python})")

    return p.parse_args()

def load_points(path: Path) -> np.ndarray:
    if path.suffix.lower() == ".npy":
        pts = np.load(path)
    elif path.suffix.lower() == ".csv":
        pts = np.loadtxt(path, delimiter=",")
    else:
        raise SystemExit(f"Unsupported file type: {path.suffix} (use .npy or .csv)")

    pts = np.asarray(pts, dtype=np.float32)
    if pts.ndim != 2 or pts.shape[1] != 3:
        raise SystemExit(f"Expected shape (N,3), got {pts.shape}")
    return pts

def main():
    args = parse_args()

    # Import pycpp (add module path if needed)
    module_dir = args.module_dir
    pycpp = try_import_pycpp(module_dir if module_dir and module_dir.exists() else None)

    # Prepare points
    if args.points:
        print(f"load points...")
        pts = load_points(args.points)
    else:
        # Generate uniform random points in [-spread/2, spread/2]
        print(f"generate points...")
        half = args.spread / 2.0
        pts = (np.random.rand(args.n, 3).astype(np.float32) * args.spread) - half
        print(f"    points: {pts.shape}")
        print(f"    points: {pts}")

    # Build camera
    eye = pycpp.Vec3(*map(float, args.eye))
    center = pycpp.Vec3(*map(float, args.center))
    up = pycpp.Vec3(*map(float, args.up))

    # Ensure dtype/shape
    if pts.dtype != np.float32:
        pts = pts.astype(np.float32, copy=False)
    if pts.ndim != 2 or pts.shape[1] != 3:
        raise SystemExit(f"points must be (N,3), got {pts.shape}")

    # Render (blocks until window closed or ESC pressed)
    pycpp.render_scatter(
        pts,
        eye, center, up,
        width=args.width, height=args.height,
        point_size=float(args.point_size),
        color=tuple(map(float, args.color)),
        bgcolor=tuple(map(float, args.bgcolor)),
    )

if __name__ == "__main__":
    main()
