#include "SketchupReader.h"
#include <stack>
#include <vector>
#include <map>
#include <glm/gtc/type_ptr.hpp>

struct SketchupUnit {
    std::optional<MaterialId> frontMaterialId;
    std::optional<MaterialId> backMaterialId;
    std::vector<Triangle> triangles;
};

struct SketchupUnitHolder {
    std::unordered_map<UnitId, SketchupUnit> map;
};

// TODO: GUID 활용해서 안정적으로 ObjectId 알 수 있도록 하기
// TODO: 생각해보니 overriding 가능한 놈들은 전부 string 을 ID로 쓰긴 해야겠어..
// https://extensions.sketchup.com/developers/sketchup_c_api/sketchup/struct_s_u_group_ref.html#a51133d3d309c5cdfdc35c7b3d13196c7
struct SketchupObject {
    std::string name;
    glm::mat4 transform{};
    std::vector<UnitId> units;
    std::vector<ObjectId> children;
};

struct SketchupObjectDescription {
    SUEntitiesRef entities;
    std::optional<ObjectId> parentObjectId;
    std::string name;
    glm::mat4 transform;
};

struct SketchupObjectHolder {
    std::unordered_map<ObjectId, SketchupObject> map;
};

static void check(SUResult result) {
    if (result != SUResult::SU_ERROR_NONE) {
        throw std::runtime_error("ERROR");
    }
}

static glm::vec3 convertPoint3D(SUPoint3D point);
static glm::vec3 convertVector3D(SUVector3D vector);
static glm::mat4 convertTransform(SUTransformation transform);

SketchupReader::SketchupReader(std::string_view path)
        : m_unitHolder(std::make_unique<SketchupUnitHolder>())
        , m_objectHolder(std::make_unique<SketchupObjectHolder>())
        , m_model{} {
    SUInitialize();
    SUModelLoadStatus status{};
    check(SUModelCreateFromFileWithStatus(&m_model, path.data(), &status));
    if (status == SUModelLoadStatus_Success_MoreRecent) {
        // TODO: 로그 찍는 것 외에 programmatically 알 수 있게 처리
        printf("WARN: SUModelLoadStatus_Success_MoreRecent\n");
    }

    // material 전부 얻어오는 작업
    size_t numMaterials{}, numMaterialsActual{};
    check(SUModelGetNumMaterials(m_model, &numMaterials));
    std::vector<SUMaterialRef> materials(numMaterials);
    check(SUModelGetMaterials(m_model, numMaterials, materials.data(), &numMaterialsActual));
    for (const auto& material: materials) {
        m_materialMap[m_materialCount] = material;
        m_materialInverse[material] = m_materialCount;
        m_materialCount += 1;
    }

    SUEntitiesRef rootEntities{};
    check(SUModelGetEntities(m_model, &rootEntities));
    SUTransformation rootTransform {
        .values = {
                1.0, 0.0, 0.0, 0.0,
                0.0, 1.0, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                0.0, 0.0, 0.0, 1.0,
        }
    };
    std::stack<SketchupObjectDescription> dfsStack({SketchupObjectDescription {
        .entities = rootEntities,
        .parentObjectId = {},
        .name = "root",
        .transform = convertTransform(rootTransform),
    }});
    while (!dfsStack.empty()) {
        const auto desc = dfsStack.top();
        dfsStack.pop();

        const auto objectId = processObject(desc);

        // group
        size_t numGroups{}, numGroupsActual{};
        check(SUEntitiesGetNumGroups(desc.entities, &numGroups));
        std::vector<SUGroupRef> groups(numGroups);
        if (numGroups > 0) {
            check(SUEntitiesGetGroups(desc.entities, numGroups, groups.data(), &numGroupsActual));
        }

        for (const auto& group: groups) {
            SUEntitiesRef groupEntities{};
            SUStringRef nameRef{};
            SUTransformation transform{};
            check(SUGroupGetEntities(group, &groupEntities));

            SUGroupGetName(group, &nameRef);
            SUGroupGetTransform(group, &transform);

            size_t nameLength{}, nameLengthCopied{};
            SUStringGetUTF8Length(nameRef, &nameLength);
            std::string name(nameLength, '0');
            SUStringGetUTF8(nameRef, nameLength, name.data(), &nameLengthCopied);

            dfsStack.push(SketchupObjectDescription {
                    .entities = groupEntities,
                    .parentObjectId = objectId,
                    .name = name,
                    .transform = convertTransform(transform),
            });
        }

        // component
        size_t numInstances{}, numInstancesActual{};
        check(SUEntitiesGetNumInstances(desc.entities, &numInstances));
        std::vector<SUComponentInstanceRef> instances(numInstances);
        if (numInstances > 0) {
            check(SUEntitiesGetInstances(desc.entities, numInstances, instances.data(), &numInstancesActual));
        }

        for (const auto& instance: instances) {
            // TODO: reuse definition - Mesh 단위 추가
            SUComponentDefinitionRef definition{};
            SUEntitiesRef definitionEntities{};
            check(SUComponentInstanceGetDefinition(instance, &definition));
            check(SUComponentDefinitionGetEntities(definition, &definitionEntities));

            SUStringRef nameRef{};
            SUTransformation transform{};
            SUComponentInstanceGetName(instance, &nameRef);
            SUComponentInstanceGetTransform(instance, &transform);

            size_t nameLength{}, nameLengthCopied{};
            SUStringGetUTF8Length(nameRef, &nameLength);
            std::string name(nameLength, '0');
            SUStringGetUTF8(nameRef, nameLength, name.data(), &nameLengthCopied);

            dfsStack.push(SketchupObjectDescription {
                    .entities = definitionEntities,
                    .parentObjectId = objectId,
                    .name = name,
                    .transform = convertTransform(transform),
            });
        }
    }
}

SketchupReader::~SketchupReader() {
    check(SUModelRelease(&m_model));
    SUTerminate();
}

bool SketchupReader::hasObject(ObjectId id) const {
    return m_objectHolder->map.count(id) > 0;
}

std::string SketchupReader::getObjectName(ObjectId id) const {
    return m_objectHolder->map.at(id).name;
}

glm::mat4 SketchupReader::getObjectTransform(ObjectId id) const {
    return m_objectHolder->map.at(id).transform;
}

unsigned int SketchupReader::getObjectUnitCount(ObjectId id) const {
    return m_objectHolder->map.at(id).units.size();
}

UnitId SketchupReader::getObjectUnit(ObjectId id, int index) const {
    return m_objectHolder->map.at(id).units[index];
}

bool SketchupReader::getObjectVisibility(ObjectId id) const {
    return true;
}

unsigned int SketchupReader::getObjectChildrenCount(ObjectId id) const {
    return m_objectHolder->map.at(id).children.size();
}

ObjectId SketchupReader::getObjectChild(ObjectId id, int index) const {
    return m_objectHolder->map.at(id).children[index];
}

bool SketchupReader::hasUnit(UnitId unitId) const {
    return m_unitHolder->map.count(unitId) > 0;
}

std::optional<MaterialId> SketchupReader::getUnitFrontMaterial(UnitId id) const {
    return m_unitHolder->map.at(id).frontMaterialId;
}

std::optional<MaterialId> SketchupReader::getUnitBackMaterial(UnitId id) const {
    return m_unitHolder->map.at(id).backMaterialId;
}

unsigned int SketchupReader::getUnitTriangleCount(UnitId id) const {
    return m_unitHolder->map.at(id).triangles.size();
}

Triangle SketchupReader::getUnitTriangle(UnitId id, int index) const {
    return m_unitHolder->map.at(id).triangles[index];
}

bool SketchupReader::hasMaterial(MaterialId materialId) const {
    return m_materialMap.count(materialId) > 0;
}

glm::vec3 SketchupReader::getMaterialDiffuseRGB(MaterialId id) const {
    return glm::vec3(); // TODO
}

TextureData SketchupReader::getMaterialTextureData(MaterialId id) const {
    return TextureData(); // TODO
}

ObjectId SketchupReader::processObject(const SketchupObjectDescription& desc) {
    std::vector<UnitId> units;

    // face 전부 얻어오는 작업
    size_t numFaces, numFacesActual;
    check(SUEntitiesGetNumFaces(desc.entities, &numFaces));
    std::vector<SUFaceRef> faces(numFaces);
    if (numFaces > 0) {
        check(SUEntitiesGetFaces(desc.entities, numFaces, faces.data(), &numFacesActual));
    }

    // face 를 material 별로 분류하는 작업
    using UnitKey = std::pair<std::optional<MaterialId>, std::optional<MaterialId>>;
    std::map<UnitKey, std::vector<SUFaceRef>> unitsByMaterial;
    for (const auto& face: faces) {
        SUMaterialRef frontMaterial{}, backMaterial{};
        bool hasFrontMaterial = SU_ERROR_NONE == SUFaceGetFrontMaterial(face, &frontMaterial);
        bool hasBackMaterial = SU_ERROR_NONE == SUFaceGetBackMaterial(face, &backMaterial);

        UnitKey key{};
        if (hasFrontMaterial) {
            key.first = m_materialInverse.at(frontMaterial);
        }
        if (hasBackMaterial) {
            key.second = m_materialInverse.at(backMaterial);
        }
        unitsByMaterial[key].push_back(face);
    }

    for (const auto& [pair, unitFaces]: unitsByMaterial) {
        const auto& [frontId, backId] = pair;
        std::vector<Triangle> triangles{};

        for (const auto& face: unitFaces) {
            // tessellate
            SUMeshHelperRef helper{};
            check(SUMeshHelperCreate(&helper, face));

            size_t numTriangles;
            check(SUMeshHelperGetNumTriangles(helper, &numTriangles));

            size_t numIndices = numTriangles * 3, numIndicesActual;
            std::vector<size_t> indices(numIndices);
            check(SUMeshHelperGetVertexIndices(helper, numIndices, indices.data(), &numIndicesActual));

            size_t numVertices, numVerticesActual;
            check(SUMeshHelperGetNumVertices(helper, &numVertices));
            std::vector<SUPoint3D> vertices(numVertices);
            std::vector<SUVector3D> normals(numVertices);
            std::vector<SUPoint3D> frontTexCoords(numVertices);
            std::vector<SUPoint3D> backTexCoords(numVertices);
            check(SUMeshHelperGetVertices(helper, numVertices, vertices.data(), &numVerticesActual));
            check(SUMeshHelperGetNormals(helper, numVertices, normals.data(), &numVerticesActual));
            check(SUMeshHelperGetFrontSTQCoords(helper, numVertices, frontTexCoords.data(), &numVerticesActual));
            check(SUMeshHelperGetBackSTQCoords(helper, numVertices, backTexCoords.data(), &numVerticesActual));

            for (int i = 0; i < numTriangles; i++) {
                Triangle triangle{};
                for (int j = 0; j < 3; j++) {
                    size_t index = indices[3 * i + j];
                    triangle.vertices[j] = Vertex {
                            .position = convertPoint3D(vertices[index]),
                            .normal = convertVector3D(normals[index]),
                            .texCoord = convertPoint3D(frontTexCoords[index]),
                    };
                }
                triangles.push_back(triangle);
            }
            check(SUMeshHelperRelease(&helper));
        }
        auto unitId = m_unitCount++;
        m_unitHolder->map.emplace(unitId, SketchupUnit {
                .frontMaterialId = frontId,
                .backMaterialId = backId,
                .triangles = triangles,
        });
        units.push_back(unitId);
    }

    auto objectId = m_objectCount++;
    m_objectHolder->map[objectId] = SketchupObject {
        .name = desc.name,
        .transform = desc.transform,
        .units = units,
        .children = {},
    };
    if (desc.parentObjectId) {
        m_objectHolder->map.at(desc.parentObjectId.value()).children.push_back(objectId);
    }
    return objectId;
}

static float m(double length) {
    // inch -> meter
    return 0.0254f * float(length);
}

static glm::vec3 convertPoint3D(SUPoint3D point) {
    return glm::vec3(m(point.x), m(point.y), m(point.z));
}

static glm::vec3 convertVector3D(SUVector3D vector) {
    return glm::vec3(vector.x, vector.y, vector.z);
}

static glm::mat4 convertTransform(SUTransformation transform) {
    float values[16];

    for (int i = 0; i < 16; i++) {
        values[i] = transform.values[i];
    }
    // col-major -> col-major
    values[12] = m(values[12]);
    values[13] = m(values[13]);
    values[14] = m(values[14]);

    return glm::make_mat4(values);
}
