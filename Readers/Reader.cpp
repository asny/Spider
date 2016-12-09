//
//  OBJLoader.cpp
//  Spider
//
//  Created by Asger Nyman Christiansen on 05/11/15.
//  Copyright © 2015 Asger Nyman Christiansen. All rights reserved.
//

#include "Reader.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

//uses stb_image to try load files
#define STBI_FAILURE_USERMSG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace tdogl;
using namespace geogo;
using namespace glm;
using namespace std;

static bool load_obj(string file_path, vector<vec3>& positions, vector<unsigned int>& position_indices,
                     vector<vec2>& uv_coordinates, vector<unsigned int>& uv_coordinate_indices,
                     vector<vec3>& normals, vector<unsigned int>& normal_indices)
{
    string line;
    ifstream myfile(file_path);
    if (!myfile.is_open())
    {
        return false;
    }
    
    while ( getline(myfile, line) )
    {
        stringstream stream(line);
        string header;
        stream >> header;
        if ( header.compare("v") == 0 )
        {
            vec3 position;
            string value;
            for (int i = 0; i < 3; i++)
            {
                stream >> value;
                position[i] = stod(value);
            }
            positions.push_back(position);
        }
        else if ( header.compare("vt") == 0 )
        {
            vec2 uv_coordinate;
            string value;
            for (int i = 0; i < 2; i++)
            {
                stream >> value;
                uv_coordinate[i] = stod(value);
            }
            uv_coordinates.push_back(uv_coordinate);
        }
        else if ( header.compare("vn") == 0 )
        {
            vec3 normal;
            string value;
            for (int i = 0; i < 3; i++)
            {
                stream >> value;
                normal[i] = stod(value);
            }
            normals.push_back(normal);
        }
        else if ( header.compare("f") == 0 )
        {
            for(int i = 0; i < 3; i++)
            {
                string indices;
                stream >> indices;
                stringstream stream2(indices);
                string value;
                int j = 0;
                while (getline(stream2, value, '/'))
                {
                    if(value.length() != 0)
                    {
                        if(j % 3 == 0)
                        {
                            position_indices.push_back(stoi(value));
                        }
                        else if(j % 3 == 1)
                        {
                            uv_coordinate_indices.push_back(stoi(value));
                        }
                        else if(j % 3 == 2)
                        {
                            normal_indices.push_back(stoi(value));
                        }
                    }
                    j++;
                }
            }
        }
    }
    myfile.close();
    
    return true;
}

void Reader::load_obj(string file_path, Mesh& geometry, Attribute<VertexID, vec2>& uv_attribute, Attribute<VertexID, vec3>& normal_attribute)
{
    vector<vec3> positions, normals;
    vector<vec2> uv_coordinates;
    vector<unsigned int> position_indices, uv_coordinates_indices, normal_indices;
    
    bool load_success = ::load_obj(file_path, positions, position_indices, uv_coordinates, uv_coordinates_indices, normals, normal_indices);
    if(load_success)
    {
        // Create the vertices
        auto mapping = std::map<unsigned int, VertexID*>();
        for( unsigned int i = 0; i < positions.size(); i++ )
        {
            auto vertex_id = geometry.create_vertex(positions.at(i));
            mapping[i] = vertex_id;
        }
        
        // Create the faces
        for (unsigned int i = 0; i < position_indices.size(); i += 3)
        {
            auto vertex_id1 = mapping[position_indices[i] - 1];
            auto vertex_id2 = mapping[position_indices[i+1] - 1];
            auto vertex_id3 = mapping[position_indices[i+2] - 1];
            geometry.create_face(vertex_id1, vertex_id2, vertex_id3);
        }
        
        // Add the uv coordinate attribute
        if(uv_coordinates.size() > 0 && uv_coordinates_indices.size() > 0)
        {
            for( unsigned int i = 0; i < normals.size(); i++ )
            {
                // TODO: For now, we only support per vertex attributes
                uv_attribute.at(mapping[i]) = uv_coordinates.at(i);
            }
        }
        
        // Add the normal attribute
        if(normals.size() > 0 && normal_indices.size() > 0)
        {
            for( unsigned int i = 0; i < normals.size(); i++ )
            {
                // TODO: For now, we only support per vertex attributes
                normal_attribute.at(mapping[i]) = normals.at(i);
            }
        }
    }
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
