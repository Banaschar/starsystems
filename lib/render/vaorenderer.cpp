#include "vaorenderer.hpp"
#include "oglheader.hpp"

VaoRenderer::VaoRenderer() {}

/*
 * Draws the vertex array objects
 * Checks if mesh is incomplete (so mesh data can be generated on different threads)
 */
void VaoRenderer::draw(Mesh &mesh) {
    if (mesh.incomplete())
        mesh.updateMesh();

    glBindVertexArray(mesh.getVao());

    if (mesh.isInstanced())
        glDrawElementsInstanced(GL_TRIANGLES, mesh.getIndicesSize(), GL_UNSIGNED_INT, 0, mesh.getInstanceSize());
    else
        glDrawElements(GL_TRIANGLES, mesh.getIndicesSize(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}