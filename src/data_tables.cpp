#include "data_tables.h"
#include "creature.h"
#include "projectile.h"
#include "pickup.h"
#include "map-asset.h"

// for std::bind() placeholders _1, _2, & so on...
using namespace std::placeholders;

std::vector<CreatureData> initialize_creature_data()
{
    // init vector to amt of creatures (typecount enum holds creature count)
    std::vector<CreatureData> data(Creature::TypeCount);

    // PLAYER DATA
    data[Creature::Player].hitpoints = 30.f;
    data[Creature::Player].speed = 75.f;
    data[Creature::Player].texture = Textures::Player;
    data[Creature::Player].attack_interval = sf::seconds(1);

    // ADDITIONAL CREATURE DATA...
    data[Creature::StudentUnion].texture = Textures::StudentUnion;
    data[Creature::StudentUnion].hitpoints = 100.f;
    data[Creature::StudentUnion].speed = 0.f;
    data[Creature::StudentUnion].attack_interval = sf::seconds(1);

    data[Creature::CollegeCenter].texture = Textures::CollegeCenter;
    data[Creature::CollegeCenter].hitpoints = 100.f;
    data[Creature::CollegeCenter].speed = 0.f;
    data[Creature::CollegeCenter].attack_interval = sf::seconds(1);

    data[Creature::CampusSafety].texture = Textures::CampusSafety;
    data[Creature::CampusSafety].hitpoints = 100.f;
    data[Creature::CampusSafety].speed = 0.f;
    data[Creature::CampusSafety].attack_interval = sf::seconds(1);

    data[Creature::Classroom].texture = Textures::Classroom;
    data[Creature::Classroom].hitpoints = 100.f;
    data[Creature::Classroom].speed = 0.f;
    data[Creature::Classroom].attack_interval = sf::seconds(1);

    data[Creature::ClassroomFlipped].texture = Textures::ClassroomFlipped;
    data[Creature::ClassroomFlipped].hitpoints = 100.f;
    data[Creature::ClassroomFlipped].speed = 0.f;
    data[Creature::ClassroomFlipped].attack_interval = sf::seconds(1);

    data[Creature::Pool].texture = Textures::Pool;
    data[Creature::Pool].hitpoints = 100.f;
    data[Creature::Pool].speed = 0.f;
    data[Creature::Pool].attack_interval = sf::seconds(1);

    data[Creature::RelayPool].texture = Textures::RelayPool;
    data[Creature::RelayPool].hitpoints = 100.f;
    data[Creature::RelayPool].speed = 0.f;
    data[Creature::RelayPool].attack_interval = sf::seconds(1);

    data[Creature::Football].texture = Textures::Football;
    data[Creature::Football].hitpoints = 100.f;
    data[Creature::Football].speed = 0.f;
    data[Creature::Football].attack_interval = sf::seconds(1);

    data[Creature::Soccer].texture = Textures::Soccer;
    data[Creature::Soccer].hitpoints = 100.f;
    data[Creature::Soccer].speed = 0.f;
    data[Creature::Soccer].attack_interval = sf::seconds(1);

    data[Creature::Tennis].texture = Textures::Tennis;
    data[Creature::Tennis].hitpoints = 100.f;
    data[Creature::Tennis].speed = 0.f;
    data[Creature::Tennis].attack_interval = sf::seconds(1);

    data[Creature::Harbor].texture = Textures::Harbor;
    data[Creature::Harbor].hitpoints = 100.f;
    data[Creature::Harbor].speed = 0.f;
    data[Creature::Harbor].attack_interval = sf::seconds(1);

    data[Creature::Mbcc].texture = Textures::Mbcc;
    data[Creature::Mbcc].hitpoints = 100.f;
    data[Creature::Mbcc].speed = 0.f;
    data[Creature::Mbcc].attack_interval = sf::seconds(1);

    data[Creature::Maintenance].texture = Textures::Maintenance;
    data[Creature::Maintenance].hitpoints = 100.f;
    data[Creature::Maintenance].speed = 0.f;
    data[Creature::Maintenance].attack_interval = sf::seconds(1);

    data[Creature::Starbucks].texture = Textures::Starbucks;
    data[Creature::Starbucks].hitpoints = 100.f;
    data[Creature::Starbucks].speed = 0.f;
    data[Creature::Starbucks].attack_interval = sf::seconds(1);

    data[Creature::Track].texture = Textures::Track;
    data[Creature::Track].hitpoints = 100.f;
    data[Creature::Track].speed = 0.f;
    data[Creature::Track].attack_interval = sf::seconds(1);

    data[Creature::Baseball].texture = Textures::Baseball;
    data[Creature::Baseball].hitpoints = 100.f;
    data[Creature::Baseball].speed = 0.f;
    data[Creature::Baseball].attack_interval = sf::seconds(1);

    data[Creature::Library].texture = Textures::Library;
    data[Creature::Library].hitpoints = 100.f;
    data[Creature::Library].speed = 0.f;
    data[Creature::Library].attack_interval = sf::seconds(1);

    data[Creature::LewisCenter].texture = Textures::LewisCenter;
    data[Creature::LewisCenter].hitpoints = 100.f;
    data[Creature::LewisCenter].speed = 0.f;
    data[Creature::LewisCenter].attack_interval = sf::seconds(1);

    return data;
}

/**
 * @warning Unused, implement for projectile data.
 * @return std::vector<ProjectileData> data (and traits)
 * */
std::vector<ProjectileData> initialize_projectile_data()
{
    std::vector<ProjectileData> data(Projectile::TypeCount);

    /* data[Projectile::PlayerFire].damage = 5.f;
    data[Projectile::PlayerFire].speed = 200.f;
    data[Projectile::PlayerFire].texture = Textures::FireProjectile; */

    return data;
}

/**
 * @warning Unused, implement for pickup data.
 * @return std::vector<PickupData> data (and traits)
 * */
std::vector<PickupData> initialize_pickup_data()
{
    std::vector<PickupData> data(Pickup::TypeCount);

    /* data[Pickup::HealthRefill].texture = Textures::HealthRefill;
    data[Pickup::HealthRefill].action = [] (Creature& c) { c.heal(15.f); }; */

    /* data[Pickup::AttackRate].texture = Textures::AttackRate;
    data[Pickup::AttackRate].action = std::bind(&Creature::increase_attack_rate, _1);

    data[Pickup::Arrows].texture = Textures::Arrows;
    // binds std::placeholder::_1 to first param of Creature::collect_ammunition,
    // and amt 3 to second param
    data[Pickup::Arrows].action = std::bind(&Creature::collect_ammunition, _1, 3); */

    return data;
}

std::vector<MapAssetData> initialize_map_asset_data()
{
    std::vector<MapAssetData> data(MapAsset::TypeCount);
    //data[MapAsset::Building].texture = Textures::MapAsset;

    return data;
}
