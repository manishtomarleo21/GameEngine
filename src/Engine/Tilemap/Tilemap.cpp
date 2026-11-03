#include "Engine/Tilemap/Tilemap.h"
#include <algorithm>

void Tilemap::Create(int width, int height, int tileSize) {
    m_Width = std::max(1, width);
    m_Height = std::max(1, height);
    m_TileSize = std::max(1, tileSize);
    m_Tiles.assign(static_cast<size_t>(m_Width * m_Height), 0);
}

void Tilemap::Clear(int value) {
    std::fill(m_Tiles.begin(), m_Tiles.end(), value);
}

int Tilemap::GetTile(int x, int y) const {
    if (x < 0 || y < 0 || x >= m_Width || y >= m_Height) return 0;
    return m_Tiles[static_cast<size_t>(y * m_Width + x)];
}

void Tilemap::SetTile(int x, int y, int id) {
    if (x < 0 || y < 0 || x >= m_Width || y >= m_Height) return;
    m_Tiles[static_cast<size_t>(y * m_Width + x)] = id;
}

void Tilemap::SetTileset(std::shared_ptr<Texture> tileset, int tilesPerRow) {
    m_Tileset = tileset;
    m_TilesPerRow = std::max(1, tilesPerRow);
}

void Tilemap::PaintTile(float worldX, float worldY, int id) {
    int tx = static_cast<int>(worldX) / m_TileSize;
    int ty = static_cast<int>(worldY) / m_TileSize;
    SetTile(tx, ty, id);
}

int Tilemap::PickTileIdFromTileset(float localX, float localY) const {
    if (!m_Tileset) return 0;
    int col = static_cast<int>(localX) / m_TileSize;
    int row = static_cast<int>(localY) / m_TileSize;
    if (col < 0 || row < 0) return 0;
    return row * m_TilesPerRow + col + 1; // 0 = empty, 1+ = tile ids
}

void Tilemap::Render(SDL_Renderer* renderer, float cameraX, float cameraY, float viewW, float viewH) {
    if (!m_Tileset || !m_Tileset->GetSDLTexture()) return;

    int startX = std::max(0, static_cast<int>(cameraX) / m_TileSize);
    int startY = std::max(0, static_cast<int>(cameraY) / m_TileSize);
    int endX = std::min(m_Width,  static_cast<int>(cameraX + viewW) / m_TileSize + 1);
    int endY = std::min(m_Height, static_cast<int>(cameraY + viewH) / m_TileSize + 1);

    for (int y = startY; y < endY; ++y) {
        for (int x = startX; x < endX; ++x) {
            int id = GetTile(x, y);
            if (id <= 0) continue;

            int tileIndex = id - 1;
            int srcCol = tileIndex % m_TilesPerRow;
            int srcRow = tileIndex / m_TilesPerRow;

            SDL_FRect src = {
                static_cast<float>(srcCol * m_TileSize),
                static_cast<float>(srcRow * m_TileSize),
                static_cast<float>(m_TileSize),
                static_cast<float>(m_TileSize)
            };

            SDL_FRect dst = {
                x * m_TileSize - cameraX,
                y * m_TileSize - cameraY,
                static_cast<float>(m_TileSize),
                static_cast<float>(m_TileSize)
            };

            SDL_RenderTexture(renderer, m_Tileset->GetSDLTexture(), &src, &dst);
        }
    }
}