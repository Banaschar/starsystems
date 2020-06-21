#include "oglheader.hpp"
#include "vaorenderer.hpp"

VaoRenderer::VaoRenderer(){}

void VaoRenderer::draw(Mesh &mesh) {
    glBindVertexArray(mesh.getVao());

    if (mesh.isInstanced())
        glDrawElementsInstanced(GL_TRIANGLES, mesh.getIndicesSize(), GL_UNSIGNED_INT, 0, mesh.getInstanceSize());
    else 
        glDrawElements(GL_TRIANGLES, mesh.getIndicesSize(), GL_UNSIGNED_INT, 0);
    
    glBindVertexArray(0);
}