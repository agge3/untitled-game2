//#define SFML_STATIC

#include "creature.h"
#include "data_tables.h"
#include "command_queue.h"
#include "utility.h"
#include "pickup.h"
#include "r_holders.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Color.hpp>

#include <cmath>
#include <iostream>
#include <string>
#include <stdexcept>
#include <ostream>
#include <iomanip>
#include <cstdint>

/// Anonymous namespace to avoid name collisions in other files - store Creature
/// data TABLE local to Creature.
namespace {
    const std::vector<CreatureData> TABLE = initialize_creature_data();
}

Creature::Creature(Type type, const TextureHolder& textures,
        const FontHolder& fonts) :
    Entity(TABLE[type].hitpoints),
    m_type(type),
    m_sprite(textures.get(TABLE[type].texture)),
    m_attack_command(),
    m_attack_countdown(sf::Time::Zero),
    m_is_attacking(false),
    m_is_marked_for_removal(false),
    m_drop_pickup_command(),
    m_travelled_distance(0.f),
    m_direction_index(0),
    m_health_display(nullptr)
{
    center_origin(m_sprite);

    /// Attack category and action is initialized in default constructor for
    /// direct access to &texture, ready for future use.
    m_attack_command.category = Category::SceneGroundLayer;
    // capture command & &textures -> create projectile for action
    m_attack_command.action = [this, &textures] (SceneNode& node, sf::Time) {
        create_projectile(node, Projectile::PlayerFire, 0.f, 0.5f, textures);
    };
    m_drop_pickup_command.category = Category::SceneGroundLayer;
    m_drop_pickup_command.action = [this, &textures] (SceneNode& node, sf::Time) {
        create_pickup(node, textures);
    };

    /** @brief Smart pointer to TextNode on the heap is initialized in default
     * constructor, for health display (and other text info...).
     * @note TextNode is suspect for errors. Has exception handling, is wrapped
     * in try-block. */

    // create text node & attach to creature
    //     * health display for player creature
    //     * name for map asset creature

    try {
        std::unique_ptr<TextNode> health_display(new TextNode(fonts, ""));

        // mem ptr that points to node
        m_health_display = health_display.get();
        attach_child(std::move(health_display));

        // uncomment to print success: match expected text nodes with expected
        // creatures
        //std::cout << "Text node initialized\n";
    } catch (std::exception& e) {
        std::cerr << "\nexception: " << e.what() << std::endl;
    }

    /// @todo Implement specific text info for player.
    //if (get_category() == Player) {
        // do special things for player
    //}

    // print success to match expected text updates with expected creatures
    std::cout << "Text for creature updated\n";
    update_texts();
}

/**
 * Overloaded insertion operator to print Creature::Type as std::string.
 * @return Returns ostream& of Creature::Type (as std::string).
 * @note Default is empty std::string.
 */
std::ostream& operator<<(std::ostream& out, const Creature::Type type)
{
    switch (type) {
    case Creature::Type::Player:
        out << "Player";
        break;
    case Creature::Type::TypeCount:
        out << std::to_string(Creature::Type::TypeCount);
        break;
    default:
        out << "";
        break;
    }
    return out;
}

void Creature::draw_current(sf::RenderTarget& target, sf::RenderStates states)
    const
{
    target.draw(m_sprite, states);
}

/**
 * Update the current Creature.
 */
void Creature::update_current(sf::Time dt, CommandQueue& commands)
{
    /** @note Checks if Creature is_destroyed() first, if is_destroyed() RNG
     * check_pickup_drop() and is_marked_for_removal(). */
    if (is_destroyed()) {
        check_pickup_drop(commands);
        m_is_marked_for_removal = true;
        return;
    }

    /** @brief check_projectile_launch() to check if attack(s) should be
     * updated. */
    check_projectile_launch(dt, commands);

    /** @brief Update Creature pathing and apply velocity. */
    update_pathing(dt);
    Entity::update_current(dt, commands);

    /** @brief Update Creature texts. */
    update_texts();
}

void Creature::update_pathing(sf::Time dt)
{
    // enemy creature - pathing
    // const ref to directions defined in data table
    const std::vector<Direction>& DIRECTIONS = TABLE[m_type].directions;
    if (!DIRECTIONS.empty()) { // do nothing if no directions
        // wait for travelled distance to be greater than defined pathing
        if (m_travelled_distance > DIRECTIONS[m_direction_index].distance) {
            // using modulo allows restarting and cycling through directions vec
            m_direction_index = (m_direction_index + 1) % DIRECTIONS.size();
            // reset distance travelled to complete next pathing instr
            m_travelled_distance = 0.f;
        }

        // compute velocity from direction
        /* NOTE: if the distance to travel is no multiple of the creature's
        speed, the creature will move further than intended. */
        float radians = to_radian(DIRECTIONS[m_direction_index].angle);
        std::cout << "Radians of creature: " << radians << "\n";
        // velocity for x = speed * cos(radians)
        float vx = get_max_speed() * std::cos(radians);
        // velocity for y = speed * sin(radians)
        float vy = get_max_speed() * std::sin(radians);
        set_velocity(vx, vy);
        std::cout << "Velocity of creature: " << vx << "x*" << vy << "y"
            << std::endl;
        // distance travelled = speed * time
        m_travelled_distance += get_max_speed() * dt.asSeconds();
    }
}

/**
 * RNG to determine if NPC should drop Pickup.
 * @see random_int() for RNG implementation.
 */
void Creature::check_pickup_drop(CommandQueue& commands)
{
    /** @attention Enemy(ies) have 1/3 chance to drop Pickup. */
    if (!is_allied() && random_int(3) == 0) {
        commands.push(m_drop_pickup_command);
    }
}

/**
 * Assign correct Texture to ID of Creature.
 * @note Default Texture is Textures::Player.
 */
Textures::ID to_texture_id(Creature::Type type)
{
    switch (type) {
    case Creature::Player:
        return Textures::Player;
    }
    // If Texture not found, default Texture is Textures::Player
    return Textures::Player;
}

/**
 * Returns Category of Creature.
 * @return unsigned int for enum Category::Type.
 * @note Default category is EnemyNPC.
 * @remark Overwrites inherited fn get_category(), to give new behavior for
 * Creature.
 */
unsigned int Creature::get_category() const
{
    /// Get type of Creature to get Category.
    if (is_allied()) {
        /// e.g. Set Creature::Player to Category::Player.
        return Category::Player;
    } else {
        return Category::MapAsset;
    }
}

/**
 * @return Returns the bounding rectangle of the creature.
 */
sf::FloatRect Creature::get_bounding_rect() const
{
    /// Uses the bounds of the sprite and its current transformation to
    /// determine bounding rectangle.
    return get_world_transform().transformRect(m_sprite.getGlobalBounds());
}

/// Uses member variable flag to determine if marked for removal or not.
bool Creature::is_marked_for_removal() const
{
    return m_is_marked_for_removal;
}

void Creature::update_texts()
{
    // For HP display:
    // set hp precision to .0f - looks better
    float hp = get_hitpoints();
    std::string hp_string;

    // scoped to avoid potential oss collisions
    {
        std::ostringstream oss;
        oss << std::setprecision(0) << std::fixed << hp
            << " days left in semester";
        hp_string = oss.str();
    }

    // set up colors:
    sf::Color occ_blue(0, 45, 106);
    sf::Color occ_orange(249, 146, 57);

    // set up styles:
    // @see SFML documentation, style is bitwise shift left
    // (e.g., "Bold = 1 << 0")
    std::uint32_t regular = 0;
    std::uint32_t bold = 1 << 0;
    std::uint32_t italic = 1 << 1;

    // default text, to be overwriten in switch statement (for specific
    // Creature Type(s)
    m_health_display->set_character_size(18);
    m_health_display->set_fill_color(occ_blue);
    m_health_display->set_style(bold);

    // DON'T OVERWRITE!
    // -rotation negates any rotation of creature and keeps text upright
    m_health_display->setRotation(-getRotation());

    switch (m_type) {
    case Creature::Player:
    // for player, show hp
        m_health_display->set_string(hp_string);

        // print success to make sure this is only done once!
        //std::cout << "Update texts: Health display text set ... success!\n"
            // and shows the correct string...
            //<< "Text: " << std::to_string(get_hitpoints()) << " HP\n";

        // overwrite default text for player hp
        m_health_display->set_character_size(13);
        //m_health_display->set_style(regular);

        // @note default text needs to be set BEFORE position, or else position
        // offset will be incorrect

        // position will vary based on creature, set in condition block
        m_health_display->setPosition(42.5f, 60.f);
        break;
    case Creature::StudentUnion:
    // for everything else, display unique name of creature
    /** @note Showing Creature name is implemented as health display! */
        m_health_display->set_string("Student Union");
        m_health_display->setPosition(0.f, 295.f);
        break;
    case Creature::CollegeCenter:
        m_health_display->set_string("College Center");
        m_health_display->setPosition(0.f, 383.f);
        break;
    case Creature::CampusSafety:
        m_health_display->set_string("Campus Safety");
        m_health_display->setPosition(0.f, 309.f);
        break;
    case Creature::Classroom:
    case Creature::ClassroomFlipped:
        m_health_display->set_string("Classroom");
        m_health_display->setPosition(0.f, 306.f);
        break;
    case Creature::LewisCenter:
        m_health_display->set_string("Lewis Center");
        m_health_display->setPosition(0.f, 295.f);
        break;
    case Creature::Library:
        m_health_display->set_string("Library");
        m_health_display->setPosition(0.f, 383.f);
        break;
    case Creature::Pool:
        m_health_display->set_string("Pool");
        m_health_display->setPosition(0.f, 262.f);
        m_health_display->set_fill_color(occ_orange);
        // to make occ orange more clear
        m_health_display->set_outline_color(sf::Color::Black);
        m_health_display->set_outline_thickness(2.5f); // 2 pt thickness
        break;
    case Creature::RelayPool:
        m_health_display->set_string("Relay Pool");
        m_health_display->setPosition(0.f, 293.f);
        m_health_display->set_fill_color(occ_orange);
        // to make occ orange more clear
        m_health_display->set_outline_color(sf::Color::Black);
        m_health_display->set_outline_thickness(2.5f); // 2 pt thickness
        break;
    case Creature::Football:
        m_health_display->set_string("Football Field");
        m_health_display->setPosition(0.f, 265.f);
        m_health_display->set_fill_color(occ_orange);
        // to make occ orange more clear
        m_health_display->set_outline_color(sf::Color::Black);
        m_health_display->set_outline_thickness(2.5f); // 2 pt thickness
        break;
    case Creature::Soccer:
        m_health_display->set_string("Soccer Field");
        m_health_display->setPosition(0.f, 264.f);
        m_health_display->set_fill_color(occ_orange);
        // to make occ orange more clear
        m_health_display->set_outline_color(sf::Color::Black);
        m_health_display->set_outline_thickness(2.5f); // 2 pt thickness
        break;
    case Creature::Tennis:
        m_health_display->set_string("Tennis Field");
        m_health_display->setPosition(0.f, 266.f);
        m_health_display->set_fill_color(occ_orange);
        // to make occ orange more clear
        m_health_display->set_outline_color(sf::Color::Black);
        m_health_display->set_outline_thickness(2.5f); // 2 pt thickness
        break;
    case Creature::Harbor:
        m_health_display->set_string("The Harbor");
        m_health_display->setPosition(0.f, 440.f);
        break;
    case Creature::Mbcc:
        m_health_display->set_string(
<<<<<<< HEAD
                "Mathematics Business & Computing Center");
=======
                "Mathematics Business & Computer Center");
>>>>>>> 70358e4573c4f036ca0905dffe86c5741d7fd904
        m_health_display->setPosition(0.f, 553.f);
        break;
    case Creature::Maintenance:
        m_health_display->set_string("Maintenance");
        m_health_display->setPosition(0.f, 363.f);
        break;
    case Creature::Starbucks:
        m_health_display->set_string("Coffee");
        m_health_display->setPosition(0.f, 315.f);
        break;
    case Creature::Track:
        m_health_display->set_string("Track Field");
        m_health_display->setPosition(0.f, 265.f);
        m_health_display->set_fill_color(occ_orange);
        // to make occ orange more clear
        m_health_display->set_outline_color(sf::Color::Black);
        m_health_display->set_outline_thickness(2.5f); // 2 pt thickness
        break;
    case Creature::Baseball:
        m_health_display->set_string("Baseball Field");
        m_health_display->setPosition(0.f, 264.f);
        m_health_display->set_fill_color(occ_orange);
        // to make occ orange more clear
        m_health_display->set_outline_color(sf::Color::Black);
        m_health_display->set_outline_thickness(2.f); // 2 pt thickness
        break;
    default:
        // default is empty string
        m_health_display->set_string("");
        m_health_display->setPosition(0.f, 0.f);
    }
}

/**
 * @warning UNUSED.
 * Checks if projectile is queued to launch and forwards Command to
 * CommandQueue.
 * @param sf::Time dt
 * Delta time as sf::Time
 * @param CommandQueue& commands
 * Command(s) to be forwarded to CommandQueue
 */
void Creature::check_projectile_launch(sf::Time delta_time,
        CommandQueue& commands)
{
    /// Enemies try to fire all the time.
    if (!is_allied())
        attack();

    /// Rapid attacks are bound by is_attack() & m_attack_countdown bound to
    /// delta time - to respect game logic.
    // only proceed if is_attacking & attack countdown = 0
    if (m_is_attacking && m_attack_countdown <= sf::Time::Zero) {
        // queue attack in command queue - commands to be exec in order recieved
        commands.push(m_attack_command);
        // attack rate + 1, to divide 1 / 1 = remainder, attack rate in seconds
        m_attack_countdown += sf::seconds(1.f / (m_attack_rate + 1.f));
        // attack has been done! no longer attacking...
        m_is_attacking = false;
    }
    // if attack countdown > 0, decrement by delta time until = 0 -> to follow
    // game logic!
    else if (m_attack_countdown > sf::Time::Zero) {
        m_attack_countdown -= delta_time;
    }
    /// @todo Different styles of attack...
}

void Creature::create_projectile(SceneNode& node, Projectile::Type type,
        float x_offset, float y_offset, const TextureHolder& textures) const
{
    /// Smart pointer to projectile initialized on the heap.
    std::unique_ptr<Projectile> projectile(new Projectile(type, textures));
    // to create outside sprite -> offset is (x, y) offset * sprite (x, y)
    sf::Vector2f offset(x_offset * m_sprite.getGlobalBounds().width,
            y_offset * m_sprite.getGlobalBounds().height);
    // get velocity from max speed of projectile
    sf::Vector2f velocity(projectile->get_max_speed(),
            projectile->get_max_speed());
    // enemy projectiles go down, allied projectiles go up
    float sign = is_allied() ? -1.f : 1.f;

    // pos = pos + (offset from sprite * 1 || -1 (up or down enemy/friend))
    /// Uses setPosition(), a SFML member fn.
    projectile->setPosition(get_world_position() + (offset * sign));
    /// Uses set_velocity(), a user-defined member fn.
    projectile->set_velocity(velocity * sign); // up or down? enemy/friendly

    /// Attach the projectile as a child scene node of the scene graph.
    node.attach_child(std::move(projectile));
}

/**
 * Check if Creature is allied or not (Player or Ally).
 * @return True if allied, false if not.
 */
bool Creature::is_allied() const
{
    return m_type == Player;
}

/**
 * Get max speed of Creature.
 * @return Max speed of Creature::Type from data_tables.cpp.
 */
float Creature::get_max_speed() const
{
    return TABLE[m_type].speed;
}

/**
 * @warning UNUSED, m_is_attacking indefinitely set to FALSE.
 * Sets m_is_attacking to true (to attack).
 * @note Only Creature(s) with an attack_interval that isn't zero are able to
 * attack.
 */
void Creature::attack()
{
    // guard to make sure attack_interval != 0
    if (TABLE[m_type].attack_interval != sf::Time::Zero)
        m_is_attacking = true;
    // make unsuable, always set m_is_attacking to FALSE on every call...
    m_is_attacking = false;
}

void Creature::create_pickup(SceneNode& node, const TextureHolder& textures)
    const
{
    /// Get random pickup type, using random_int() and type count of pickups.
    auto type = static_cast<Pickup::Type>(random_int(Pickup::TypeCount));

    /// Create unique_ptr to pickup on the heap.
    std::unique_ptr<Pickup> pickup(new Pickup(type, textures));

    pickup->setPosition(get_world_position());
    pickup->set_velocity(0.f, 1.f);

    /// Attach pickup as child scene node on the scene graph.
    node.attach_child(std::move(pickup));
}

void Creature::create_map_asset(SceneNode& node, MapAsset::Type type,
                                const TextureHolder& textures) const
{
    //const sf::Vector2f SPAWN_POINT(2450.f, 850.f);
    ////auto type = static_cast<MapAsset::Type>(MapAsset::Building);
    //std::unique_ptr<MapAsset> map_asset(new MapAsset(type, textures));
    //map_asset->setPosition(SPAWN_POINT.x + 50.f, SPAWN_POINT.y + 50.f);
    //std::cout << "Created MapAsset.\n";
}

// NOTE: To be revisited. Either delete or implement. Don't want to lose idea.
///**
//* @param sf::Vector2f empty, pass an empty sf::Vector2f and it will be modified
//* to contain last Player movement.
//* @return void, so function cannot be misused.
//* @note ONLY for Player movement, to revert last movement.
//*/
//void Creature::get_last_movement(sf::Vector2f pmov) const
//{
//    if (Category::Player)
//}
