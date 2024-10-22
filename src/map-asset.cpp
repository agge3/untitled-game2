#include "map-asset.h"
#include "data_tables.h"
#include "category.h"
#include "command_queue.h"
#include "utility.h"
#include "r_holders.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <limits>

/// Local TABLE in anonymous namespace to prevent naming conflicts amongst entities.
namespace {
    const std::vector<MapAssetData> TABLE = initialize_map_asset_data();
}

MapAsset::MapAsset(Type type, const TextureHolder& textures) :
    Entity(std::numeric_limits<float>::max()),
    m_type(type),
    m_sprite(textures.get(TABLE[type].texture))
{
    /// Default constructor centers origin of sprite.
    center_origin(m_sprite);
}

unsigned int MapAsset::get_category() const
{
    return Category::MapAsset;
}

/**
 * Gets the bounding rectangle of the pickup.
 * @return sf::FloatRect that is the bounding rectangle.
 */
sf::FloatRect MapAsset::get_bounding_rect() const
{
    // bounds of the sprite, respecting its transform
    return get_world_transform().transformRect(m_sprite.getGlobalBounds());
}

void MapAsset::draw_current(sf::RenderTarget& target, sf::RenderStates states)
    const
{
    target.draw(m_sprite, states);
}
