#pragma once

#include "r_holders.h"
#include "r_ids.h"
#include "scene_node.h"

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Color.hpp>

#include <cstdint>

// text node is a derivative of scene node
class TextNode : public SceneNode {
public:
    explicit TextNode(const FontHolder& fonts, const std::string& text);
    void set_string(const std::string& text);
    void set_fill_color(std::uint8_t r, std::uint8_t g, std::uint8_t b, 
                        std::uint8_t alpha = 255);
    void set_fill_color(const sf::Color& color);
    void set_outline_color(const sf::Color& color);
    void set_outline_thickness(float thickness);
    void set_character_size(unsigned int size);
    void set_style(std::uint32_t style);
private:
    virtual void draw_current(sf::RenderTarget& target, sf::RenderStates states)
        const;
    sf::Text m_text;
};
