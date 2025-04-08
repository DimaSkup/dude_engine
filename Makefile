# g++ -w -std=c++14 ./src/*cpp  -- compile all the sources in src folder 
# -o game                       -- make an object file named "game"   
# -I"./lib/lua"                 -- include files from folder
# -L"./lib/lua"                 -- link files from folder
# -llua5.3                      -- use ext lib lua5.3
# -lSDL2                        -- use ext lib SDL2
# -lSDL2_image                  -- use ext lib to work with images
# -lSDL2_ttf                    -- use ext lib to work with fonts
# -lSDL2_mixer                  -- use ext lib to work with sounds
build:
	g++ -w -std=c++14 \
	./src/*.cpp \
	-o game \
	-I"./lib/lua" \
	-L"./lib/lua" \
	-llua5.3 \
	-lSDL2 \
	-lSDL2_image \
	-lSDL2_ttf \
	-lSDL2_mixer;

clean:
	rm ./game;

run:
	./game
