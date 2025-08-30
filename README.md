# Cpp_OpenGL

A C++ + OpenGL project using **glfw** and **glad** as third-party libraries, managed as git submodules.

---

## Clone the project

This repository uses git submodules. To clone including third-party code:

```bash
git clone --recurse-submodules git@github.com:ChrisLimer/Cpp_OpenGL.git
cd Cpp_OpenGL
```

If you already cloned without `--recurse-submodules`, run:
```bash
git submodule update --init --recursive
```
---

## Updating submodules
To pull the latest changes for all submodules:
```bash
git submodule update --remote --merge
```

Or update a single submodule (example: glfw):
```bash
cd Cpp_library/third_party/glfw
git fetch origin
git checkout master   # or 'main'
git pull
cd ../../../..
git add Cpp_library/third_party/glfw
git commit -m "Update glfw submodule"
git push
```