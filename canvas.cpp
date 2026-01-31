#include <glad/glad.h>
#include "entity.h"
#include "window.h"
#include "scene.h"
#include "interface.h"
#include <imgui.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <filesystem>

using namespace entity::canvas;
using namespace window;
using namespace scene;
using namespace interface;

windowsHandler windsHemndls;

namespace entity
{
    namespace canvas
    {
        item canvasItems;
	}
}

std::unordered_map<std::string, item::position> item::item_position;
std::unordered_map<std::string, item::itemData> item::item_itemData;
std::unordered_map<std::string, item::Shader> item::item_shader;
std::unordered_map<std::string, item::container> item::item_container;
std::unordered_map<std::string, item::Text> item::item_text;
std::map<char, item::Character> item::characters;
GLuint item::textVAO, item::textVBO;

void item::LoadFont(const char* Font_Path)
{
	if (!std::filesystem::exists(Font_Path))
        Font_Path = "fonts/ARIAL.ttf";

    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cout << "ERROR: Could not init FreeType" << std::endl;
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, Font_Path, 0, &face)) {
        std::cout << "ERROR: Failed to load font" << std::endl;
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR: Failed to load Glyph" << std::endl;
            continue;
        }

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        item::Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            (GLuint)face->glyph->advance.x
        };

        item::characters.insert(std::pair<char, item::Character>(c, character));
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenVertexArrays(1, &item::textVAO);
    glGenBuffers(1, &item::textVBO);
    glBindVertexArray(item::textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, item::textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void item::addItem(const char* UUID, const char* name, type itemType, const char* parentID)
{
    if (itemType == type::TEXT)
    {
		components::entity_data[UUID] = components::Data{ name, parentID, UUID, true, components::type::OTHER };
        item::item_itemData[UUID] = { UUID, true };
        item::item_text[UUID] = { "Hello World", 1.0f, {1,1,1}, "" };
        item::item_position[UUID] = { 100.0f, 100.0f };
		item::item_shader[UUID] = { gameobjects.loadShader("textVert.vs", "textFrag.fs", UUID), false};
		item::LoadFont("ARIAL.ttf");
    }
    if (itemType == type::HOLDER)
    {
        components::entity_data[UUID] = components::Data{ name, "", UUID, true, components::type::OTHER };
		item::item_container[UUID] = {UUID};
    }
    scenesHandler.inputSceneObject(scenesHandler.currentSceneName().c_str(), UUID);
}

void item::draw()
{
	for (const auto& obj : item::item_itemData)
    {
        auto& textData = item::item_text[obj.second.UUID];
        auto& shaderComp = item::item_shader[obj.second.UUID];
        auto& transform = item::item_position[obj.second.UUID];

        if (!obj.second.show_ || !scenesHandler.canObjectShown(obj.first.c_str()) || componentss.entity_data[obj.first].Parent == "")
            continue;

        glUseProgram(shaderComp.ShaderProgram);

        glm::mat4 projection = glm::ortho(0.0f, (float)windsHemndls.width,
            0.0f, (float)windsHemndls.height);


        glUniformMatrix4fv(glGetUniformLocation(shaderComp.ShaderProgram, "projection"),
            1, GL_FALSE, glm::value_ptr(projection));

        glUniform3f(glGetUniformLocation(shaderComp.ShaderProgram, "textColor"),
            textData.color.x, textData.color.y, textData.color.z);

        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(item::textVAO);

        float x = transform.x;
        float y = transform.y;
        float scale = textData.scale;

        std::string text = textData.content;

        for (std::string::const_iterator c = text.begin(); c != text.end(); c++)
        {
            if (item::characters.count(*c) == 0) {
                std::cout << "WARNING: Character not found: " << *c << std::endl;
                continue;
            }

            item::Character ch = item::characters[*c];

            float xpos = x + ch.Bearing.x * scale;
            float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;

            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };

            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            glBindBuffer(GL_ARRAY_BUFFER, item::textVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            x += (ch.Advance >> 6) * scale;
        }

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}