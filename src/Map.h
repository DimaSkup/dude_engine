#ifndef MAP_H
#define MAP_H

#include <string>

class Map
{
public:
    Map(const char* textureID, const int scale, const int tileSize);
    ~Map() {};

    void LoadMap(
        const char* filePath, 
        const int mapSizeX, 
        const int mapSizeY);

    void AddTile(
        const int srcRectX, 
        const int srcRectY, 
        const int x, 
        const int y);

private:
    std::string m_TextureID;
    int m_Scale = 0;
    int m_TileSize = 0;
};

#endif
