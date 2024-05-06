noflags =
flags = -g -fsanitize=leak
errflags = -Wall -Wextra
linkedLibraries = -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lmingw32
dependencies = Makefile main.cpp func.h user.h projectile.h enemy.h
shareLinkedLibraries = SDL2_image.dll SDL2_ttf.dll SDL2.dll

main: $(dependencies)
	g++ $(errflags) main.cpp -o run -IC:\Users\david\Downloads\Code\SDL2\include -LC:\Users\david\Downloads\Code\SDL2\lib $(linkedLibraries)
	./run.exe
	python key.py

noKey: $(dependencies)
	g++ $(errflags) main.cpp -o run -IC:\Users\david\Downloads\Code\SDL2\include -LC:\Users\david\Downloads\Code\SDL2\lib $(linkedLibraries)
	./run.exe

noRun: $(dependencies)
	g++ $(errflags) main.cpp -o run -IC:\Users\david\Downloads\Code\SDL2\include -LC:\Users\david\Downloads\Code\SDL2\lib $(linkedLibraries)

noKeySan: $(dependencies)
	g++ $(errflags) $(flags) main.cpp -o run -IC:\Users\david\Downloads\Code\SDL2\include -LC:\Users\david\Downloads\Code\SDL2\lib $(linkedLibraries)
	./run.exe

# boxRun: $(dependencies)
# 	g++ $(errflags) -static main.cpp -o boxRun $(shareLinkedLibraries)
# 	powershell Remove-Item C:\Users\david\Desktop\boxRunShare\boxRun.exe
# 	powershell move boxRun.exe C:\Users\david\Desktop\boxRunShare
# 	powershell Remove-Item C:\Users\david\Desktop\boxRun.zip
# 	powershell Compress-Archive C:\Users\david\Desktop\boxRunShare C:\Users\david\Desktop\boxRun.zip
# 	powershell make
# 	powershell clear
# 	powershell clear