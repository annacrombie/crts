#ifndef SERVER_SIM_ENT_H
#define SERVER_SIM_ENT_H

#include "server/sim/sim.h"
#include "shared/sim/world.h"

void kill_ent(struct simulation *sim, struct ent *e);
struct ent *spawn_ent(struct world *sim);
void damage_ent(struct simulation *sim, struct ent *e, uint8_t damage);
void drop_held_ent(struct world *sim, struct ent *e);
void mount_vehicle(struct ent *e, struct ent *ve);
void unmount_vehicle(struct ent *e, struct ent *ve, struct point *up);
enum iteration_result simulate_ent(void *_sim, void *_e);
enum iteration_result process_spawn_iterator(void *_s, void *_e);
#endif