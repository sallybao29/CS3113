#ifndef SheetSprite_h
#define SheetSprite_h

class ShaderProgram;

class SheetSprite {
public:
    SheetSprite();
    SheetSprite(unsigned int textureID, float u, float v, float width, float height, float size);
    
    void Draw(ShaderProgram& program) const;
    float size;
    unsigned int textureID;
    float u;
    float v;
    float width;
    float height;
};

#endif
