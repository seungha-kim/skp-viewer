#include "MonkeyRenderer.h"
#include "Shader.h"

MonkeyRenderer::MonkeyRenderer()
        : ourShader(Shader("monkey.vert", "monkey.frag")) {

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile("resources/monkey.obj",
                                             aiProcess_CalcTangentSpace |
                                             aiProcess_Triangulate |
                                             aiProcess_JoinIdenticalVertices |
                                             aiProcess_SortByPType);


    for (int i = 0; i < scene->mNumMeshes; i++) {
        // TODO: transform stack
        m_meshes.emplace_back(*scene->mMeshes[i], scene->mRootNode->mTransformation);
    }
}

void MonkeyRenderer::render(RenderContext &ctx) {
    const CameraState &cam = ctx.cam;

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ourShader.use();
    ourShader.setMatrix4f("view", cam.viewMatrix());
    ourShader.setMatrix4f("projection", cam.projectionMatrix());
    ourShader.setVector3f("cameraPos", cam.pos);
    ourShader.setVector3f("lightPos", glm::vec3(50.0f, 50.0f, 50.0f));

    for (auto &mesh: m_meshes) {
        glBindVertexArray(mesh.VAO());

        glm::mat4 model = glm::mat4(1.0f);
        ourShader.setMatrix4f("model", model);

        glDrawArrays(GL_TRIANGLES, 0, mesh.verticesCount());
    }
}
