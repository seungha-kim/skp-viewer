#include "build.h"
#include "render/RenderModel.h"
#include "runtime/RuntimeModel.h"
#include <limits>
#include <memory>
#include <stack>

namespace acon {

static RenderVertex convertVertex(const Vertex& vertex) {
    return RenderVertex {
        .pos = vertex.position,
        .normal = vertex.normal,
        .faceNormal = vertex.faceNormal,
        .frontTexCoord = vertex.frontTexCoord,
        .backTexCoord = vertex.backTexCoord,
    };
}

static BoundingBox buildVertices(const AbstractReader& reader, UnitId unitId, std::vector<RenderVertex>& vertices) {
    glm::vec3 max {std::numeric_limits<float>::lowest()};
    glm::vec3 min {std::numeric_limits<float>::max()};

    unsigned faceCount = reader.getUnitTriangleCount(unitId);
    for (int i = 0; i < faceCount; i++) {
        auto face = reader.getUnitTriangle(unitId, i);
        for (const auto& vertex: face.vertices) {
            auto renderVertex = convertVertex(vertex);

            max = glm::max(renderVertex.pos, max);
            min = glm::min(renderVertex.pos, min);

            vertices.push_back(renderVertex);
        }
    }
    return BoundingBox {
        .min = min,
        .max = max,
    };
}

std::pair<std::unique_ptr<RuntimeModel>, std::unique_ptr<RenderModel>> buildModel(const AbstractReader& reader) {
    auto runtimeModel = std::make_unique<RuntimeModel>();
    auto renderModel = std::make_unique<RenderModel>();

    // Tags
    for (int i = 0; i < reader.getTagCount(); i++) {
        const auto tagId = reader.getTag(i);
        const auto tagName = reader.getTagName(tagId);
        RuntimeTagData tagData {
            .id = tagId,
            .name = tagName,
            .visibility = true,
            .objects = {},
        };

        runtimeModel->m_tagData.emplace(tagId, std::move(tagData));
        runtimeModel->m_tagList.push_back(tagId);
    }

    // Traverse all objects

    struct DfsItem {
        ObjectId id;
        std::optional<ObjectId> parentId;
        glm::mat4 ancestorTransform;
    };

    std::stack<DfsItem> dfsStack({{
        .id = ROOT_OBJECT_ID,
        .parentId = {},
        .ancestorTransform = glm::mat4(1.0f),
    }});

    while (!dfsStack.empty()) {
        auto item = dfsStack.top();
        dfsStack.pop();

        RuntimeObjectData objectData {};
        objectData.name = reader.getObjectName(item.id);
        objectData.visibility = true; // TODO
        objectData.transform = reader.getObjectTransform(item.id);
        objectData.parentIdOpt = item.parentId;

        auto worldTransform = item.ancestorTransform * objectData.transform;

        auto unitCount = reader.getObjectUnitCount(item.id);
        for (int i = 0; i < unitCount; i++) {
            RenderMaterial backMaterial {};
            RenderMaterial frontMaterial {};
            auto unitId = reader.getObjectUnit(item.id, i);

            if (auto backMaterialIdOpt = reader.getUnitBackMaterial(unitId)) {
                auto backMaterialId = backMaterialIdOpt.value();

                if (reader.getMaterialHasTexture(backMaterialId)) {
                    auto backTextureId = reader.getMaterialTexture(backMaterialId);
                    backMaterial.textureOpt = backTextureId;

                    if (!renderModel->m_textures.contains(backTextureId)) {
                        auto textureData = reader.copyTextureData(backTextureId);
                        auto backTexture = std::make_unique<RenderTexture>(std::move(textureData));
                        renderModel->m_textures[backTextureId] = std::move(backTexture);
                    }
                } else if (reader.getMaterialHasColor(backMaterialId)) {
                    backMaterial.color = reader.getMaterialColor(backMaterialId);
                }

                if (reader.getMaterialHasOpacity(backMaterialId)) {
                    backMaterial.use_opacity = true;
                    backMaterial.opacity = reader.getMaterialOpacity(backMaterialId);
                }
            }

            if (auto frontMaterialIdOpt = reader.getUnitFrontMaterial(unitId)) {
                auto frontMaterialId = frontMaterialIdOpt.value();

                if (reader.getMaterialHasTexture(frontMaterialId)) {
                    auto frontTextureId = reader.getMaterialTexture(frontMaterialId);
                    frontMaterial.textureOpt = frontTextureId;

                    if (!renderModel->m_textures.contains(frontTextureId)) {
                        auto textureData = reader.copyTextureData(frontTextureId);
                        auto frontTexture = std::make_unique<RenderTexture>(std::move(textureData));
                        renderModel->m_textures[frontTextureId] = std::move(frontTexture);
                    }
                } else if (reader.getMaterialHasColor(frontMaterialId)) {
                    frontMaterial.color = reader.getMaterialColor(frontMaterialId);
                }

                if (reader.getMaterialHasOpacity(frontMaterialId)) {
                    frontMaterial.use_opacity = true;
                    frontMaterial.opacity = reader.getMaterialOpacity(frontMaterialId);
                }
            }

            std::vector<RenderVertex> vertices {};
            auto boundingBox = buildVertices(reader, unitId, vertices);
            auto unit = std::make_unique<RenderUnit>(
                unitId,
                item.id,
                std::move(vertices),
                boundingBox,
                worldTransform,
                frontMaterial,
                backMaterial,
                renderModel->m_textures);
            renderModel->m_units.push_back(std::move(unit));
        }

        auto childCount = reader.getObjectChildrenCount(item.id);
        for (int i = 0; i < childCount; i++) {
            auto childId = reader.getObjectChild(item.id, i);
            dfsStack.push({
                .id = childId,
                .parentId = item.id,
                .ancestorTransform = worldTransform,
            });
            objectData.children.push_back(childId);
        }

        runtimeModel->m_objectData.emplace(item.id, std::move(objectData));
    }

    // Tag - Object relation
    for (const auto tagId: runtimeModel->m_tagList) {
        const auto tagObjectCount = reader.getTagObjectCount(tagId);
        for (int j = 0; j < tagObjectCount; j++) {
            const auto objectId = reader.getTagObject(tagId, j);
            runtimeModel->m_tagData.at(tagId).objects.insert(objectId);
            runtimeModel->m_objectData.at(objectId).tagIdOpt = tagId;
        }
    }

    return std::make_pair(std::move(runtimeModel), std::move(renderModel));
}

}
