#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 shadowMatrices[6];

out vec4 FragPos; // World-space position to fragment shader

void main()
{
    // Emit triangle to all 6 cubemap faces
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face; // Selects cubemap face
        
        for(int i = 0; i < 3; ++i) // For each triangle vertex
        {
            FragPos = gl_in[i].gl_Position; // World position from vertex shader
            gl_Position = shadowMatrices[face] * FragPos; // Project to this face
            EmitVertex();
        }    
        EndPrimitive();
    }
}