#include "posix.h"

#include "shared/constants/globals.h"
#include "shared/sim/action.h"

const struct global_cfg_t gcfg = {
	.ents = {
		[et_none] = { "ghost", .trav = 0xff },
		[et_worker] = {
			"elf",
			.animate = true,
			.corpse = et_elf_corpse,
			.hp = 100,
			.lifespan = 20000,
			.trav = trav_land,
		},
		[et_elf_corpse] = {
			"elf corpse",
			.hp = 300,
			.lifespan = 100,
			.trav = 0xff
		},
		[et_deer] = {
			"deer",
			.animate = true,
			.corpse = et_resource_meat,
			.group_size = 3,
			.hp = 50,
			.lifespan = 4000,
			.spawn_chance = 10000,
			.spawn_tile = tile_plain,
			.trav = trav_land,
		},
		[et_fish] = {
			"fish",
			.animate = true,
			.corpse = et_resource_meat,
			.group_size = 3,
			.hp = 50,
			.lifespan = 4000,
			.spawn_chance = 10000,
			.spawn_tile = tile_sea,
			.trav = trav_aquatic,
		},
		[et_resource_meat] = {
			"meat",
			.holdable = true,
			.lifespan = 1000,
			.trav = 0xff,
		},
		[et_resource_crop] = {
			"crop",
			.holdable = true,
			.lifespan = 1000,
			.trav = 0xff,
		},
		[et_resource_wood] = {
			"wood",
			.holdable = true,
			.lifespan = 5000,
			.trav = 0xff,
		},
		[et_resource_rock] = {
			"rock",
			.holdable = true,
			.lifespan = 5000,
			.trav = 0xff,
		},
	},
	.tiles = {
		[tile_sea] = {
			"sea",
			.trav_type = trav_aquatic,
		},
		[tile_coast] = {
			"coast",
			.base = tile_coast,
			.next = tile_tree,
			.next_to = tile_sea,
			.trav_type = trav_land,
		},
		[tile_plain] = {
			"plain",
			.base = tile_dirt,
			.next = tile_tree,
			.next_to = tile_tree,
			.trav_type = trav_land,
			.flamable = true,
		},
		[tile_tree] = {
			"tree",
			.base = tile_dirt,
			.hardness = 100,
			.next = tile_old_tree,
			.next_to = tile_dirt,
			.trav_type = trav_land,
			.flamable = true,
		},
		[tile_old_tree] = {
			"old tree",
			.base = tile_dirt,
			.hardness = 50,
			.next = tile_tree,
			.next_to = tile_old_tree,
			.trav_type = trav_land,
			.flamable = true,
		},
		[tile_rock] = {
			"rock",
			.base = tile_dirt,
			.hardness = 253,
			.trav_type = trav_land,
		},
		[tile_dirt] = {
			"dirt",
			.base = tile_dirt,
			.next = tile_plain,
			.next_to = tile_tree,
			.trav_type = trav_land,
		},
		[tile_fire] = {
			"fire",
			.base = tile_ash,
			.function = true,
			.trav_type = trav_land,
		},
		[tile_ash] = {
			"ashes",
			.base = tile_dirt,
			.trav_type = trav_land,
		},
	},
	/* Fields ending in _rate specify the number of ticks before some event
	 * happens.  Fields ending in _chance specify a 1 in that number chance
	 * of an event happening.
	 */
	.misc = {
		.spawnable_ents = { et_deer, et_fish },
		/* damage taken by ents in fire */
		.fire_damage = 50,
		/* time before fire can spread */
		.fire_spread_rate = 10,
		/* chance of spreading */
		.fire_spread_chance = 10,
		/* once spreading, per tile chance of igniting */
		.fire_spread_ignite_chance = 2,
		/* random % this number produces starting x and y spawn coords */
		.initial_spawn_range = 100,
		/* amount of starting elves */
		.initial_spawn_amount = 64,
		/* chance an idle ent will move randomly */
		.meander_chance = 55,
		/* maximum amount an ent can age over its lifespan before dying */
		.max_over_age = 1000,
	}
};
