#include "ActorManager.h"
#include "Actor.h"
#include "ActorSynchronizer.h"
#include "Physics.h"
#include "BioActor.h"
#include "WorldActor.h"

ActorManager::ActorManager( ActorSynchronizer* syncher ) : 
	zSynch(syncher)
{
	AddObserver(this->zSynch);
}

ActorManager::~ActorManager()
{
	for(auto it = this->zActors.begin(); it != this->zActors.end(); it++)
	{
		Actor* temp = (*it);
		SAFE_DELETE(temp);
	}
}

void ActorManager::AddActor( Actor* actor )
{
	zActors.insert(actor);

	ActorAdded e;
	e.zActor = actor;
	NotifyObservers(&e);
}

void ActorManager::RemoveActor( Actor* actor )
{
	this->zActors.erase(actor);

	ActorRemoved e;
	e.zActor = actor;
	NotifyObservers(&e);

	delete actor;
}

Actor* ActorManager::CheckCollisions( Actor* actor, float& range )
{
	Actor* collide = NULL;
	PhysicsCollisionData data;
	float rangeWithin = range;

	for (auto it = this->zActors.begin(); it != this->zActors.end(); it++)
	{
		//If same, ignore
		if((*it) == actor)
			continue;
		
		//check length, ignore if too far.
		Vector3 vec = actor->GetPosition() - (*it)->GetPosition();
		if(vec.GetLength() > rangeWithin)
			continue;

			if(BioActor* target = dynamic_cast<BioActor*>(*it))
			{
				PhysicsObject* targetObject = target->GetPhysicsObject();
				data = GetPhysics()->GetCollisionRayMesh(actor->GetPosition(), actor->GetDir(), targetObject);

				if(data.collision && data.distance < range)
				{
					range = data.distance;
					collide = (*it);
				}
			}
	}

	return collide;
}

Actor* ActorManager::GetActor( const unsigned int ID ) const
{
	for(auto it_Actor = this->zActors.begin(); it_Actor != this->zActors.end(); it_Actor++)
	{
		if((*it_Actor)->GetID() == ID)
		{
			return (*it_Actor);
		}
	}
	return NULL;
}