#include "Log.h"
#include "Constants.h"
#include "Game.h"
#include "Render.h"
#include "EntityMgr.h"

int main(int argc, char* args[])
{
    InitLogger();

    Render render;
    Game game;

    constexpr bool isFullScreen = true;
    
    render.Initialize(WINDOW_WIDTH, WINDOW_HEIGHT, isFullScreen);
    game.Initialize();

    LogMsg("Game is running...");

    while (game.IsRunning())
    {
        game.ProcessInput();
        game.Update();

        render.Begin();
        game.Render();
        render.End();
    }


    game.Destroy();
    render.Shutdown();
    CloseLogger();

    return 0;
}
