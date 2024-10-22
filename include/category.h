#pragma once

/*
 * @namespace Category
 * Entity/SceneNode Category(ies), used to dispatch commands
 */
namespace Category {
    enum Type {
        // bit-wise category for highest efficiency & bit-wise OR to group
        None = 0,
        SceneGroundLayer = 1 << 0,
        Player = 1 << 1, // player
        MapAsset = 1 << 2,
        PlayerProjectile = 1 << 3,
        EnemyProjectile = 1 << 4,
        Pickup = 1 << 5,
        // TODO: decide if there needs to be player, friendly, neutral, enemy
        // for every entity

        Creature = Player | MapAsset,
        Projectile = PlayerProjectile | EnemyProjectile,
    };
}
