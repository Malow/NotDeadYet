/*
	Made by Jensen Christopher Datum(18/12/12 created) 
	for project Not Dead Yet at Blekinge tekniska h�gskola
*/

#pragma once

#include "Item.h"

class Food : public Item
{
public:
	Food(const long id, const unsigned int TYPE, const float hunger);
	virtual ~Food();
	/*! Decreases Stacks by 1.*/
	virtual bool Use();

	float GetHunger() const {return this->zHunger;}

	void SetHunger(const float hunger) {this->zHunger = hunger;}

private:
	float zHunger;
};