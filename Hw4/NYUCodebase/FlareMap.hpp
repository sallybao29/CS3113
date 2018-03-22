#ifndef FlareMap_hpp
#define FlareMap_hpp

#include <stdio.h>
#include <string>
#include <vector>

class ShaderProgram;

struct FlareMapEntity {
    std::string type;
    float x;
    float y;
};

class FlareMap {
public:
    FlareMap(float tileSize);
    ~FlareMap();
    void Load(std::string filename);
    void SetSpriteSheet(int sheet, size_t spriteCountX, size_t spriteCountY);
    void Render(ShaderProgram& shader);
    
    unsigned int** mapData;
    std::vector<FlareMapEntity> entities;
    float tileSize;
    size_t mapWidth;
    size_t mapHeight;
    
    int spriteSheetTexture;
    size_t spriteCountX;
    size_t spriteCountY;
    
private:
    bool ReadHeader(std::ifstream& stream);
    bool ReadLayerData(std::ifstream& stream);
    bool ReadEntityData(std::ifstream& stream);
};

#endif

