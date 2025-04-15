#include "Log.h"
#include "Constants.h"
#include "Game.h"
#include "Render.h"
#include "EntityMgr.h"
#include <iostream>

int main(int argc, char* args[])
{
    InitLogger();

    Render render;
    Game game;
    
    render.Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);
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

    g_EntityMgr.ListAllEntities();

    game.Destroy();
    render.Shutdown();
    CloseLogger();

    return 0;
}
