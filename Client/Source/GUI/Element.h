#pragma once
/**
* Author: Rikard Johansson
* Create Date: 28/03/2012
* 
* This abstract class will be used as a base of every GUI element
**/

#include "iImage.h"
#include <string>
#include "GUIEvent.h"
#include "NoEvent.h"
#include "ChangeTextAndMenuEvent.h"
#include "GraphicsEngine.h"

using namespace std;

class Element
{
private:
	float mX;
	float mY;
	float mZ;
	
	float mWidth;
	float mHeight;
	
	string mTextureName;

	iImage* mImage;

public:
	Element();
	Element(float x, float y, float z, string textureName, float width, float height);
	/*Element(Element& origObj);*/
	virtual ~Element();

	/*! Sets the position of the element*/
	void SetPosition(float x, float y, float z){ this->mX = x; this->mY = y;this->mZ = z; }
	/*! Saves the position of the element in the parameters*/
	void GetPosition(float& x, float& y, float& z){ x = this->mX; y = this->mY; z = this->mZ; }
	/*! Returns the pos as a D3DXVECTOR2*/
	Vector2 GetPositionD3D(){ return this->mImage->GetPosition(); }
	void SetPositionD3D(Vector2 pos){ this->mImage->SetPosition(pos); this->mX = pos.x; this->mY = pos.y; }

	/*! Sets the width of the element*/
	void SetWidth(float width){ this->mWidth = width; }
	/*! Saves the width of the element in parameter*/
	void GetWidth(float& width){ width = this->mWidth; }

	/*! Set the height of the element*/
	void SetHeight(float height){ this->mHeight = height; }
	/*! Saves the height of the element in parameter*/
	void GetHeight(float& height){ height = this->mHeight; }
	
	/*! Returns the dimension of a D3DXVECTOR2*/
	Vector2 GetDimension(){ return Vector2(this->mWidth, this->mHeight); }

	/*! Sets the texture variable*/
	void SetTextureName(string textureName){ this->mTextureName = textureName; }
	/*! Saves the texture name in parameter*/
	void GetTextureName(string& textureName){ textureName = this->mTextureName; }
	
	/*! Sets the image*/
	void SetImage(iImage* image){ this->mImage = image; }
	/*! Saves the image in the parameter*/
	iImage* GetImage(){ return this->mImage; }

	/*! Adds the element to the renderer*/
	virtual bool AddToRenderer(GraphicsEngine* ge);
	/*! Removes the element from the renderer*/
	virtual bool RemoveFromRenderer(GraphicsEngine* ge);
};