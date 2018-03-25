#ifndef SheetSprite_hpp
#define SheetSprite_hpp

#include <stdio.h>

class ShaderProgram;

class SheetSprite {
public:
    SheetSprite(unsigned int textureID, float u, float v, float width, float height, float aspect, float size);
    SheetSprite(unsigned int textureID, int index, int spriteCountX, int spriteCountY, float aspect, float size);
    
    void Render(ShaderProgram& program) const;
    
    unsigned int textureID;

    float u;
    float v;
    
    // Normalized width and height
    float width;
    float height;
    
    // Aspect ratio of actual sprite
    float aspect;
    float size;
    
    bool reversed;
};

#endif
