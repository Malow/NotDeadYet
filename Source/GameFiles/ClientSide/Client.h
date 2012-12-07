/*
	Made by Jensen Christopher Datum(21/11/12 created) 
	for project Not Dead Yet at Blekinge tekniska h�gskola
*/
#pragma once
#include "Process.h"
#include "GraphicsEngine.h"
#include "Network/ServerChannel.h"
#include "GameFiles/ClientSide/Player.h"
#include "GameFiles/ClientSide/Animal.h"
#include "GameFiles/KeyUtil/KeyHandler.h"
#include "Network/NetworkMessageConverter.h"
#include "GameFiles/ClientSide/StaticObject.h"

using namespace MaloW;

class Client : public MaloW::Process
{
private:
	int zID;

	int	zPort;
	std::string zIP;

	INT64 zStartime;
	float zFrameTime;
	float zDeltaTime;
	float zSecsPerCnt;
	
	KeyHandler zKeyInfo;
	std::string zMeshID;
	GraphicsEngine* zEng;

	float zTimeSinceLastPing;

	std::vector<Player*> zPlayers;
	std::vector<Animal*> zAnimals;
	std::vector<StaticObject*> zStaticObjects;
	NetworkMessageConverter zMsgHandler;
	ServerChannel* zServerChannel;
private:
	/*! Handle Keyboard Input */
	void HandleKeyboardInput();
	/*! Splits Network Message and detects what type was sent*/
	void HandleNetworkMessage(std::string msg);
	/*! Pings client to check if server is still running*/
	void Ping();
	/*! Close the connection and print the reason to the client*/
	void CloseConnection(const std::string reason);
	/*! Initializes the graphic stuff*/
	void initClient();
	float Update();
	/*! Check if the key is pressed and potentially sends a message to the server*/
	bool CheckKey(const unsigned int ID);
	void UpdateWorldObjects();
	/*! Search for the object with the correct ID and returns a Position if found*/
	int SearchForPlayer(const int id);
	/*! Search for the object with the correct ID and returns a Position if found*/
	int SearchForAnimal(const int id);
	/*! Search for the object with the correct ID and returns a Position if found*/
	int SearchForObject(const int id);

	int FindObject(const int id, const unsigned int type);
	/*! Handles Message types*/
	void HandleUpdateObject(const std::vector<std::string>& msgArray, const unsigned int objectType);
	void HandleNewObject(const std::vector<std::string>& msgArray, const unsigned int objectType);
	void HandleRemoveObject(const std::vector<std::string>& msgArray, const unsigned int objectType);
public:
	Client();
	/*! Connects to a Host with the specified parameters*/
	int Connect(const std::string& ip, const int port);
	virtual ~Client();
	void Life();
	/*! Checks if Thread is alive*/
	bool IsAlive();
	void UpdateCameraPos();
};