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

void Tilemap::Save(std::ostream& out) const {
    out << "TILEMAP " << m_Width << " " << m_Height << " " << m_TileSize << "\n";
    out << "TILES";
    for (int id : m_Tiles) {
        out << " " << id;
    }
    out << "\n";
}

void Tilemap::LoadFromValues(int width, int height, int tileSize, const std::vector<int>& tiles) {
    Create(width, height, tileSize);
    if (tiles.size() == m_Tiles.size()) {
        m_Tiles = tiles;
    }
}

void Tilemap::SetAllNonZeroSolid(bool enabled) {
    m_AllNonZeroSolid = enabled;
}

void Tilemap::SetSolidTile(int tileId, bool solid) {
    auto it = std::find(m_SolidTiles.begin(), m_SolidTiles.end(), tileId);
    if (solid && it == m_SolidTiles.end()) {
        m_SolidTiles.push_back(tileId);
    } else if (!solid && it != m_SolidTiles.end()) {
        m_SolidTiles.erase(it);
    }
}

bool Tilemap::IsSolid(int tileId) const {
    if (tileId <= 0) return false;
    if (m_AllNonZeroSolid) return true;

    return std::find(m_SolidTiles.begin(), m_SolidTiles.end(), tileId) != m_SolidTiles.end();
}

bool Tilemap::IsSolidAtWorld(float worldX, float worldY) const {
    int tx = static_cast<int>(worldX) / m_TileSize;
    int ty = static_cast<int>(worldY) / m_TileSize;
    return IsSolid(GetTile(tx, ty));
}

bool Tilemap::ResolveCollision(float& x, float& y, float w, float h, float& velX, float& velY) const {
    if (m_TileSize <= 0) return false;

    bool hit = false;

    int startTX = static_cast<int>(x) / m_TileSize;
    int endTX   = static_cast<int>(x + w - 0.001f) / m_TileSize;
    int startTY = static_cast<int>(y) / m_TileSize;
    int endTY   = static_cast<int>(y + h - 0.001f) / m_TileSize;

    // Resolve X first
    for (int ty = startTY; ty <= endTY; ++ty) {
        for (int tx = startTX; tx <= endTX; ++tx) {
            if (!IsSolid(GetTile(tx, ty))) continue;

            float tileLeft   = static_cast<float>(tx * m_TileSize);
            float tileRight  = tileLeft + m_TileSize;
            float tileTop    = static_cast<float>(ty * m_TileSize);
            float tileBottom = tileTop + m_TileSize;

            // overlap check
            if (x + w <= tileLeft || x >= tileRight || y + h <= tileTop || y >= tileBottom) {
                continue;
            }

            hit = true;

            float overlapLeft  = (x + w) - tileLeft;
            float overlapRight = tileRight - x;

            if (overlapLeft < overlapRight) {
                x = tileLeft - w;
            } else {
                x = tileRight;
            }
            velX = 0.0f;
        }
    }

    // Recompute after X resolution
    startTX = static_cast<int>(x) / m_TileSize;
    endTX   = static_cast<int>(x + w - 0.001f) / m_TileSize;
    startTY = static_cast<int>(y) / m_TileSize;
    endTY   = static_cast<int>(y + h - 0.001f) / m_TileSize;

    // Resolve Y
    for (int ty = startTY; ty <= endTY; ++ty) {
        for (int tx = startTX; tx <= endTX; ++tx) {
            if (!IsSolid(GetTile(tx, ty))) continue;

            float tileLeft   = static_cast<float>(tx * m_TileSize);
            float tileRight  = tileLeft + m_TileSize;
            float tileTop    = static_cast<float>(ty * m_TileSize);
            float tileBottom = tileTop + m_TileSize;

            if (x + w <= tileLeft || x >= tileRight || y + h <= tileTop || y >= tileBottom) {
                continue;
            }

            hit = true;

            float overlapTop    = (y + h) - tileTop;
            float overlapBottom = tileBottom - y;

            if (overlapTop < overlapBottom) {
                y = tileTop - h;
            } else {
                y = tileBottom;
            }
            velY = 0.0f;
        }
    }

    return hit;
}