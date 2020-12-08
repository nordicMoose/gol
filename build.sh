#build only works with mingw
c++ gol.cpp -o gameoflife -Iwengine/external/glad -Iwengine/include/core wengine/wengine.lib -lopengl32 -lgdi32 -O3 -static -std=c++17 -mwindows
