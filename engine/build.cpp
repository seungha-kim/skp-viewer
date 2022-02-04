#include "build.h"
#include "runtime/RuntimeModel.h"
#include "render/RenderModel.h"
#include <stack>
#include <memory>

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

static inline RenderMaterial defaultMaterial() {
    // TODO: 리더마다 기본값이 다를 수 있음 - e.g. 스케치업 기본 색과 블렌더 기본 색이 다를 수 있음.
    return glm::vec3(0.7f, 0.7f, 0.7f);
}

static void buildVertices(const AbstractReader& reader, UnitId unitId, std::vector<RenderVertex>& vertices) {
    unsigned faceCount = reader.getUnitTriangleCount(unitId);
    for (int i = 0; i < faceCount; i++) {
        auto face = reader.getUnitTriangle(unitId, i);
        for (const auto& vertex : face.vertices) {
            auto renderVertex = convertVertex(vertex);

            vertices.push_back(renderVertex);
        }
    }
}

std::pair<std::unique_ptr<RuntimeModel>, std::unique_ptr<RenderModel>> buildModel(const AbstractReader &reader) {
    auto runtimeModel = std::make_unique<RuntimeModel>();
    auto renderModel = std::make_unique<RenderModel>();

    struct DfsItem {
        ObjectId id;
        glm::mat4 ancestorTransform;
    };

    std::stack<DfsItem> dfsStack({{
            .id = ROOT_OBJECT_ID,
            .ancestorTransform = glm::mat4(1.0f),
    }});

    while (!dfsStack.empty()) {
        auto item = dfsStack.top();
        dfsStack.pop();

        auto transform = item.ancestorTransform * reader.getObjectTransform(item.id);

        auto unitCount = reader.getObjectUnitCount(item.id);
        for (int i = 0; i < unitCount; i++) {
            RenderMaterial backMaterial = defaultMaterial();
            RenderMaterial frontMaterial = defaultMaterial();
            auto unitId = reader.getObjectUnit(item.id, i);

            if (auto backMaterialIdOpt = reader.getUnitBackMaterial(unitId)) {
                auto backMaterialId = backMaterialIdOpt.value();

                if (reader.getMaterialHasTexture(backMaterialId)) {
                    auto backTextureId = reader.getMaterialTexture(backMaterialId);
                    backMaterial = backTextureId;

                    if (!renderModel->m_textures.contains(backTextureId)) {
                        const auto textureData = reader.copyTextureData(backTextureId);
                        auto backTexture = std::make_unique<RenderTexture>(*textureData);
                        renderModel->m_textures[backTextureId] = std::move(backTexture);
                    }
                } else if (reader.getMaterialHasColor(backMaterialId)) {
                    backMaterial = reader.getMaterialColor(backMaterialId);
                }
            }

            if (auto frontMaterialIdOpt = reader.getUnitFrontMaterial(unitId)) {
                auto frontMaterialId = frontMaterialIdOpt.value();

                if (reader.getMaterialHasTexture(frontMaterialId)) {
                    auto frontTextureId = reader.getMaterialTexture(frontMaterialId);
                    frontMaterial = frontTextureId;

                    if (!renderModel->m_textures.contains(frontTextureId)) {
                        const auto textureData = reader.copyTextureData(frontTextureId);
                        auto frontTexture = std::make_unique<RenderTexture>(*textureData);
                        renderModel->m_textures[frontTextureId] = std::move(frontTexture);
                    }
                } else if (reader.getMaterialHasColor(frontMaterialId)) {
                    frontMaterial = reader.getMaterialColor(frontMaterialId);
                }
            }

            std::vector<RenderVertex> vertices;
            buildVertices(reader, unitId, vertices);
            auto mesh = std::make_unique<RenderMesh>(unitId, vertices, transform, frontMaterial, backMaterial, renderModel->m_textures);
            renderModel->m_meshes.push_back(std::move(mesh));
        }

        auto childCount = reader.getObjectChildrenCount(item.id);
        for (int i = 0; i < childCount; i++) {
            dfsStack.push({
                    .id = reader.getObjectChild(item.id, i),
                    .ancestorTransform = transform
            });
        }
    }

    return std::make_pair(std::move(runtimeModel), std::move(renderModel));
}

}
