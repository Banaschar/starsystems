#ifndef BASETYPES_HPP
#define BASETYPES_HPP

#include <string>

enum class ShaderType {SHADER_TYPE_DEFAULT, SHADER_TYPE_LIGHT, SHADER_TYPE_TERRAIN, SHADER_TYPE_SKY, SHADER_TYPE_WATER, SHADER_TYPE_WATER_PERFORMANCE,
                        SHADER_TYPE_GUI, SHADER_TYPE_POST_PROCESSOR, SHADER_TYPE_DEBUG};

struct Texture {
    unsigned int id;
    std::string type;
    
    Texture() {}
    Texture(unsigned int id_, std::string type_) : id(id_), type(type_) {}
};

#endif