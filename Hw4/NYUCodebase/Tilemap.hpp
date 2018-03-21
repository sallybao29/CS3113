#ifndef Tilemap_hpp
#define Tilemap_hpp

#include <stdio.h>
#include <string>

class ShaderProgram;

class Tilemap {
public:
    Tilemap(float tileSize);
    void LoadFromFile(std::string filename);
    void SetSpriteSheet(int sheet, size_t spriteCountX, size_t spriteCountY);
    void Render(ShaderProgram& shader);
    
private:
    void GetKeyValuePair(const std::string& line, std::string& key, std::string& value) const;
    bool ReadHeader(std::ifstream& file);
    bool ReadLayerData(std::ifstream& file);
    bool ReadEntityData(std::ifstream& file);
    void PlaceEntity();
    
    unsigned int** tilemap;
    float tileSize;
    size_t mapWidth;
    size_t mapHeight;
    
    int spriteSheetTexture;
    size_t spriteCountX;
    size_t spriteCountY;
};

#endif
