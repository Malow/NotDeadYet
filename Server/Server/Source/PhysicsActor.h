/*
Made by Ed�nge Simon 2013-01-22
for project Not Dead Yet at Blekinge tekniska h�gskola.
*/

#pragma once

#include "Actor.h"
#include "PhysicsObject.h"
#include <NetworkMessageConverter.h>

class PhysicsActor : public Actor
{
public:
	PhysicsActor();
	PhysicsActor(const Vector3& startpos, PhysicsObject* pObj);
	PhysicsActor(const Vector3& startpos, PhysicsObject* pObj, const Vector4& rot);

	virtual ~PhysicsActor();

	/*! PhysicsObject needs to be initialized.*/
	inline Vector3 GetPosition() const 
	{
		if(this->zPhysicObj) 
			return this->zPhysicObj->GetPosition(); 

		Vector3 def;
		return def;
	}
	/*! Returns model length in scale 1, 1, 1 .*/
	float GetModelLength() const {return zModelLength;}
	/*! Returns the model initial direction, the direction in model space.*/
	inline Vector3 GetInitialDirection() const {return zInitialDirection;}
	/*! PhysicsObject needs to be initialized.*/

	const Vector3 GetScale() const 
	{
		if(this->zPhysicObj) 
			return this->zPhysicObj->GetScaling(); 
		
		Vector3 def;
		return def;
	}
	/*! PhysicsObject needs to be initialized.*/
	inline const Vector4 GetRotation() const 
	{
		if(this->zPhysicObj) 
			return this->zPhysicObj->GetRotationQuaternion(); 

		Vector4 def;
		return def;
	}

	void SetModelLength(const float length) {zModelLength = length;}
	/*! PhysicsObject needs to be initialized.*/
	inline void SetPosition(const Vector3& pos) 
	{
		if(this->zPhysicObj) 
			this->zPhysicObj->SetPosition(pos);
	}
	/*! PhysicsObject needs to be initialized.*/
	inline void SetRotation(const Vector4& rot) 
	{
		if(this->zPhysicObj) 
			this->zPhysicObj->SetQuaternion(rot);
	}
	/*! PhysicsObject needs to be initialized.*/
	void SetScale(const Vector3& scale) 
	{
		if(this->zPhysicObj) 
		{
			this->zPhysicObj->SetScaling(scale);
			this->zScaleChanged = true;
		}
	}
	void SetPhysicObject(PhysicsObject* pObj)
	{
		this->zPhysicObj = pObj;
	}
	/*! */
	PhysicsObject* GetPhysicObject() const {return this->zPhysicObj;}

	virtual void Update(float deltaTime) = 0;
	/*! Converts Pos, Rot, Scale to network message string.*/
	virtual std::string ToMessageString(NetworkMessageConverter* NMC);

protected:
	PhysicsObject* zPhysicObj;
	/*! The models initial model direction, which direction the model is pointing in model space.*/
	Vector3 zInitialDirection;

private:
	bool zScaleChanged;
	float zModelLength;
};