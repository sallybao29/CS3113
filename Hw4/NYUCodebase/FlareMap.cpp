#include <fstream>
#include <sstream>
#include <cassert>
#include <SDL_opengl.h>
#include "FlareMap.hpp"
#include "ShaderProgram.h"

FlareMap::FlareMap(float tileSize)
: tileSize(tileSize), mapData(nullptr), mapWidth(0), mapHeight(0) {}

void FlareMap::SetSpriteSheet(int sheet, size_t spriteCountX, size_t spriteCountY) {
    spriteSheetTexture = sheet;
    this->spriteCountX = spriteCountX;
    this->spriteCountY = spriteCountY;
}

FlareMap::~FlareMap() {
    for (int i = 0; i < mapHeight; i++) {
        delete [] mapData[i];
    }
    delete [] mapData;
}

void FlareMap::Load(std::string filename) {
    std::ifstream infile(filename);
    if (infile.fail()) {
        assert(false);
    }
    std::string line;
    while (getline(infile, line)) {
        if (line == "[header]") {
            if (!ReadHeader(infile)) assert(false);
        }
        else if (line == "[layer]") {
            ReadLayerData(infile);
        }
        else if (line == "[ObjectsLayer]") {
            ReadEntityData(infile);
        }
    }
    infile.close();
}

bool FlareMap::ReadHeader(std::ifstream& stream) {
    std::string line;
    int width = -1;
    int height = -1;
    while (getline(stream, line)) {
        if (line == "") { break; }
        std::istringstream sStream(line);
        std::string key,value;
        std::getline(sStream, key, '=');
        std::getline(sStream, value);
        if (key == "width") {
            width = atoi(value.c_str());
        }
        else if (key == "height"){
            height = atoi(value.c_str());
        }
    }
    if (width == -1 || height == -1) {
        return false;
    }
    else {
        // allocate our map data
        mapWidth = width;
        mapHeight = height;
        mapData = new unsigned int*[mapHeight];
        for (int i = 0; i < mapHeight; ++i) {
            mapData[i] = new unsigned int[mapWidth];
        }
        return true;
    }
}

bool FlareMap::ReadLayerData(std::ifstream& stream) {
    std::string line;
    while (getline(stream, line)) {
        if (line == "") break;
        std::istringstream sStream(line);
        std::string key,value;
        std::getline(sStream, key, '=');
        std::getline(sStream, value);
        if (key == "data") {
            for (int y = 0; y < mapHeight; y++) {
                getline(stream, line);
                std::istringstream lineStream(line);
                std::string tile;
                for (int x = 0; x < mapWidth; x++) {
                    getline(lineStream, tile, ',');
                    unsigned int val =  (unsigned int)atoi(tile.c_str());
                    // Tiles in this format are indexed from 1 not 0
                    mapData[y][x] = val;
                }
            }
        }
    }
    return true;
}

bool FlareMap::ReadEntityData(std::ifstream& stream) {
    std::string line;
    std::string type;
    while (getline(stream, line)) {
        if (line == "") { break; }
        std::string key,value;
        std::istringstream sStream(line);
        std::getline(sStream, key, '=');
        std::getline(sStream, value);
        if (key == "type") {
            type = value;
        }
        else if (key == "location") {
            std::istringstream lineStream(value);
            std::string xPosition, yPosition;
            getline(lineStream, xPosition, ',');
            getline(lineStream, yPosition, ',');
            
            FlareMapEntity newEntity;
            newEntity.type = type;
            newEntity.x = std::atoi(xPosition.c_str());
            newEntity.y = std::atoi(yPosition.c_str());
            entities.push_back(newEntity);
        }
    }
    return true;
}

void FlareMap::Render(ShaderProgram& shader) {
    std::vector<float> vertexData;
    std::vector<float> texCoordData;
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            // Skip empty tiles
            if (mapData[y][x] == 0) continue;
            // Tiles in this format are indexed from 1 not 0
            float u = (float)((mapData[y][x] - 1) % spriteCountX) / (float) spriteCountX;
            float v = (float)((mapData[y][x] - 1) / spriteCountX) / (float) spriteCountY;
            float spriteWidth = 1.0f/(float)spriteCountX;
            float spriteHeight = 1.0f/(float)spriteCountY;
            vertexData.insert(vertexData.end(), {
                tileSize * x, -tileSize * y,
                tileSize * x, (-tileSize * y)-tileSize,
                (tileSize * x)+tileSize, (-tileSize * y)-tileSize,
                
                tileSize * x, -tileSize * y,
                (tileSize * x)+tileSize, (-tileSize * y)-tileSize,
                (tileSize * x)+tileSize, -tileSize * y
            });
            texCoordData.insert(texCoordData.end(), {
                u, v,
                u, v + spriteHeight,
                u + spriteWidth, v + spriteHeight,
                
                u, v,
                u + spriteWidth, v + spriteHeight,
                u + spriteWidth, v
            });
        }
    }
    
    glBindTexture(GL_TEXTURE_2D, spriteSheetTexture);
    glVertexAttribPointer(shader.positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
    glEnableVertexAttribArray(shader.positionAttribute);
    glVertexAttribPointer(shader.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
    glEnableVertexAttribArray(shader.texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, (int)vertexData.size() / 2);
    
    glDisableVertexAttribArray(shader.positionAttribute);
    glDisableVertexAttribArray(shader.texCoordAttribute);
}

void FlareMap::worldToTileCoordinates(float worldX, float worldY, int& gridX, int& gridY) const {
    gridX = worldToTileCoordX(worldX);
    gridY = worldToTileCoordY(worldY);
}

int FlareMap::worldToTileCoordX(float worldX) const {
    return (int) (worldX / tileSize);
}

int FlareMap::worldToTileCoordY(float worldY) const {
    return (int) (-worldY / tileSize);
}
