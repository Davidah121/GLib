@echo OFF
ninja -f ./build/Debug/build.ninja -v
clang++ -g %WLIBPATH64% %WLIBVALUES% -L C:/Libraries/glew-2.1.0/lib/Release/x64 -l glew32.lib -l opengl32.lib -fsanitize=address ./bin/Debug/obj/*.obj -o ./bin/Debug/GLib.exe