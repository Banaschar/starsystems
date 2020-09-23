#include "vaorenderer.hpp"
#include "oglheader.hpp"
#include "global.hpp"

VaoRenderer::VaoRenderer() {}

/*
 * Draws the vertex array objects
 * Checks if mesh is incomplete (so mesh data can be generated on different threads)
 */
void VaoRenderer::draw(Mesh *mesh) {
    if (mesh->incomplete())
        mesh->updateMesh();

    glBindVertexArray(mesh->getVao());

    if (mesh->isInstanced())
        glDrawElementsInstanced(mesh->getDrawMode(), mesh->getIndicesSize(), GL_UNSIGNED_INT, 0, mesh->getInstanceSize());
    else 
        glDrawElements(mesh->getDrawMode(), mesh->getIndicesSize(), GL_UNSIGNED_INT, 0);

    g_triangleCount += mesh->getTriangleCount();

    glBindVertexArray(0);
}