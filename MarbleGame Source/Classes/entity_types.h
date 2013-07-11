#ifndef __ENTITY_TYPES__
#define __ENTITY_TYPES__

typedef enum entity_types
{
	ET_PLAYER = 0,
	ET_CUBE,
	ET_PLATFORM,
	ET_SPHERE,
	ET_CYLINDER,
	ET_CANNON,
	ET_TRAMPOLINE,
	ET_BUTTON,
    ET_OBJ_MODEL,
	ET_CONVEYOR,
	ET_WAYPOINT,
    ET_FRACTURE,
	ET_COLLECTIBLE,
    ET_FIREBREATHER
} EntityType;

typedef enum culling_types
{
	CT_RADIUS = 0,
	CT_AABB
} CullingType;

#endif