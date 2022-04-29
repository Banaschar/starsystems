#ifndef STARSYSTEMS_GRAPHICAPI_HPP
#define STARSYSTEMS_GRAPHICAPI_HPP

class GraphicApiBuffer {
    virtual ~GraphicApiBuffer() = 0;
    virtual unsigned int getBuffer() = 0;
    virtual unsigned int bindBuffer() = 0;
};

class GraphicApiObject {
    virtual ~GraphicApiObject() = 0;
};

class GraphicApiRender {
    virtual void render(GraphicApiObject obj) = 0;
};
#endif
