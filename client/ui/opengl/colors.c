#include "posix.h"

#include "client/ui/opengl/colors.h"

struct colors colors = {
	.ent = {
		[et_worker]        = { 0.0, 0.0, 0.0, 1.0 },
		[et_elf_corpse]    = { 0.3, 0.8, 0.4, 1.0 },
		[et_deer]          = { 0.0, 0.0, 0.0, 1.0 },
		[et_fish]          = { 0.0, 0.0, 0.0, 1.0 },
		[et_vehicle_boat]  = { 0.0, 0.0, 0.0, 1.0 },
		[et_resource_wood] = { 0.0, 0.0, 0.0, 1.0 },
		[et_resource_meat] = { 0.0, 0.0, 0.0, 1.0 },
		[et_resource_rock] = { 0.0, 0.0, 0.0, 1.0 },
		[et_resource_crop] = { 0.0, 0.0, 0.0, 1.0 },
		[et_storehouse]    = { 0.0, 0.0, 0.0, 1.0 },
		[et_elf_friend]    = { 1.0, 1.0, 0.8, 1.0 },
		[et_elf_foe]       = { 0.0, 0.0, 0.0, 1.0 },
	},
	.tile = {
		[tile_sea]      = { 0.4, 0.56, 0.42, 1.0 },
		[tile_coast]    = { 1.0, 0.96, 0.42, 1.0 },
		[tile_plain]    = { 0.44, 0.69, 0.19, 1.0 },
		[tile_tree]     = { 0.0, 0.5, 0.03, 1.0 },
		[tile_rock]     = { 0.1, 0.1, 0.1, 1.0 },
		[tile_dirt]     = { 0.41, 0.28, 0.15, 1.0 },
		[tile_old_tree] = { 0.28, 0.5, 0.0, 1.0 },
		[tile_fire]     = { 0.0, 0.0, 0.0, 1.0 },
		[tile_ash]      = { 0.0, 0.0, 0.0, 1.0 },
	},
};