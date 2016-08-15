//
//  OBJLoader.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 05/11/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "Reader.hpp"

//uses stb_image to try load files
#define STBI_FAILURE_USERMSG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace tdogl;
using namespace geogo;
using namespace glm;
using namespace std;

bool Reader::load_obj(std::string filePath, std::vector<glm::vec3>& out_vertices, std::vector<glm::vec2>& out_uvs, std::vector<glm::vec3> & out_normals)
{
    std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
    std::vector< glm::vec3 > temp_vertices;
    std::vector< glm::vec2 > temp_uvs;
    std::vector< glm::vec3 > temp_normals;
    
    FILE * file = fopen(filePath.c_str(), "r");
    if( file == NULL )
    {
        printf("Impossible to open the file !\n");
        return false;
    }
    while( 1 )
    {
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.
        
        // else : parse lineHeader
        if ( strcmp( lineHeader, "v" ) == 0 )
        {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            temp_vertices.push_back(vertex);
        }
        else if ( strcmp( lineHeader, "vt" ) == 0 )
        {
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y );
            temp_uvs.push_back(uv);
        }
        else if ( strcmp( lineHeader, "vn" ) == 0 )
        {
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
            temp_normals.push_back(normal);
        }
        else if ( strcmp( lineHeader, "f" ) == 0 )
        {
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
            if (matches != 9){
                printf("File can't be read by our simple parser : ( Try exporting with other options\n");
                return false;
            }
            
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices    .push_back(uvIndex[0]);
            uvIndices    .push_back(uvIndex[1]);
            uvIndices    .push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }
    }
    
    // For each vertex of each triangle
    for( unsigned int i = 0; i < vertexIndices.size(); i++ )
    {
        unsigned int vertexIndex = vertexIndices[i];
        glm::vec3 vertex = temp_vertices[ vertexIndex - 1 ];
        out_vertices.push_back(vertex);
    }
    
    // For each uv of each triangle
    for( unsigned int i = 0; i < uvIndices.size(); i++ )
    {
        unsigned int uvIndex = uvIndices[i];
        glm::vec2 uv = temp_uvs[ uvIndex - 1 ];
        out_uvs.push_back(uv);
    }
    
    // For each normal of each triangle
    for( unsigned int i = 0; i < normalIndices.size(); i++ )
    {
        unsigned int normalIndex = normalIndices[i];
        glm::vec3 normal = temp_normals[ normalIndex - 1 ];
        out_normals.push_back(normal);
    }
    return true;
}

bool Reader::load_obj(std::string filePath, std::vector<glm::vec3>& out_vertices, std::vector<glm::vec2>& out_uvs)
{
    std::vector< unsigned int > vertexIndices, uvIndices;
    std::vector< glm::vec3 > temp_vertices;
    std::vector< glm::vec2 > temp_uvs;
    
    FILE * file = fopen(filePath.c_str(), "r");
    if( file == NULL )
    {
        printf("Impossible to open the file !\n");
        return false;
    }
    while( 1 )
    {
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.
        
        // else : parse lineHeader
        if ( strcmp( lineHeader, "v" ) == 0 )
        {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            temp_vertices.push_back(vertex);
        }
        else if ( strcmp( lineHeader, "vt" ) == 0 )
        {
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y );
            temp_uvs.push_back(uv);
        }
        else if ( strcmp( lineHeader, "f" ) == 0 )
        {
            unsigned int vertexIndex[3], uvIndex[3];
            int matches = fscanf(file, "%d/%d %d/%d %d/%d\n", &vertexIndex[0], &uvIndex[0], &vertexIndex[1], &uvIndex[1], &vertexIndex[2], &uvIndex[2]);
            if (matches == 6)
            {
                vertexIndices.push_back(vertexIndex[0]);
                vertexIndices.push_back(vertexIndex[1]);
                vertexIndices.push_back(vertexIndex[2]);
                uvIndices    .push_back(uvIndex[0]);
                uvIndices    .push_back(uvIndex[1]);
                uvIndices    .push_back(uvIndex[2]);
            }
        }
    }
    
    // For each vertex of each triangle
    for( unsigned int i = 0; i < vertexIndices.size(); i++ )
    {
        unsigned int vertexIndex = vertexIndices[i];
        glm::vec3 vertex = temp_vertices[ vertexIndex - 1 ];
        out_vertices.push_back(vertex);
    }
    
    // For each uv of each triangle
    for( unsigned int i = 0; i < uvIndices.size(); i++ )
    {
        unsigned int uvIndex = uvIndices[i];
        glm::vec2 uv = temp_uvs[ uvIndex - 1 ];
        out_uvs.push_back(uv);
    }
    
    return true;
}


bool Reader::load_obj(std::string filePath, std::vector<glm::vec3>& out_vertices, std::vector<glm::vec3> & out_normals)
{
    std::vector< unsigned int > vertexIndices, normalIndices;
    std::vector< glm::vec3 > temp_vertices, temp_normals;
    
    bool success = load_obj(filePath, temp_vertices, vertexIndices, temp_normals, normalIndices);
    if(!success)
        return success;
    
    // For each vertex of each triangle
    for( unsigned int i = 0; i < vertexIndices.size(); i++ )
    {
        unsigned int vertexIndex = vertexIndices[i];
        glm::vec3 vertex = temp_vertices[ vertexIndex - 1 ];
        out_vertices.push_back(vertex);
    }
    
    // For each normal of each triangle
    for( unsigned int i = 0; i < normalIndices.size(); i++ )
    {
        unsigned int normalIndex = normalIndices[i];
        glm::vec3 normal = temp_normals[ normalIndex - 1 ];
        out_normals.push_back(normal);
    }
    return true;
}

bool Reader::load_obj(std::string filePath, std::vector<glm::vec3>& vertices, std::vector<unsigned int>& vertex_indices,
                      std::vector<glm::vec3>& normals, std::vector<unsigned int>& normal_indices)
{
    FILE * file = fopen(filePath.c_str(), "r");
    if( file == NULL )
    {
        printf("Impossible to open the file !\n");
        return false;
    }
    while( 1 )
    {
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.
        
        // else : parse lineHeader
        if ( strcmp( lineHeader, "v" ) == 0 )
        {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            vertices.push_back(vertex);
        }
        else if ( strcmp( lineHeader, "vn" ) == 0 )
        {
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
            normals.push_back(normal);
        }
        else if ( strcmp( lineHeader, "f" ) == 0 )
        {
            unsigned int vertexIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2] );
            if (matches != 6){
                printf("File can't be read by our simple parser : ( Try exporting with other options\n");
                return false;
            }
            
            vertex_indices.push_back(vertexIndex[0]);
            vertex_indices.push_back(vertexIndex[1]);
            vertex_indices.push_back(vertexIndex[2]);
            normal_indices.push_back(normalIndex[0]);
            normal_indices.push_back(normalIndex[1]);
            normal_indices.push_back(normalIndex[2]);
        }
    }
    return true;
}

shared_ptr<Geometry> Reader::load_obj(string file_path)
{
    auto geometry = shared_ptr<Geometry>(new Geometry());
    
    vector<vec3> positions, normals;
    vector<unsigned int> position_indices, normal_indices;
    
    bool load_success = Reader::load_obj(file_path, positions, position_indices, normals, normal_indices);
    if(load_success)
    {
        auto position_attribute = geometry->get_vec3_vertex_attribute("position");
        auto normal_attribute = geometry->get_vec3_vertex_attribute("normal");
        
        // Create the vertices
        auto mapping = std::map<unsigned int, VertexID*>();
        for( unsigned int i = 0; i < positions.size(); i++ )
        {
            auto vertex_id = geometry->create_vertex();
            mapping[i] = vertex_id;
            
            glm::vec3 position = positions[ i ];
            position_attribute->add(*vertex_id, position);
            
            glm::vec3 normal = normals[ i ];
            normal_attribute->add(*vertex_id, normal);
        }
        
        // Create the faces
        for (unsigned int i = 0; i < position_indices.size(); i += 3)
        {
            auto vertex_id1 = mapping[position_indices[i] - 1];
            auto vertex_id2 = mapping[position_indices[i+1] - 1];
            auto vertex_id3 = mapping[position_indices[i+2] - 1];
            geometry->create_face(vertex_id1, vertex_id2, vertex_id3);
        }
    }
    return geometry;
}


Bitmap Reader::load_bitmap(std::string filePath)
{
    int width, height, channels;
    unsigned char* pixels = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
    if(!pixels) throw std::runtime_error(stbi_failure_reason());
    
    Bitmap bmp(width, height, (Bitmap::Format)channels, pixels);
    stbi_image_free(pixels);
    return bmp;
}

char* Reader::read_shader_source(std::string filePath)
{
    FILE *filePointer;
    char *content = NULL;
    
    int count=0;
    
    if (filePath != "") {
        filePointer = fopen(filePath.c_str(),"rt");
        
        if (filePointer != NULL) {
            
            fseek(filePointer, 0, SEEK_END);
            count = static_cast<int>(ftell(filePointer));
            rewind(filePointer);
            
            if (count > 0) {
                content = (char *)malloc(sizeof(char) * (count+1));
                count = static_cast<int>(fread(content,sizeof(char),count,filePointer));
                content[count] = '\0';
            }
            fclose(filePointer);
        }
    }
    return content;
}
