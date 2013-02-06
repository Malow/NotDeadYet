#pragma once

#include "World/World.h"
#include "Actor.h"

class Behavior
{
protected:
	Actor* zActor;
	World* zWorld;

public:
	Behavior(Actor* actor, World* world);
	virtual ~Behavior(){};

	Actor* GetActor() const { return zActor; }
	World* GetWorld() const { return zWorld; }

	virtual bool Update(float dt) = 0;
};