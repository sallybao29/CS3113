#include <fstream>
#include <sstream>
#include <vector>
#include <SDL_opengl.h>
#include "Tilemap.hpp"
#include "ShaderProgram.h"

Tilemap::Tilemap(float tileSize) : tileSize(tileSize) {}

void Tilemap::SetSpriteSheet(int sheet, size_t spriteCountX, size_t spriteCountY) {
    spriteSheetTexture = sheet;
    this->spriteCountX = spriteCountX;
    this->spriteCountY = spriteCountY;
}

void Tilemap::GetKeyValuePair(const std::string& line, std::string& key, std::string& value) const {
    std::istringstream sStream(line);
    getline(sStream, key, '=');
    getline(sStream, value);
}

void Tilemap::LoadFromFile(std::string filename) {
    std::ifstream infile(filename);
    std::string line;
    while (getline(infile, line)) {
        if (line == "[header]") {
            if (!ReadHeader(infile)) break;
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

bool Tilemap::ReadHeader(std::ifstream& stream) {
    std::string line;
    std::string key,value;
    int width = -1;
    int height = -1;
    while (getline(stream, line)) {
        if (line == "") { break; }
        GetKeyValuePair(line, key, value);
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
        tilemap = new unsigned int*[mapHeight];
        for (int i = 0; i < mapHeight; ++i) {
            tilemap[i] = new unsigned int[mapWidth];
        }
        return true;
    }
}

bool Tilemap::ReadLayerData(std::ifstream& stream) {
    std::string line;
    std::string key,value;
    while (getline(stream, line)) {
        if (line == "") break;
        GetKeyValuePair(line, key, value);
        if (key == "data") {
            for (int y = 0; y < mapHeight; y++) {
                getline(stream, line);
                std::istringstream lineStream(line);
                std::string tile;
                for (int x = 0; x < mapWidth; x++) {
                    getline(lineStream, tile, ',');
                    unsigned char val =  (unsigned char)atoi(tile.c_str());
                    // Tiles in this format are indexed from 1 not 0
                    tilemap[y][x] = val >  0 ? val - 1 : 0;
                }
            }
        }
    }
    return true;
}

bool Tilemap::ReadEntityData(std::ifstream& file) {
    return true;
}

void Tilemap::Render(ShaderProgram& shader) {
    std::vector<float> vertexData;
    std::vector<float> texCoordData;
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            float u = (float)(((int)tilemap[y][x]) % spriteCountX) / (float) spriteCountX;
            float v = (float)(((int)tilemap[y][x]) / spriteCountX) / (float) spriteCountY;
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
                u+spriteWidth, v+(spriteHeight),
                u+spriteWidth, v
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
