#pragma once

#include "Engine/Renderer/Texture.h"
#include <memory>
#include <string>
#include <vector>

class Tilemap {
public:
    Tilemap() = default;

    void Create(int width, int height, int tileSize);
    void Clear(int value = 0);

    int  GetWidth() const { return m_Width; }
    int  GetHeight() const { return m_Height; }
    int  GetTileSize() const { return m_TileSize; }

    int  GetTile(int x, int y) const;
    void SetTile(int x, int y, int id);

    void SetTileset(std::shared_ptr<Texture> tileset, int tilesPerRow);
    std::shared_ptr<Texture> GetTileset() const { return m_Tileset; }
    int GetTilesPerRow() const { return m_TilesPerRow; }

    void Render(SDL_Renderer* renderer, float cameraX, float cameraY, float viewW, float viewH);

    // Paint helpers
    void PaintTile(float worldX, float worldY, int id);
    int  PickTileIdFromTileset(float localX, float localY) const;

private:
    int m_Width = 0;
    int m_Height = 0;
    int m_TileSize = 32;
    int m_TilesPerRow = 1;

    std::vector<int> m_Tiles; // row-major
    std::shared_ptr<Texture> m_Tileset;
};