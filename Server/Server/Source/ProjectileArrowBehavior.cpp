#include <World/World.h>
#include "ProjectileArrowBehavior.h"
#include "Actor.h"

static const Vector3 GRAVITY = Vector3(0, -9.82f, 0);

ProjectileArrowBehavior::ProjectileArrowBehavior( Actor* actor, World* world ) : Behavior(actor, world)
{
	this->zSpeed = 15.0f;
	this->zVelocity = actor->GetDir();// * zSpeed;
	this->zDamping = 0.99f;
	this->zMoving = true;
}

bool ProjectileArrowBehavior::Update( float dt )
{
	if(!zMoving)
		return true;

	Vector3 newPos;
	Vector3 newDir;
	
	// Update linear position.
	newPos =  this->zActor->GetPosition();
	zVelocity.Normalize();
	zVelocity *=zSpeed;
	newPos += (zVelocity * dt);
	newDir = zVelocity;
	newDir.Normalize();

	//Rotate Mesh
	Vector3 ProjectileStartDirection = Vector3(0,0,-1);
	Vector3 ProjectileMoveDirection = newDir;

	ProjectileStartDirection.Normalize();

	Vector3 around = ProjectileStartDirection.GetCrossProduct(ProjectileMoveDirection);
	float angle = acos(ProjectileStartDirection.GetDotProduct(ProjectileMoveDirection));

	//Set Values
	this->zActor->SetPosition(newPos);
	this->zActor->SetRotation(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	this->zActor->SetRotation(around, angle);
	this->zActor->SetDir(newDir);

	//**Check if the projectile has hit the ground**
	float yValue;
	yValue = this->zWorld->CalcHeightAtWorldPos(newPos.GetXZ());

	// If true, stop the projectile and return.
	if(newPos.y <= yValue )
	{
		newPos.y = yValue;
		this->zActor->SetPosition(newPos);
		this->zMoving = false;
		
		return true;
	}

	//**Update Velocity for next update**

	// Update linear velocity from the acceleration.
	this->zVelocity += (GRAVITY * dt);

	// Impose drag.
	this->zVelocity *= pow(zDamping, dt);

	return false;
}