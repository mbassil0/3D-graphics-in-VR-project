#ifndef OBJECT_H
#define OBJECT_H

#include<iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

struct Vertex {
    glm::vec3 Position;
    glm::vec2 Texture;
    glm::vec3 Normal;
};

struct MeshChunk {
    std::string material;
    std::vector<Vertex> vertices;

    GLuint VAO, VBO;
};

class Object
{
public:
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> textures;
    std::vector<glm::vec3> normals;

    std::vector<MeshChunk> chunks;

    glm::mat4 model = glm::mat4(1.0);

    Object() {}

    Object(const char* path) {

        std::ifstream infile(path);
        std::string line;

        std::string currentMaterial = "default";

        // start with one chunk
        chunks.push_back(MeshChunk());
        chunks.back().material = currentMaterial;

        while (std::getline(infile, line))
        {
            std::istringstream iss(line);
            std::string indice;
            iss >> indice;

            if (indice == "v") {
                float x, y, z;
                iss >> x >> y >> z;
                positions.push_back(glm::vec3(x, y, z));
            }
            else if (indice == "vn") {
                float x, y, z;
                iss >> x >> y >> z;
                normals.push_back(glm::vec3(x, y, z));
            }
            else if (indice == "vt") {
                float u, v;
                iss >> u >> v;
                textures.push_back(glm::vec2(u, v));
            }
            else if (indice == "usemtl") {
                iss >> currentMaterial;

                std::cout << "Material switch: " << currentMaterial << std::endl;

                // create new chunk
                chunks.push_back(MeshChunk());
                chunks.back().material = currentMaterial;
            }
            else if (indice == "f") {

                std::string f1, f2, f3;
                iss >> f1 >> f2 >> f3;

                auto parseVertex = [&](std::string f) {
                    std::string p, t, n;

                    p = f.substr(0, f.find("/"));
                    f.erase(0, f.find("/") + 1);

                    t = f.substr(0, f.find("/"));
                    f.erase(0, f.find("/") + 1);

                    n = f.substr(0, f.find("/"));

                    Vertex v;
                    v.Position = positions.at(std::stoi(p) - 1);
                    v.Texture = textures.at(std::stoi(t) - 1);
                    v.Normal = normals.at(std::stoi(n) - 1);

                    return v;
                };

                chunks.back().vertices.push_back(parseVertex(f1));
                chunks.back().vertices.push_back(parseVertex(f2));
                chunks.back().vertices.push_back(parseVertex(f3));
            }
        }

        infile.close();

        std::cout << "Loaded model with " << chunks.size() << " material chunks\n";
    }

    void makeObject(Shader shader) {

        for (auto& chunk : chunks) {
            std::cout << chunk.material << std::endl;

            int numVertices = chunk.vertices.size();

            float* data = new float[8 * numVertices];

            for (int i = 0; i < numVertices; i++) {
                Vertex v = chunk.vertices[i];

                data[i * 8 + 0] = v.Position.x;
                data[i * 8 + 1] = v.Position.y;
                data[i * 8 + 2] = v.Position.z;

                data[i * 8 + 3] = v.Texture.x;
                data[i * 8 + 4] = v.Texture.y;

                data[i * 8 + 5] = v.Normal.x;
                data[i * 8 + 6] = v.Normal.y;
                data[i * 8 + 7] = v.Normal.z;
            }

            glGenVertexArrays(1, &chunk.VAO);
            glGenBuffers(1, &chunk.VBO);

            glBindVertexArray(chunk.VAO);
            glBindBuffer(GL_ARRAY_BUFFER, chunk.VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * numVertices, data, GL_STATIC_DRAW);

            auto att_pos = glGetAttribLocation(shader.ID, "position");
            glEnableVertexAttribArray(att_pos);
            glVertexAttribPointer(att_pos, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)0);

            auto att_tex = glGetAttribLocation(shader.ID, "tex_coord");
            if (att_tex != -1) {
                glEnableVertexAttribArray(att_tex);
                glVertexAttribPointer(att_tex, 2, GL_FLOAT, false, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            }

            auto att_norm = glGetAttribLocation(shader.ID, "normal");
            if (att_norm != -1) {
                glEnableVertexAttribArray(att_norm);
                glVertexAttribPointer(att_norm, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)(5 * sizeof(float)));
            }


            auto att_color = glGetAttribLocation(shader.ID, "color");
            if (att_color != -1) {
                glEnableVertexAttribArray(att_color);
                glVertexAttribPointer(att_color, 3, GL_FLOAT, false, 11 * sizeof(float), (void*)(8 * sizeof(float)));
            }


            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
            

            //auto u_color = glGetUniformLocation(program, "color"); 

            delete[] data;
        }
    }

    void draw(Shader shader) {

        for (auto& chunk : chunks) {

            // DEBUG: print material being used


            //std::cout << "\r Drawing material: " << chunk.material << std::endl;
            for (int i = 0; i < chunk.vertices.size(); i++) {
               
                    //std::cout << chunk.material << " " << chunk.vertices[i].Position.x << "  " << chunk.vertices[i].Position.y << "  " << chunk.vertices[i].Position.z <<" " << chunk.vertices[i].Texture.x << " " << chunk.vertices[i].Texture.y << std::endl;
            }
            
            
            if (chunk.material == "Material.001")
                glUniform3f(glGetUniformLocation(shader.ID, "color"),
                    1.0f, 0.0f, 0.0f);
            else if (chunk.material == "Material.002")
                glUniform3f(glGetUniformLocation(shader.ID, "color"),
                    0.0f, 1.0f, 0.0f);
            
            glBindVertexArray(chunk.VAO);
            glDrawArrays(GL_TRIANGLES, 0, chunk.vertices.size());
        }
    }
};

#endif