#include "world.h"
#include "player.h"
#include "projectile.h"
#include "pickup.h"
#include "text_node.h"
#include "utility.h"

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <string>
#include <limits>
#include <map>

namespace {
    static const sf::Vector2f SPAWN_POINT(900.f, 1100.f);
}

World::World(sf::RenderWindow& window, FontHolder& fonts) :
    // initialize all parts of the world correctly
    // window first ->
    m_window(window),

    // systems second ->
    m_textures(),
    m_fonts(fonts),
    m_scene_graph(),
    m_scene_layers(),

    // world third ->
    // set the size of the world
    m_world_bounds(0.f, 0.f, 7000.f, 4500.f),
    // set player view to be zoomed in, keeping aspect ratio - 1.78
    m_world_view(sf::FloatRect(0.f, 0.f, 1366.f, 768.f)),
    m_scroll_speed(0.f),

    // player fourth ->
    m_player_creature(nullptr),
    // spawn player in the center of the world
    //m_player_spawn_point(m_world_bounds.width / 2.f,
    //        m_world_bounds.height / 2.f)

    // spawn player at designated spawn point
    m_player_spawn_point(SPAWN_POINT.x, SPAWN_POINT.y),

    // npcs fifth ->
    /* Don't need npcs...
     * m_npc_spawn_points(),
     * m_active_npcs() */

    _map_asset_spawn_points()
{
        load_textures();
        build_scene();

        /// Prepare the view - set center to player spawn point.
        m_world_view.setCenter(m_player_spawn_point);
}

void World::update(sf::Time delta_time)
{
    m_world_view.move(0.f, m_scroll_speed * delta_time.asSeconds());
    m_player_creature->set_velocity(0.f, 0.f);

    /** @brief Setup commands to destroy entities if outside world chunk.
     * @warning NOT USED. */
    //destroy_entities_outside_chunk();

    /** @brief Forward commands to the scene graph and adapt player velocity
     * correctly. */
    while (!m_command_queue.is_empty())
        m_scene_graph.on_command(m_command_queue.pop(), delta_time);
    adapt_player_velocity();

    /// Constantly update collision detection and response (WARNING: May destroy
    /// Entity(ies).
    handle_collisions();

    // Handle Map collisions. If Player crosses a black border (how Map is
    // designed), revert the previous movement command.
    //handle_map_collisions();

    /** @remark UNUSED, no NPCs... */
    /// Remove all destroyed entities and create new ones.
    //m_scene_graph.removal();
    handle_player_death();
    //spawn_npcs();
    spawn_map_assets();

    /// Regular game update step, adapt player position (correct even though
    /// outside view, because adapt_player_position() handles appropriately).
    m_scene_graph.update(delta_time, m_command_queue);
    adapt_player_position();
    handle_map_edges();

    // uncomment to print player position
    sf::Vector2f pos = m_player_creature->getPosition();
    std::cout << std::setprecision(0) << std::fixed << "Player position: ("
        << pos.x << ", " << pos.y << ")\n";
}

void World::draw()
{
    m_window.setView(m_world_view);
    m_window.draw(m_scene_graph);
}

/**
 * Get command queue from outside the world.
 * @return Return the command queue.
 */
CommandQueue& World::get_command_queue()
{
    return m_command_queue;
}

/**
 * Loads textures for the World.
 * @remark Main fn that loads textures ... texture loading goes HERE.
 */
void World::load_textures()
{
    m_textures.load(Textures::Player, "textures/player/new-pete.png");
    //m_textures.load(Textures::FireProjectile, "textures/player/player.png");

    //m_textures.load(Textures::Bunny, "textures/player/player.png");
    //m_textures.load(Textures::Bear, "textures/player/player.png");

    //m_textures.load(Textures::HealthRefill, "textures/player/player.png");

    // Map assets:
    //m_textures.load(Textures::Map, "textures/world/occ-map-3-8192x7536.png");
    // NOTE: also need to load this into an sf::Image, for map collision
    // checking
    //_map = m_textures.get(Textures::Map).copyToImage();
    //

    load_map();
}

void World::load_map()
{
    std::string world = "textures/world/";

    m_textures.load(Textures::Grass, world + "new-grass.png");

	/* xxx leaving for example, but this is a good place to have a texture
	 * loader that (ideally from lua) calls all the textures and shoves them
	 * into a data structure to iterate down. Also, avoiding this syntax
	 * entirely and having some automated, populate from lua would be fantastic
	 * and is the goal.

    m_textures.load(Textures::StudentUnion, world + "student-union.png");
    m_textures.load(Textures::CollegeCenter, world + "college-center.png");
    m_textures.load(Textures::CampusSafety, world + "campus-safety.png");
    m_textures.load(Textures::Classroom, world + "classroom.png");
    m_textures.load(Textures::ClassroomFlipped, world + "classroom-flipped.png");
    m_textures.load(Textures::Pool, world + "pool.png");
    m_textures.load(Textures::RelayPool, world + "relay-pool.png");
    m_textures.load(Textures::Football, world + "football.png");
    m_textures.load(Textures::Soccer, world + "soccer.png");
    m_textures.load(Textures::Tennis, world + "tennis.png");
    m_textures.load(Textures::Harbor, world + "harbor.png");
    m_textures.load(Textures::Mbcc, world + "mbcc.png");
    m_textures.load(Textures::Maintenance, world + "maintenance.png");
    m_textures.load(Textures::Starbucks, world + "starbucks.png");
    m_textures.load(Textures::Track, world + "track.png");
    m_textures.load(Textures::Baseball, world + "baseball.png");
    m_textures.load(Textures::Library, world + "college-center.png");
    m_textures.load(Textures::LewisCenter, world + "student-union.png");
    m_textures.load(Textures::Scenery, world + "grass-assets-transparent.png");
    m_textures.load(Textures::Scenery1, world + "grass-assets-transparent.png");
    m_textures.load(Textures::Scenery2, world + "grass-assets-transparent.png");

	*/
}

void World::build_scene()
{
    /// Initialize all the different scene layers.
    for(std::size_t i = 0; i < LayerCount; ++i) {
        Category::Type category =
            (i == Foreground) ? Category::SceneGroundLayer : Category::None;
        SceneNode::Ptr layer(new SceneNode(category));
        m_scene_layers[i] = layer.get();

        m_scene_graph.attach_child(std::move(layer));
    }

    // Prepare map background.
    sf::Texture& texture = m_textures.get(Textures::Grass);
    sf::IntRect texture_rect(m_world_bounds);
    texture.setRepeated(true);

    // Add background sprite to the scene.
    std::unique_ptr<SpriteNode> background_sprite(new SpriteNode(
                texture, texture_rect));
    background_sprite->setPosition(m_world_bounds.left, m_world_bounds.top);
    m_scene_layers[Background]->attach_child(std::move(background_sprite));

    // Add player character to the scene.
	// xxx source player config from lua
    std::unique_ptr<Creature> player(new Creature(
                Creature::Player, m_textures, m_fonts));
    m_player_creature = player.get();
    m_player_creature->setPosition(m_player_spawn_point);
    m_scene_layers[Foreground]->attach_child(std::move(player));

    build_scenery();

    //build_map();

    /** No NPCs... */
    //add_npcs();

    add_map_assets();

}

void World::build_scenery()
{
    // sf::IntRect(top, left, width, height)
	// xxx this texture atlas should be defined in lua and read in.
    std::map<std::string, sf::IntRect> atlas {
        {"square_circle_trees", {0, 0, 999, 999}},
        {"square_triangle_trees", {1000, 0, 999, 999}},
        {"triangle_tree", {2000, 0, 999, 999}},
        {"big_fountain", {3000, 0, 951, 999}},
        {"left_hedge", {0, 1000, 999, 999}},
        {"circle_tree", {1000, 1000, 999, 999}},
        {"light_post", {2000, 1000, 999, 999}},
        {"big_rock", {3000, 1000, 951, 999}},
        {"right_hedge", {0, 2000, 999, 999}},
        {"small_fountain", {1000, 2000, 999, 999}},
        {"yellow_flower", {2000, 2000, 999, 999}},
        {"bushes", {3000, 2000, 951, 999}},
        {"benches", {0, 3000, 999, 999}},
        {"bridge", {1000, 3000, 999, 999}},
        {"bench", {2000, 3000, 999, 999}},
        {"nice_bench", {3000, 3000, 951, 999}},
        {"bush", {0, 4000, 999, 999}},
        {"path", {1000, 4000, 999, 999}},
        {"red_flower", {2000, 4000, 999, 999}},
        {"flowers", {3000, 4000, 951, 999}},
        {"medium_rock", {1000, 5000, 999, 892}},
        {"small_rock", {2000, 5000, 999, 892}}
    };

    // Sprites for Textures::Scenery (first load):
    sf::Texture& texture = m_textures.get(Textures::Scenery);

    std::unique_ptr<SpriteNode> square_circle_trees(new SpriteNode(
                texture, atlas.find("square_circle_trees")->second));
    square_circle_trees->center_origin();
    square_circle_trees->setPosition(1409.f, 672.f);
    square_circle_trees->scale(0.5f, 0.5f);
    m_scene_layers[Background]->attach_child(std::move(square_circle_trees));

	/* xxx leaving this in as an example of what needs to be done in lua and
	 * heavily automated the unrolling

    std::unique_ptr<SpriteNode> medium_rock(new SpriteNode(
                texture, atlas.find("medium_rock")->second));
    medium_rock->center_origin();
    medium_rock->setPosition(6049.f, 120.f);
    medium_rock->scale(0.5f, 0.5f);
    m_scene_layers[Background]->attach_child(std::move(medium_rock));
	
	*/

    /* xxx same logic, what needs to be done in lua

    // Sprites for Textures::Scenery2 (third load):
	// could perhaps load the 2nd atlases multiple times from lua, but pretty
	// sure lua creates a better approach.
    //sf::Texture& texture2 = m_textures.get(Textures::Scenery2);

    // pos already set!
    std::unique_ptr<SpriteNode> square_circle_trees2(new SpriteNode(
                texture2, atlas.find("square_circle_trees")->second));
    square_circle_trees2->center_origin();
    square_circle_trees2->setPosition(5861.f, 1539.f);
    square_circle_trees2->scale(0.5f, 0.5f);
    m_scene_layers[Background]->attach_child(std::move(square_circle_trees2));

	// xxx need a way of defining layer attributes, or if that's relevant (in
	// lua...)
	*/
}

/**
 * @warning Not implemented.
 * @todo Implement.
 */
void World::build_map()
{
    // SPAWN_POINT(2450.f, 850.f);
    std::vector<std::string> assets = { "student_union", "another_thing" };
    for (int i = 0; i < assets.size(); ++i) {
        assets.at(0);
    }
}

// xxx this seems like something that could be done in lua, also allows the
// ability to define player movement differently based on states easier (e.g.,
// STATE: move speed increase -> call different script). Potential analysis is
// that the computations for player should not be scripted and just be in cpp
// to avoid the interpreter (speed).
void World::adapt_player_position()
{
    /// Initialize view bounds to world view.
    // 1st param, top left: center - (size / 2)
  	sf::FloatRect view(m_world_view.getCenter()
            - (m_world_view.getSize() / 2.f), m_world_view.getSize());
    /// Initialize distance from "border" (before view pan).
    // what distance from border before panning view? 16.f = 16px
	constexpr float border = 125.f;

    /// Initialize position to player position.
	sf::Vector2f pos = m_player_creature->getPosition();

    // numbers for map pan calculations
    float x_range = m_world_bounds.width;
    float y_range = m_world_bounds.height;

	// if pos.x <= pan neg to the left on the x-axis
    if (pos.x <= view.left + border) {
        if (m_world_view.getCenter().x - view.width <= 0) {
            float edge = abs(m_world_view.getCenter().x - view.width);
            m_world_view.move(-(view.width / 2.f) + edge, 0.f);
        } else {
            // pan half distance of view bounds (world view)
            m_world_view.move(-(view.width / 2.f), 0.f);
        }
    }

    // if pos.x >= pan pos to the right on the x-axis
    if (pos.x >= view.left + view.width - border) {
        if (m_world_view.getCenter().x + view.width >= x_range) {
            float edge = m_world_view.getCenter().x + view.width - x_range;
            m_world_view.move((view.width / 2.f) - edge, 0.f);
        } else {
            m_world_view.move(view.width / 2.f, 0.f);
        }
    }

    // same as x-axis, for y-axis
    if (pos.y <= view.top + border) {
        if (m_world_view.getCenter().y - view.height <= 0) {
            float edge = abs(m_world_view.getCenter().y - view.height);
            m_world_view.move(0.f, -(view.height / 2.f) + edge);
        } else {
            m_world_view.move(0.f, -(view.height / 2.f));
        }
    }

	if (pos.y >= view.top + view.height - border) {
        if (m_world_view.getCenter().y + view.height >= y_range) {
            float edge = m_world_view.getCenter().y + view.height - y_range;
            // -1 is magic number, seg faulting when crossing y max boundary!
            m_world_view.move(0.f, (view.height / 2.f) - edge - 1);
        } else {
            m_world_view.move(0.f, view.height / 2.f);
        }
    }

    /// Set player position to current position.
	m_player_creature->setPosition(pos);

    // uncomment to print current player pos
    //std::cout << "Player position: (" << pos.x << ", " << pos.y << ")\n";
}

// lua?
void World::adapt_player_velocity()
{
    sf::Vector2f velocity = m_player_creature->get_velocity();

    // if moving diagonally, reduce velocity (to always have same velocity)
    if (velocity.x != 0.f && velocity.y != 0.f)
        m_player_creature->set_velocity(velocity / std::sqrt(2.f));

    // add scrolling velocity
    m_player_creature->accelerate(0.f, m_scroll_speed);
}

/**
 * @warning UNUSED, but implemented.
 * Spawns NPCs from m_npc_spawn_points.
 * @see add_npcs().
 * @note Assumes m_npc_spawn_points is sorted in ascending order. Iterating
 * from lowest to highest coordinates is more likely to be the common case.
 */
// xxx a lot of this unrolling should be parsing lua scripts
void World::spawn_npcs()
{
    /** @todo Decide on whether to implement world chunks or not.
    // if there's avail spawn points...
    if (!m_npc_spawn_points.empty()) {
        // iter through spawn points until at current world chunk (...lowest
        // coord are more likely to be the common case)
        for (auto iter = m_npc_spawn_points.begin();
                iter != m_npc_spawn_points.end(); ++iter) {
            // if spawn points are within current world chunk...
            MATH: spawn.x > chunk.left
            && spawn.x < (chunk.left + chunk.size.x)
            && spawn.y < chunk.top
            && spawn.y > (chunk.top - chunk.size.y)
            if (iter->x > get_chunk_bounds().left
                    && iter->x < (get_chunk_bounds().left
                        + get_chunk_bounds().getSize().x)
                    && iter->y < get_chunk_bounds().top
                    && iter->y > (get_chunk_bounds().top
                        - get_chunk_bounds().getSize().y)) {
                // ... spawn & del from vec...
                // init SpawnPoint after check to not create unneccesary structs
                SpawnPoint spawn = *iter;
                // create smart ptr to npc spawn on heap
                std::unique_ptr<Creature> npc(
                        new Creature(spawn.type, m_textures, m_fonts));
                std::cout << spawn.type << " spawned in the world!"
                    << std::endl;
                // set enemy pos to spawn pos
                npc->setPosition(spawn.x, spawn.y);
                // bind to foreground layer
                m_scene_layers[Foreground]->attach_child(std::move(npc));
                // remove spawn point from vec & keep iter for valid spawns
                iter = m_npc_spawn_points.erase(iter);
            }
            // TODO: break out of loop when spawn point > chunk to not do
            // unneccesary iter
        }
    }
    */
    // if npc spawn points vector is not empty...
    if (!m_npc_spawn_points.empty()) {
        // iter through each spawn point and spawn
        for (auto iter = m_npc_spawn_points.rbegin();
                iter != m_npc_spawn_points.rend(); ++iter) {
            // init SpawnPoint AFTER check to not create unneccesary structs
            SpawnPoint spawn = *iter;
            // create smart ptr to spawn npc on heap
            std::unique_ptr<Creature> npc(
                    new Creature(spawn.type, m_textures, m_fonts));
            // set enemy pos to spawn pos
            npc->setPosition(spawn.vec2.x, spawn.vec2.y);
            // print success and pos for confirmation
            std::cout << spawn.type << " spawned in the world!" << " ("
                << std::setprecision(0) << spawn.vec2.x << ", " << spawn.vec2.y
                << ")\n";

            // bind to foreground layer
            m_scene_layers[Foreground]->attach_child(std::move(npc));
            // riter, so pop_back end and "increment" backwards (moving to begin
            // until vector is empty and all spawn points have been spawned)
            m_npc_spawn_points.pop_back();
        }
    }
}

/**
 * @warning UNUSED, but implemented.
 * Adds ONE NPC at a time to m_npc_spawn_points.
 * @param Creature::Type type
 * The type of the Creature to be added.
 * @param const sf::Vector2& vec2_rel
 * The relative position (to the Player) of the Creature to be added.
 * @todo Should vec2_rel be const or not?
 * @attention Used by add_npcs() to add ALL NPCs. Is an abstraction, to be
 * reused.
 * @see add_npcs()
 */
// xxx parse lua config and unroll
void World::add_npc(Creature::Type type, sf::Vector2f& vec2_rel)
{
    // spawn with enemy type, spawn pos + player pos -> to spawn rel to player
    sf::Vector2f rel{m_player_spawn_point.x + vec2_rel.x,
        m_player_spawn_point.y + vec2_rel.y};
    SpawnPoint spawn(type, rel);
    // after init spawn with enemy type and pos of spawn, push into spawn point
    // vec
    m_npc_spawn_points.push_back(spawn);
    std::cout << spawn.type << " added to NPC spawn points" << std::endl;
}

/**
 * @warning UNUSED, but implemented.
 * Adds ALL NPCs to be spawned to m_npc_spawn_points.
 * @note Uses add_npc() abstraction to add NPCs to m_npc_spawn_points.
 * @see add_npc().
 * @note Sorts NPCs in descending order, based on y-axis.
 * @remark Does NOT SPAWN NPCs.
 * @see spawn_npcs().
 */
// xxx parse lua config and unroll
void World::add_npcs()
{
    sf::Vector2f bunny_spawn{20.f, 50.f};
    sf::Vector2f bear_spawn{100.f, 200.f};
    //add_npc(Creature::Bunny, bunny_spawn);
    //add_npc(Creature::Bear, bear_spawn);

    // sort enemy spawn points so they can be iter through in an expected way
    std::sort(m_npc_spawn_points.begin(), m_npc_spawn_points.end(), []
            (SpawnPoint lhs, SpawnPoint rhs) {
        // sorting algorithm used - descending sort, based on y-axis
        return (lhs.vec2.y < rhs.vec2.y);
    });
}

/**
 * Destroys Entity(ies) outside current world chunk, to reduce resource strain.
 * @warning NOT USED.
 * @todo Decide whether to implement chunk system or not?
 */
// xxx cpp for sure, but there's probably nice interplay with lua (like define
// memory config to optimize in lua and source it into here).
void World::destroy_entities_outside_chunk()
{
    /*
    Command command;
    /// Entities to be destroyed: Projectile and EnemyNpc.
    command.category = Category::Projectile | Category::EnemyNpc;
    command.action = derived_action<Entity>([this] (Entity& e, sf::Time) {
            /// If entity bounding rectangle leaves chunk bounds, then destroy.
            if (!get_chunk_bounds().intersects(e.get_bounding_rect()))
                e.destroy();
    });
    /// Push destroy() command into command queue.
    m_command_queue.push(command);
    */
}


/** Helper function that determines if the colliding scene nodes match certain
 * expected categories.
 * @return Returns true if the colliders match the expected categories, false
 * if not.
 * @remark There are four types of collisions that are checked to match certain
 * categories - Player/Pickup, Player/EnemyNpc, Player/EnemyProjectile,
 * PlayerProjectile/EnemyNpc.
 */
bool matches_categories(SceneNode::Pair& colliders, Category::Type type1,
                        Category::Type type2)
{
    /// Colliders are stored in a pair, first and second are colliders.
    unsigned int category1 = colliders.first->get_category();
    unsigned int category2 = colliders.second->get_category();
    // can perform bitwise comparison because unsigned ints
    if (type1 & category1 && type2 & category2) {
        return true;
    } else if (type1 & category2 && type2 & category1) {
        // still a match! just not in expected order
        /// If not in expected order, swap pair so that it is in expected order,
        /// also return true because the colliders match the expected types.
        std::swap(colliders.first, colliders.second);
        return true;
    } else {
        return false;
    }
}

/**
 * @return Returns a sf::FloatRect that is the bounds of the view.
 */
sf::FloatRect World::get_view_bounds() const
{
    // view bounds: x = (world center - world size) / 2, y = world size
    return sf::FloatRect(m_world_view.getCenter() - m_world_view.getSize() / 2.f,
            m_world_view.getSize());
}

/**
 * @return Returns a sf::FloatRect that is the bounds of the current world chunk.
 * @note Chunks are a pixel offset from current view bounds (x, y).
 */
sf::FloatRect World::get_chunk_bounds() const
{
    /// Based on current view bounds.
    sf::FloatRect bounds = get_view_bounds();
    /// Chunks are 1000 pixel offset from current view bounds (x, y).
    bounds.top -+ 1000.f;
    bounds.height += 1000.f;
    return bounds;
}

/**
 * Uses matches_categories() to decide how to handle each collider pair (as
 * desired).
 */
void World::handle_collisions()
{
    /// Initialize collision_pairs set for scene graph to use.
    std::set<SceneNode::Pair> collision_pairs;
    m_scene_graph.check_scene_collision(m_scene_graph, collision_pairs);
    for (SceneNode::Pair pair : collision_pairs) {
        /// For Player/Pickup, apply the pickup to the player and destroy the
        /// pickup.
        //if (matches_categories(pair, Category::Player, Category::PlayerPickup)) {
        //    // static cast the pair's type to the expected type to make sure
        //    // (safe because the pair's type is expected to match), and create
        //    // local variables storing each - to work with.
        //    auto& player = static_cast<Creature&>(*pair.first);
        //    auto& pickup = static_cast<Pickup&>(*pair.second);
        //    pickup.apply(player);
        //    pickup.destroy();
        //} else if (matches_categories(pair, Category::Player,
        //            Category::EnemyNpc)) {
        //    /// For Player/EnemyNpc, damage the player and destroy the enemy.
        //    auto& player = static_cast<Creature&>(*pair.first);
        //    auto& enemy = static_cast<Creature&>(*pair.second);
        //    player.damage(enemy.get_hitpoints());
        //    enemy.destroy();
        //} else if (matches_categories(pair, Category::Player,
        //            Category::EnemyProjectile)
        //            || matches_categories(pair, Category::PlayerProjectile,
        //            Category::EnemyNpc)) {
        //    /// For Player/EnemyProjectile and PlayerProjectile/EnemyNpc
        //    /// (handled the same because projectiles are to be handled the same,
        //    /// regardless of recipient), damage the recipient and destroy the
        //    /// projectile.
        //    auto& creature = static_cast<Creature&>(*pair.first);
        //    auto& projectile = static_cast<Projectile&>(*pair.second);
        //    creature.damage(projectile.get_damage());
        //    projectile.destroy();
        if (matches_categories(pair, Category::Player,
                                      Category::MapAsset)) {
            std::cout << "Collision with MapAsset detected!\n";

            auto& player = static_cast<Creature&>(*pair.first);
            auto& map = static_cast<Creature&>(*pair.second);

            sf::FloatRect pbound = player.get_bounding_rect();
            float pbound_right = (pbound.left + pbound.width);
            float pbound_bottom = pbound.top + pbound.height;

            sf::FloatRect mbound = map.get_bounding_rect();
            float mbound_right = mbound.left + mbound.width;
            float mbound_bottom = mbound.top + mbound.height;

            sf::Vector2f pvel = player.get_velocity();

            // left-side collision
            if (pbound.left <= mbound_right) {
                float overlap = mbound_right - pbound.left;
                pvel.x -= overlap;
                // to soften collision animation ->
                pvel.x *= 0.25f;
                player.set_velocity(pvel);
            }
            // right-side collision
            if (pbound_right >= mbound.left) {
                float overlap = pbound_right - mbound.left;
                pvel.x += overlap;
                pvel.x *= 0.25f;
                player.set_velocity(pvel);
            }
            // bottom-up collision
            if (pbound.top <= mbound_bottom) {
                float overlap = mbound_bottom - pbound.top;
                pvel.y -= overlap;
                pvel.y *= 0.25f;
                player.set_velocity(pvel);
            }
            // top-down collision
            if (pbound_bottom >= mbound.top) {
                float overlap = pbound_bottom - mbound.top;
                pvel.y += overlap;
                pvel.y *= 0.25f;
                player.set_velocity(pvel);
            }

            // damage player 0.05 hp (~1/24th of a day)
            player.damage(0.05);
        }
    }
}

void World::handle_map_collisions()
{
    sf::Vector2f pos = m_player_creature->getPosition();
    //sf::Vector2f bounds = m_player_creature->getGlobalBounds();
    //float left = pos.x - (bounds.height / 2.f);
    //float right = (pos.x + bounds.width) - (bounds.height / 2.f);
    //float top = pos.x + (bounds.width / 2.f);
    //float bottom = pos.x;
    sf::Color color_at_xy = _map.getPixel(pos.x, pos.y);

    // sort out map collisions: conditions to check if player sprite intersects
    // with black borders on map
    if (color_at_xy == sf::Color(120, 4, 34)
            || color_at_xy == sf::Color(63, 0, 127)
            || color_at_xy == sf::Color(95, 62, 29)
            || color_at_xy == sf::Color(255, 106, 0)) {
        std::cout << "Player hit wall of building!\n";
        sf::Vector2f prev = PREV_PLAYER_MOVEMENT;
        std::cout << prev.x << "\n" << prev.y << "\n";
        m_player_creature->accelerate(-prev);

        // damage player 0.05 hp (~1/24th of a day)
        m_player_creature->damage(0.05);
    }

    //if player pos == black
    //then accelerate -PlayerMover;
    //
    //

    //m_player_creature->PlayerMover()

    //// how maps collisions have to work...
    ////     get player pos
    ////     get color at pos
    ////     if color == black then
    ////     find what direction the intersection is
    ////
    ////
    ////     revert previous movement command...
    ////
    ////
    ////
    ////     calc what direction and how much of intersection
    //
    //// helper functions...
    //float left = []() {
    //    return pos.x - m_player_creature.getSize().x / 2.f; });
    //float right = []() {
    //    return pos.x + m_player_creature.getSize().x / 2.f; });
    //float top = []() {
    //    return pos.y - m_player_creature.getSize().y / 2.f; });
    //float bottom = []() {
    //    return pos.x + m_player_creature.getSize().y / 2.f; });

    //float overlap_left
}

void World::handle_map_edges()
{
    sf::Vector2f pos = m_player_creature->getPosition();
    float x_range = m_world_bounds.width;
    float y_range = m_world_bounds.height;
    if (pos.x <= 0.f) {
        pos.x = 0.f;
        m_player_creature->setPosition(pos);
    }
    if (pos.x >= x_range) {
        pos.x = x_range;
        m_player_creature->setPosition(pos);
    }
    if (pos.y <= 0.f) {
        pos.y = 0.f;
        m_player_creature->setPosition(pos);
    }
    // -1 is magic number, seg faulting when crossing y max boundary!
    if (pos.y >= y_range - 1) {
        pos.y = y_range - 1;
        m_player_creature->setPosition(pos);
    }
}

void World::handle_player_death()
{
    if (m_player_creature->is_marked_for_removal()) {
        // @note original unique_ptr will lose scope and call its destructor
        // - memory safe
        m_scene_graph.removal();
        // Add player character to the scene.
        std::unique_ptr<Creature> player(new Creature(
                Creature::Player, m_textures, m_fonts));
        m_player_creature = player.get();
        m_player_creature->setPosition(m_player_spawn_point);
        m_scene_layers[Foreground]->attach_child(std::move(player));
    }
}

void World::add_map_asset(Creature::Type type, sf::Vector2f& coord)
{
    // @note NOT relative to player!
    //sf::Vector2f rel(m_player_spawn_point.x + vec2_rel.x,
    //                 m_player_spawn_point.y + vec2_rel.y);

    SpawnPoint spawn(type, coord);

    // after init spawn with enemy type and pos of spawn, push into spawn point
    // vec
    _map_asset_spawn_points.push_back(spawn);
    std::cout << spawn.type << " added to MapAsset spawn points" << std::endl;
}

void World::add_map_assets()
{
    sf::Vector2f student_union(4300.f, 700.f);
    add_map_asset(Creature::StudentUnion, student_union);

    sf::Vector2f college_center(3000.f, 700.f);
    add_map_asset(Creature::CollegeCenter, college_center);

    sf::Vector2f campus_safety(5500.f, 800.f);
    add_map_asset(Creature::CampusSafety, campus_safety);

    sf::Vector2f classroom(3900.f, 1700.f);
    add_map_asset(Creature::Classroom, classroom);

    sf::Vector2f classroom_dupe_1(3400.f, 2000.f);
    add_map_asset(Creature::Classroom, classroom_dupe_1);

    sf::Vector2f classroom_dupe_2(2000.f, 500.f);
    add_map_asset(Creature::Classroom, classroom_dupe_2);

    sf::Vector2f classroom_flipped_1(6500.f, 1000.f);
    add_map_asset(Creature::ClassroomFlipped, classroom_flipped_1);

    sf::Vector2f classroom_flipped_2(6600.f, 1700.f);
    add_map_asset(Creature::ClassroomFlipped, classroom_flipped_2);

    sf::Vector2f pool(2400.f, 1900.f);
    add_map_asset(Creature::Pool, pool);

    sf::Vector2f relay_pool(1800.f, 2250.f);
    add_map_asset(Creature::RelayPool, relay_pool);

    sf::Vector2f football(1800.f, 1250.f);
    add_map_asset(Creature::Football, football);

    sf::Vector2f soccer(500.f, 1500.f);
    add_map_asset(Creature::Soccer, soccer);

    sf::Vector2f tennis(500.f, 2350.f);
    add_map_asset(Creature::Tennis, tennis);

    sf::Vector2f harbor(1300.f, 3500.f);
    add_map_asset(Creature::Harbor, harbor);

    sf::Vector2f harbor_dupe_1(700.f, 3300.f);
    add_map_asset(Creature::Harbor, harbor_dupe_1);

    sf::Vector2f harbor_dupe_2(100.f, 3500.f);
    add_map_asset(Creature::Harbor, harbor_dupe_2);

    sf::Vector2f mbcc(5000.f, 1900.f);
    add_map_asset(Creature::Mbcc, mbcc);

    sf::Vector2f maintenance(4500.f, 3000.f);
    add_map_asset(Creature::Maintenance, maintenance);

    sf::Vector2f starbucks(2000.f, 3700.f);
    add_map_asset(Creature::Starbucks, starbucks);

    sf::Vector2f track(3000.f, 2800.f);
    add_map_asset(Creature::Track, track);

    sf::Vector2f baseball(700.f, 700.f);
    add_map_asset(Creature::Baseball, baseball);

    sf::Vector2f library(5636.f, 3400.f);
    add_map_asset(Creature::Library, library);

    sf::Vector2f lewis_center(6425.f, 2409.f);
    add_map_asset(Creature::LewisCenter, lewis_center);

    sf::Vector2f classroom_flipped_3(2850.f, 4016.f);
    add_map_asset(Creature::ClassroomFlipped, classroom_flipped_3);

    sf::Vector2f classroom_flipped_4(3508.f, 4024.f);
    add_map_asset(Creature::ClassroomFlipped, classroom_flipped_4);

    sf::Vector2f classroom_dupe_3(4539.f, 4056.f);
    add_map_asset(Creature::Classroom, classroom_dupe_3);

    sf::Vector2f classroom_dupe_4(6468.f, 3393.f);
    add_map_asset(Creature::Classroom, classroom_dupe_3);

    sf::Vector2f harbor_dupe_3(6495.f, 4077.f);
    add_map_asset(Creature::Harbor, harbor_dupe_3);

    // uncomment to print success
    //std::cout << "MapAsset Creature added.\n";
}

void World::spawn_map_assets() {
    while (!_map_asset_spawn_points.empty()) {
        SpawnPoint spawn = _map_asset_spawn_points.back();
        std::unique_ptr<Creature> map_asset(
                new Creature(spawn.type, m_textures, m_fonts));
        map_asset->setPosition(spawn.vec2.x, spawn.vec2.y);

        // print success and pos for confirmation
        std::cout << spawn.type << " spawned in the world!" << " ("
            << std::setprecision(0) << spawn.vec2.x << ", " << spawn.vec2.y
            << ")\n";

        m_scene_layers[Foreground]->attach_child(std::move(map_asset));
        _map_asset_spawn_points.pop_back();
    }
}

//void World::spawn_map_assets() {
//    // if npc spawn points vector is not empty...
//    if (!_map_asset_spawn_points.empty()) {
//        // iter through each spawn point and spawn
//        for (auto iter = _map_asset_spawn_points.rbegin();
//                iter != _map_asset_spawn_points.rend(); ++iter) {
//            // init SpawnPoint AFTER check to not create unneccesary structs
//            SpawnPoint spawn = *iter;
//            // create smart ptr to spawn npc on heap
//            std::unique_ptr<Creature> map_asset(
//                    new Creature(spawn.type, m_textures, m_fonts));
//            // set enemy pos to spawn pos
//            map_asset->setPosition(spawn.vec2.x, spawn.vec2.y);
//            // print success and pos for confirmation
//            std::cout << spawn.type << " spawned in the world!" << " ("
//                << std::setprecision(0) << spawn.vec2.x << ", " << spawn.vec2.y
//                << ")\n";
//
//            // bind to foreground layer
//            m_scene_layers[Foreground]->attach_child(std::move(map_asset));
//            // riter, so pop_back end and "increment" backwards (moving to begin
//            // until vector is empty and all spawn points have been spawned)
//            m_npc_spawn_points.pop_back();
//        }
//    }
//}
