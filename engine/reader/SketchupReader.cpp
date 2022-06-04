#include "SketchupReader.h"
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <stdexcept>
#include <vector>

namespace acon {

static inline glm::vec3 convertPoint3D(SUPoint3D point);
static inline glm::vec2 convertTexCoord(SUUVQ uvq, float s_scale, float t_scale, bool isMaterialInherited);
static inline glm::vec3 convertVector3D(SUVector3D vector);
static inline glm::mat4 convertTransform(SUTransformation transform);
static inline glm::vec4 convertColor(SUColor color);
static void flipImageVertically(std::vector<unsigned char>& data, int width, int height, int channels);
static std::string convertAndReleaseString(SUStringRef stringRef);

struct SketchupUnit {
    std::optional<MaterialId> frontMaterialId;
    std::optional<MaterialId> backMaterialId;
    bool isFrontMaterialInherited;
    bool isBackMaterialInherited;
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
    glm::mat4 transform {};
    std::vector<UnitId> units;
    std::vector<ObjectId> children;
};

struct SketchupObjectHolder {
    std::unordered_map<ObjectId, SketchupObject> map;
};

struct SketchupTextureMeta {
    int width;
    int height;
    float s_scale;
    float t_scale;
};

struct SketchupTextureMetaHolder {
    std::unordered_map<TextureId, SketchupTextureMeta> map;
};

class SketchupTextureData: public TextureData {
public:
    SketchupTextureData(int width, int height, int dataSize, int bitsPerPixel, std::vector<unsigned char>&& data)
            : m_width(width)
            , m_height(height)
            , m_dataSize(dataSize)
            , m_bitsPerPixel(bitsPerPixel)
            , m_data(std::move(data)) { }

    ~SketchupTextureData() override = default;

    [[nodiscard]] const unsigned char* data() const override { return m_data.data(); }

    [[nodiscard]] int dataSize() const override { return m_dataSize; }

    [[nodiscard]] int width() const override { return m_width; }

    [[nodiscard]] int height() const override { return m_height; }

    [[nodiscard]] bool hasAlpha() const override {
        // TODO: 32 아닌 경우?
        return m_bitsPerPixel != 32;
    }

private:
    int m_width, m_height, m_dataSize, m_bitsPerPixel;
    std::vector<unsigned char> m_data;
};

static void check(SUResult result) {
    if (result != SUResult::SU_ERROR_NONE) {
        throw std::runtime_error("ERROR");
    }
}

SketchupReader::SketchupReader(std::string_view path)
        : m_unitHolder(std::make_unique<SketchupUnitHolder>())
        , m_objectHolder(std::make_unique<SketchupObjectHolder>())
        , m_textureMetaHolder(std::make_unique<SketchupTextureMetaHolder>())
        , m_model {} {
    SUInitialize();
    SUModelLoadStatus status {};
    check(SUModelCreateFromFileWithStatus(&m_model, path.data(), &status));
    if (status == SUModelLoadStatus_Success_MoreRecent) {
        // TODO: 로그 찍는 것 외에 programmatically 알 수 있게 처리
        printf("WARN: SUModelLoadStatus_Success_MoreRecent\n");
    }

    // default tag
    check(SUModelGetDefaultLayer(m_model, &m_defaultLayerRef));

    // tag 전부 얻어오는 작업
    size_t layerCount;
    check(SUModelGetNumLayers(m_model, &layerCount));
    std::vector<SULayerRef> layers(layerCount);
    check(SUModelGetLayers(m_model, layerCount, layers.data(), &layerCount));
    for (const auto& layerRef: layers) {
        if (isValidLayer(layerRef)) {
            auto tagId = m_tagCount++;
            m_tagList.push_back(tagId);
            m_tagMap[tagId] = layerRef;
            m_tagInverse[layerRef] = tagId;
        }
    }

    // material 전부 얻어오는 작업
    size_t numMaterials {}, numMaterialsActual {};
    check(SUModelGetNumMaterials(m_model, &numMaterials));
    std::vector<SUMaterialRef> materials(numMaterials);
    check(SUModelGetMaterials(m_model, numMaterials, materials.data(), &numMaterialsActual));
    for (const auto& material: materials) {
        const auto materialId = m_materialCount++;
        m_materialMap[materialId] = material;
        m_materialInverse[material] = materialId;
    }

    for (const auto& [materialId, material]: m_materialMap) {
        SUMaterialType type {};
        bool useOpacity;
        double opacity;
        check(SUMaterialGetType(material, &type));
        bool materialHasTexture = type == SUMaterialType_Textured || type == SUMaterialType_ColorizedTexture;

        if (materialHasTexture) {
            SUTextureRef texture;
            check(SUMaterialGetTexture(material, &texture));
            size_t width {}, height {};
            double s_scale {}, t_scale {};
            check(SUTextureGetDimensions(texture, &width, &height, &s_scale, &t_scale));
            SketchupTextureMeta meta {
                .width = int(width),
                .height = int(height),
                .s_scale = float(s_scale),
                .t_scale = float(t_scale),
            };
            const auto textureId = m_textureCount++;
            m_textureMap[textureId] = texture;
            m_textureInverse[texture] = textureId;
            m_materialTextures[materialId] = textureId;
            m_textureMetaHolder->map[textureId] = meta;
        } else {
            SUColor color {};
            check(SUMaterialGetColor(material, &color));
            m_materialColors[materialId] = convertColor(color);
        }

        SUMaterialGetUseOpacity(material, &useOpacity);
        if (useOpacity) {
            SUMaterialGetOpacity(material, &opacity);
            m_materialOpacity[materialId] = opacity;
        }
    }

    SUEntitiesRef rootEntities {};
    check(SUModelGetEntities(m_model, &rootEntities));
    SUTransformation rootTransform {
        .values = {
                1.0, 0.0, 0.0, 0.0,
                0.0, 1.0, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                0.0, 0.0, 0.0, 1.0,
        },
    };
    m_dfsStack.emplace(SketchupObjectDescription {
        .entities = rootEntities,
        .parentObjectId = {},
        .name = "root",
        .transform = convertTransform(rootTransform),
        .inheritedMaterialOpt = {},
    });
    while (!m_dfsStack.empty()) {
        const auto desc = m_dfsStack.top();
        m_dfsStack.pop();

        const auto objectId = processObject(desc);

        // group
        size_t numGroups {}, numGroupsActual {};
        check(SUEntitiesGetNumGroups(desc.entities, &numGroups));
        std::vector<SUGroupRef> groups(numGroups);
        if (numGroups > 0) {
            check(SUEntitiesGetGroups(desc.entities, numGroups, groups.data(), &numGroupsActual));
        }

        for (const auto& group: groups) {
            auto instance = SUGroupToComponentInstance(group);
            pushChildren(objectId, instance, desc.inheritedMaterialOpt);
        }

        // component
        size_t numInstances {}, numInstancesActual {};
        check(SUEntitiesGetNumInstances(desc.entities, &numInstances));
        std::vector<SUComponentInstanceRef> instances(numInstances);
        if (numInstances > 0) {
            check(SUEntitiesGetInstances(desc.entities, numInstances, instances.data(), &numInstancesActual));
        }

        for (const auto& instance: instances) {
            pushChildren(objectId, instance, desc.inheritedMaterialOpt);
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

bool SketchupReader::getMaterialHasColor(MaterialId materialId) const {
    return m_materialColors.contains(materialId);
}

glm::vec4 SketchupReader::getMaterialColor(MaterialId id) const {
    return m_materialColors.at(id);
}

bool SketchupReader::getMaterialHasOpacity(MaterialId id) const {
    return m_materialOpacity.contains(id);
}

float SketchupReader::getMaterialOpacity(MaterialId id) const {
    return m_materialOpacity.at(id);
}

struct UnitElement {
    SUFaceRef face;
    bool isFrontMaterialInherited;
    bool isBackMaterialInherited;
};

ObjectId SketchupReader::processObject(const SketchupObjectDescription& desc) {
    std::vector<UnitId> units;

    SUTextureWriterRef textureWriter {};
    SUTextureWriterCreate(&textureWriter);

    // face 전부 얻어오는 작업
    size_t numFaces, numFacesActual;
    check(SUEntitiesGetNumFaces(desc.entities, &numFaces));
    std::vector<SUFaceRef> faces(numFaces);
    if (numFaces > 0) {
        check(SUEntitiesGetFaces(desc.entities, numFaces, faces.data(), &numFacesActual));
    }

    // face 를 material 별로 분류하는 작업
    using UnitKey = std::pair<std::optional<MaterialId>, std::optional<MaterialId>>;
    std::map<UnitKey, std::vector<UnitElement>> unitsByMaterial;
    for (const auto& face: faces) {
        double area;
        SUMaterialRef frontMaterial {}, backMaterial {};
        const auto frontResult = SUFaceGetFrontMaterial(face, &frontMaterial);
        const auto backResult = SUFaceGetBackMaterial(face, &backMaterial);
        bool hasFrontMaterial = SU_ERROR_NONE == frontResult;
        bool hasBackMaterial = SU_ERROR_NONE == backResult;
        UnitKey key {};
        UnitElement element {
            .face = face,
            .isFrontMaterialInherited = true,
            .isBackMaterialInherited = true,
        };
        if (desc.inheritedMaterialOpt) {
            key.first = key.second = desc.inheritedMaterialOpt.value();
        }
        if (hasFrontMaterial) {
            key.first = m_materialInverse.at(frontMaterial);
            element.isFrontMaterialInherited = false;
        }
        if (hasBackMaterial) {
            key.second = m_materialInverse.at(backMaterial);
            element.isBackMaterialInherited = false;
        }
        unitsByMaterial[key].push_back(element);
    }

    for (const auto& [pair, elements]: unitsByMaterial) {
        const auto& [frontMaterialId, backMaterialId] = pair;
        std::vector<Triangle> triangles {};

        float front_s_scale = 1.0f, front_t_scale = 1.0f, back_s_scale = 1.0f, back_t_scale = 1.0f;
        if (frontMaterialId.has_value() && getMaterialHasTexture(frontMaterialId.value())) {
            auto frontTextureId = m_materialTextures.at(frontMaterialId.value());
            const auto& frontTextureMeta = m_textureMetaHolder->map.at(frontTextureId);
            front_s_scale = frontTextureMeta.s_scale;
            front_t_scale = frontTextureMeta.t_scale;
        }
        if (backMaterialId.has_value() && getMaterialHasTexture(backMaterialId.value())) {
            auto backTextureId = m_materialTextures.at(backMaterialId.value());
            const auto& backTextureMeta = m_textureMetaHolder->map.at(backTextureId);
            back_s_scale = backTextureMeta.s_scale;
            back_t_scale = backTextureMeta.t_scale;
        }

        for (const auto& element: elements) {
            // tessellate
            SUMeshHelperRef helper {};
            check(SUMeshHelperCreate(&helper, element.face));

            SUVector3D faceNormal {};
            check(SUFaceGetNormal(element.face, &faceNormal));
            auto glmFaceNormal = glm::normalize(convertVector3D(faceNormal));

            size_t numTriangles;
            check(SUMeshHelperGetNumTriangles(helper, &numTriangles));

            size_t numIndices = numTriangles * 3, numIndicesActual;
            std::vector<size_t> indices(numIndices);
            if (numIndices > 0) {
                check(SUMeshHelperGetVertexIndices(helper, numIndices, indices.data(), &numIndicesActual));
            }

            size_t numVertices, numVerticesActual;
            check(SUMeshHelperGetNumVertices(helper, &numVertices));
            std::vector<SUPoint3D> vertices(numVertices);
            std::vector<SUVector3D> normals(numVertices);
            std::vector<SUUVQ> frontTexCoords(numVertices);
            std::vector<SUUVQ> backTexCoords(numVertices);
            if (numVertices > 0) {
                check(SUMeshHelperGetVertices(helper, numVertices, vertices.data(), &numVerticesActual));
                check(SUMeshHelperGetNormals(helper, numVertices, normals.data(), &numVerticesActual));
            }
            SUUVHelperRef uvHelper {};
            SUFaceGetUVHelper(element.face, true, true, textureWriter, &uvHelper);

            for (int i = 0; i < vertices.size(); i++) {
                SUUVHelperGetFrontUVQ(uvHelper, &vertices[i], &frontTexCoords[i]);
                SUUVHelperGetBackUVQ(uvHelper, &vertices[i], &backTexCoords[i]);
            }

            for (int i = 0; i < numTriangles; i++) {
                Triangle triangle {};

                for (int j = 0; j < 3; j++) {
                    size_t index = indices[3 * i + j];
                    // TODO: 계산이 vertex 마다 수 차례 중복 되는 문제
                    auto ftc = convertTexCoord(
                        frontTexCoords[index], front_s_scale, front_t_scale, element.isFrontMaterialInherited);
                    auto btc = convertTexCoord(
                        backTexCoords[index], back_s_scale, back_t_scale, element.isBackMaterialInherited);

                    triangle.vertices[j] = Vertex {
                        .position = convertPoint3D(vertices[index]),
                        .normal = convertVector3D(normals[index]),
                        .faceNormal = glmFaceNormal,
                        .frontTexCoord = ftc,
                        .backTexCoord = btc,
                    };
                    triangle.vertices[j].frontTexCoord = ftc;
                    triangle.vertices[j].backTexCoord = btc;
                }

                triangles.push_back(triangle);
            }
            check(SUMeshHelperRelease(&helper));
        }

        auto unitId = m_unitCount++;
        m_unitHolder->map.emplace(
            unitId,
            SketchupUnit {
                .frontMaterialId = frontMaterialId,
                .backMaterialId = backMaterialId,
                .isFrontMaterialInherited = false,
                .isBackMaterialInherited = false,
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
    if (desc.tagIdOpt) {
        const auto tagId = desc.tagIdOpt.value();
        m_tagObjects[tagId].push_back(objectId);
    }

    if (desc.parentObjectId) {
        m_objectHolder->map.at(desc.parentObjectId.value()).children.push_back(objectId);
    }
    return objectId;
}

bool SketchupReader::getMaterialHasTexture(MaterialId materialId) const {
    return m_materialTextures.contains(materialId);
}

TextureId SketchupReader::getMaterialTexture(MaterialId materialId) const {
    return m_materialTextures.at(materialId);
}

std::unique_ptr<TextureData> SketchupReader::copyTextureData(TextureId textureId) const {
    const auto& texture = m_textureMap.at(textureId);
    SUImageRepRef image {};
    size_t dataSize {}, bitsPerPixel {};
    size_t width {}, height {};
    std::vector<unsigned char> data;

    SUImageRepCreate(&image);
    SUTextureGetColorizedImageRep(texture, &image);
    // TODO: GPU 활용 이미지 디코딩?
    check(SUImageRepConvertTo32BitsPerPixel(image));
    SUImageRepGetDataSize(image, &dataSize, &bitsPerPixel);
    check(SUImageRepGetPixelDimensions(image, &width, &height));
    data = std::vector<unsigned char>(dataSize);
    SUImageRepGetData(image, dataSize, data.data());
    SUImageRepRelease(&image);

    // flip image vertically
    flipImageVertically(data, int(width), int(height), 4); // 4 = 4 bytes = 32 bits

    return std::make_unique<SketchupTextureData>(width, height, dataSize, bitsPerPixel, std::move(data));
}

int SketchupReader::getTextureWidth(TextureId textureId) const {
    return m_textureMetaHolder->map.at(textureId).width;
}

int SketchupReader::getTextureHeight(TextureId textureId) const {
    return m_textureMetaHolder->map.at(textureId).height;
}

unsigned SketchupReader::getTagCount() const {
    return m_tagMap.size();
}

TagId SketchupReader::getTag(int index) const {
    return m_tagList[index];
}

std::string SketchupReader::getTagName(TagId id) const {
    const auto layerRef = m_tagMap.at(id);
    SUStringRef nameRef {};
    SUStringCreate(&nameRef);
    check(SULayerGetName(layerRef, &nameRef));
    return convertAndReleaseString(nameRef);
}

unsigned SketchupReader::getTagObjectCount(TagId id) const {
    if (m_tagObjects.contains(id)) {
        return m_tagObjects.at(id).size();
    } else {
        return 0;
    }
}

ObjectId SketchupReader::getTagObject(TagId id, int index) const {
    return m_tagObjects.at(id)[index];
}

bool SketchupReader::isValidLayer(SULayerRef layerRef) const {
    // 스케치업의 default layer (Layer0) 은 '레이어 없음' 의미에 가까움.
    // 따라서, default layer 를 가지고 있는 경우 아예 처리하지 않는 방식을 채택.
    return layerRef.ptr != m_defaultLayerRef.ptr;
}

void SketchupReader::pushChildren(
    ObjectId id, SUComponentInstanceRef instance, std::optional<MaterialId> inheritedMaterialOpt) {
    const auto drawingRef = SUComponentInstanceToDrawingElement(instance);

    std::optional<TagId> tagIdOpt {};
    SULayerRef layerRef {};
    check(SUDrawingElementGetLayer(drawingRef, &layerRef));
    if (isValidLayer(layerRef)) {
        tagIdOpt = m_tagInverse.at(layerRef);
    }

    // TODO: reuse definition - Mesh 단위 추가
    SUComponentDefinitionRef definition {};
    SUEntitiesRef definitionEntities {};
    check(SUComponentInstanceGetDefinition(instance, &definition));
    check(SUComponentDefinitionGetEntities(definition, &definitionEntities));

    SUStringRef nameRef {};
    SUTransformation transform {};
    SUComponentInstanceGetName(instance, &nameRef);
    SUComponentInstanceGetTransform(instance, &transform);

    const auto name = convertAndReleaseString(nameRef);

    auto el = SUComponentInstanceToDrawingElement(instance);
    SUMaterialRef groupMaterial {};
    auto result = SUDrawingElementGetMaterial(el, &groupMaterial);
    if (result == SU_ERROR_NONE) {
        inheritedMaterialOpt = m_materialInverse.at(groupMaterial);
    }

    m_dfsStack.emplace(SketchupObjectDescription {
        .entities = definitionEntities,
        .parentObjectId = id,
        .name = name,
        .transform = convertTransform(transform),
        .inheritedMaterialOpt = inheritedMaterialOpt,
        .tagIdOpt = tagIdOpt,
    });
}

static float m(double length) {
    // inch -> meter
    return 0.0254f * float(length);
}

static glm::vec3 convertPoint3D(SUPoint3D point) {
    return glm::vec3(m(point.x), m(point.y), m(point.z));
}

static glm::vec2 convertTexCoord(SUUVQ uvq, float s_scale, float t_scale, bool isMaterialInherited) {
    auto q = uvq.q;
    if (q == 0.0) {
        q = 1.0;
    }
    float u = uvq.u / q;
    float v = uvq.v / q;
    // https://forums.sketchup.com/t/how-to-get-a-textures-size/6139/10
    if (isMaterialInherited) {
        u *= s_scale;
        v *= t_scale;
    }
    return {u, v};
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

static glm::vec4 convertColor(SUColor color) {
    glm::vec4 raw = {float(color.red), float(color.green), float(color.blue), float(color.alpha)};
    return raw / 255.0f;
}

static void flipImageVertically(std::vector<unsigned char>& data, int width, int height, int channels) {
    int rowRange = channels * width;
    for (int row = 0; row < height / 2; row++) {
        int upperStart = rowRange * row;
        int lowerStart = rowRange * (height - 1 - row);
        std::swap_ranges(data.begin() + upperStart, data.begin() + upperStart + rowRange, data.begin() + lowerStart);
    }
}

std::string convertAndReleaseString(SUStringRef stringRef) {
    size_t len {};
    SUStringGetUTF8Length(stringRef, &len);
    std::string result(len, '0');
    SUStringGetUTF8(stringRef, len, result.data(), &len);
    SUStringRelease(&stringRef);
    return result;
}

}
