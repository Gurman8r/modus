@echo off
cd ..\vendor\source
git submodule sync

git submodule add --force https://github.com/skypjack/entt.git
git submodule add --force https://github.com/aseprite/freetype2.git
git submodule add --force https://github.com/kthohr/gcem.git
git submodule add --force https://github.com/nigels-com/glew.git
git submodule add --force https://github.com/glfw/glfw.git
git submodule add --force https://github.com/g-truc/glm.git
git submodule add --force https://github.com/nlohmann/json
git submodule add --force https://github.com/nothings/stb.git
git submodule add --force https://github.com/pybind/pybind11.git
git submodule add --force https://github.com/madler/zlib

git submodule add --force -b docking https://github.com/ocornut/imgui.git
rem git submodule add --force https://github.com/ocornut/imgui_club.git
rem git submodule add --force https://github.com/BalazsJako/ImGuiColorTextEdit.git
rem git submodule add --force https://github.com/thedmd/imgui-node-editor.git
rem git submodule add --force https://github.com/CedricGuillemet/ImGuizmo.git

rem git submodule add --force https://github.com/Gurman8r/assimp.git
rem git submodule add --force https://github.com/python/cpython.git
rem git submodule add --force https://github.com/facebookarchive/RakNet.git

git submodule update --init --recursive
git pull
if %ERRORLEVEL% NEQ 0 ( pause )