#include "Map.h"
#include "Game.h"
#include "EntityMgr.h"
#include "FileSystem.h"
#include "StrHelper.h"
#include "Entity.h"
#include "Components/TileComponent.h"


Map::Map(
    const char* textureID,
    const int scale,
    const int tileSize) 
    :
    m_Scale(scale),
    m_TileSize(tileSize) 
{
    if (IsStrEmpty(textureID))
        LogErr("input texture ID is empty!");

    sprintf(g_String, "creation of a new map with texture ID: %s", textureID);
    LogMsg(g_String);

    m_TextureID = textureID;
}

///////////////////////////////////////////////////////////

void Map::LoadMap(
    const char* filePath,
    const int mapSizeX,
    const int mapSizeY)
{
    // read the map file definitions from the .map file

    if (IsStrEmpty(filePath))
    {
        LogErr("input filePath is empty!");
        return;
    }


    FILE* pFile = nullptr;
    
    if ((pFile = fopen(filePath, "r+")) == nullptr)
    {
        LogErr("can't open a file by path: %s", filePath);
        return;
    }

    // read tiles positions encoded in 2 chars, for instance:
    // a) 21 = tile at row 2, column 1
    // b) 13 = tile at row 1, column 3
    // c) 09 = tile at row 0, column 9
    char ch = 0;
    int srcRectX = 0;
    int srcRectY = 0;
    const int tileSize = m_TileSize;
    const int scaleFactor = m_Scale * tileSize;

    for (int y = 0; y < mapSizeY; ++y)
    {
        for (int x = 0; x < mapSizeX; ++x)
        {
            // read in the tile row and compute tile's posY on the tile texture
            ch = fgetc(pFile);
            srcRectY = atoi(&ch) * tileSize;

            // read in the tile column and compute tile's posX on the tile texture
            ch = fgetc(pFile);
            srcRectX = atoi(&ch) * tileSize;

            // create and setup new tile entity
            AddTile(srcRectX, srcRectY, x * scaleFactor, y * scaleFactor);

            // ignore ","
            ch = fgetc(pFile);
        }
    }

    fclose(pFile);
}

///////////////////////////////////////////////////////////

void Map::AddTile(
    const int srcRectX,    // X-position of tile on tilemap
    const int srcRectY,    // Y-position of tile on tilemap
    const int x,           // X-position of tile on the screen
    const int y)           // Y-position of tile on the screen
{
    // add a new tile entity in the game scene
    Entity& newTile = g_EntityMgr.AddEntity("Tile");
    newTile.AddComponent<TileComponent>(
        srcRectX, srcRectY, 
        x, y, 
        m_TileSize, 
        m_Scale, 
        m_TextureID.c_str());
}
