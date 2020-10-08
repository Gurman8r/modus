#!/bin/bash
cd ../ext
git submodule sync

git submodule add --force https://github.com/Gurman8r/assimp.git
git submodule add --force https://github.com/python/cpython.git
git submodule add --force https://github.com/aseprite/freetype2.git
git submodule add --force https://github.com/kthohr/gcem.git
git submodule add --force https://github.com/nigels-com/glew.git
git submodule add --force https://github.com/glfw/glfw.git
git submodule add --force https://github.com/g-truc/glm.git
git submodule add --force -b docking https://github.com/Gurman8r/imgui.git
git submodule add --force https://github.com/ocornut/imgui_club.git
git submodule add --force https://github.com/Gurman8r/ImGuiColorTextEdit.git
git submodule add --force https://github.com/Gurman8r/imgui-node-editor.git
git submodule add --force https://github.com/CedricGuillemet/ImGuizmo.git
git submodule add --force https://github.com/epezent/implot
git submodule add --force https://github.com/nlohmann/json
git submodule add --force https://github.com/nothings/stb.git
git submodule add --force https://github.com/pybind/pybind11.git
git submodule add --force https://github.com/facebookarchive/RakNet.git
git submodule add --force https://github.com/madler/zlib

git submodule update --init --recursive
git pull

exit $?