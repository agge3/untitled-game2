#include "text_node.h"
#include "utility.h"

#include <SFML/Graphics/RenderTarget.hpp>

/**
 * Default constructor sets font of TextNode to parameter, sets font size, and
 * sets text displayed to std::string parameter.
 * */
TextNode::TextNode(const FontHolder& fonts, const std::string& text)
{
    m_text.setFont(fonts.get(Fonts::Main));
    m_text.setCharacterSize(14);
    set_string(text);
}

void TextNode::draw_current(sf::RenderTarget& target, sf::RenderStates states)
    const
{
    target.draw(m_text, states);
}

/**
 * Separate utility fn to both setString() and center_origin() in one fn call.
 * @param const std::string& text
 * The std::string to be set as the text for the TextNode.
 */
void TextNode::set_string(const std::string& text)
{
    m_text.setString(text);
    center_origin(m_text);
}

/**
* Set fill color of underlying sf::Text.
* @param std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t alpha 
*/
void TextNode::set_fill_color(std::uint8_t r, std::uint8_t g, std::uint8_t b, 
                              std::uint8_t alpha) 
{
    sf::Color color(r, g, b, alpha);
    m_text.setFillColor(color);
}

/** 
 * Set fill color of underlying sf::Text.
 * @param const sf::Color& color
*/
void TextNode::set_fill_color(const sf::Color& color)
{
    m_text.setFillColor(color);
}

/** 
 * Set outline color of underlying sf::Text.
 * @param const sf::Color& color
*/
void TextNode::set_outline_color(const sf::Color& color)
{
    m_text.setOutlineColor(color);
}

/**
 * Set outline thickness of underlying sf::Text.
 * float thickness
*/
void TextNode::set_outline_thickness(float thickness)
{
    m_text.setOutlineThickness(thickness);
}

/**
* Set character size of underlying sf::Text.
*/
void TextNode::set_character_size(unsigned int size)
{
    m_text.setCharacterSize(size);
}

/**
* Set style of underlying sf::Text (regular, bold, italic, etc.).
* @see SFML documentation, style is bitwise shift left (e.g., "Bold = 1 << 0").
*/
void TextNode::set_style(std::uint32_t style)
{
    m_text.setStyle(style);
}
