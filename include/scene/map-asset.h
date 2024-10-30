#pragma once

#include "entity.h"
#include "command.h"
#include "r_ids.h"
#include "r_holders.h"

#include <SFML/Graphics/Sprite.hpp>

class MapAsset : public Entity {
public:
    /**
     * @enum Type
     * Type of pickup entity.
     */
    enum Type : unsigned int {
        Building,
        TypeCount,
    };

    MapAsset(Type type, const TextureHolder& textures);
    virtual unsigned int get_category() const;
    virtual sf::FloatRect get_bounding_rect() const;
protected:
/// draw_current() protected so derived classes can inherit, but still behaves private.
    virtual void draw_current(sf::RenderTarget& target, sf::RenderStates state)
        const;
private:
    Type m_type;
    sf::Sprite m_sprite;
};
