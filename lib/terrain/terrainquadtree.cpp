#include "terrainquadtree.hpp"

TerrainQuadTree::TerrainQuadTree() {
    rootNode = new TerrainChunk();
}