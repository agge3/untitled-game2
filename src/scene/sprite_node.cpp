#include "sprite_node.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <cmath>

SpriteNode::SpriteNode(const sf::Texture& texture) :
    m_sprite(texture)
{}

SpriteNode::SpriteNode(const sf::Texture& texture,
        const sf::IntRect& texture_rect) :
    m_sprite(texture, texture_rect)
{}

void SpriteNode::draw_current(sf::RenderTarget& target, sf::RenderStates states)
    const
{
    target.draw(m_sprite, states);
}

void SpriteNode::center_origin()
{
	sf::FloatRect bounds = m_sprite.getGlobalBounds();
	m_sprite.setOrigin(std::floor(bounds.left + bounds.width / 2.f),
            std::floor(bounds.top + bounds.height / 2.f));
}
