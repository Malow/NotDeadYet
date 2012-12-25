#include "GameFiles/ServerSide/Host.h"


Host::Host()
{
	this->zServerListener = NULL;
	this->zMaxClients = 10;
	this->zClients = std::vector<ClientData *>(); 
	this->zActorHandler = new ActorHandler();

	this->zStartime = 0;
	this->zSecsPerCnt = 0.0f;
	this->zDeltaTime = 0.0f;
	this->zTimeOut = 15.0f;
	this->zPingMessageInterval = 5.0f;
	this->zUpdateDelay = 0.05f;
}

Host::~Host()
{
	//Sends to all clients, the server is hutting down.
	BroadCastServerShutdown();

	this->Close();
	this->WaitUntillDone();

	this->zServerListener->Close();
	SAFE_DELETE(this->zServerListener);
	SAFE_DELETE(this->zActorHandler);

	for(auto x = zClients.begin(); x < zClients.end(); x++)
	{
		SAFE_DELETE(*x);
	}
}

void Host::Life()
{
	MaloW::Debug("Host Process Started");
	this->zServerListener->Start();
	
	INT64 frequency;
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
	
	this->zSecsPerCnt = 1.0f / (float)(frequency);

	QueryPerformanceCounter((LARGE_INTEGER*)&this->zStartime);

	static float waitTimer = 0.0f;
	
	while(this->stayAlive)
	{
		waitTimer += Update();

		//Checks if ServerListener is still working
		if(!this->zServerListener->IsAlive())
		{
			MaloW::Debug("Server Listener has died.");
		}

		HandleNewConnections();
		ReadMessages();
		HandleRecivedMessages();
		PingClients();
		UpdatePl();
		
		if(waitTimer >= this->zUpdateDelay)
		{
			SendPlayerUpdates();
			waitTimer = 0.0f;
		}

		Sleep(5);
	}
}

int Host::InitHost( int port, unsigned int maxClients )
{
	int code = 0;

	if(!this->zServerListener)
		this->zServerListener = new ServerListener();
	else
	{
		SAFE_DELETE(this->zServerListener);
		this->zServerListener = new ServerListener();
	}
	
	code = this->zServerListener->InitListener(port);

	this->zMaxClients = maxClients;
	this->zPort	= port;
	
	return code;
}

void Host::HandleNewConnections()
{
	MaloW::ProcessEvent* pe; 
	pe = this->zServerListener->PeekEvent();

	if(!pe)
		return;

	ClientConnectedEvent* cce = dynamic_cast<ClientConnectedEvent*>(pe);
	if(!cce)
	{
		SAFE_DELETE(pe);
		return;
	}

	MaloW::Debug("New Player Connected.");

	if((unsigned int)this->zClients.size() > zMaxClients)
	{
		std::string message;

		message = zMessageConverter.Convert(MESSAGE_TYPE_SERVER_FULL);

		cce->GetClientChannel()->sendData(message);
		SAFE_DELETE(pe);

		return;
	}

	MaloW::Debug("New Player Accepted.");

	std::string message = "";

	MaloW::ClientChannel* client = cce->GetClientChannel();

	client->setNotifier(this);
	this->zClients.push_back(new ClientData(client));

	message = this->zMessageConverter.Convert(MESSAGE_TYPE_SELF_ID, client->getClientID());
	client->sendData(message);
	client->Start();

	SAFE_DELETE(pe);
}

void Host::SendToAllClients( const std::string& message )
{
	if(!HasPlayers())
		return;

	std::vector<ClientData*>::iterator it;

	for(it = zClients.begin(); it < zClients.end(); it++)
	{
		(*it)->zClient->sendData(message);
	}
}

void Host::SendToClient( int clientID, const std::string& message )
{
	int pos = SearchForClient(clientID);

	if(pos == -1)
		return;

	this->zClients[pos]->zClient->sendData(message);
}

void Host::SendPlayerUpdates()
{
	if(!HasPlayers())
		return;

	std::vector<std::string> playerData;
	std::string mess = "";

	//Fetch player data
	std::vector<PlayerActor*> pl = this->zActorHandler->GetPlayers();
	for (auto it_Player = pl.begin(); it_Player < pl.end(); it_Player++)
	{
		Vector3 pos = (*it_Player)->GetPosition();
		Vector4 rot = (*it_Player)->GetRotation();

		mess =  this->zMessageConverter.Convert(MESSAGE_TYPE_UPDATE_PLAYER, (*it_Player)->GetID());
		mess += this->zMessageConverter.Convert(MESSAGE_TYPE_FRAME_TIME, (*it_Player)->GetFrameTime());
		mess += this->zMessageConverter.Convert(MESSAGE_TYPE_POSITION, pos.x, pos.y, pos.z);
		mess += this->zMessageConverter.Convert(MESSAGE_TYPE_ROTATION, rot.x, rot.y, rot.z, rot.w);
		mess += this->zMessageConverter.Convert(MESSAGE_TYPE_STATE, (*it_Player)->GetState());

		playerData.push_back(mess);
	}

	//Send Data to clients
	for (auto it_Client = zClients.begin(); it_Client < zClients.end(); it_Client++)
	{
		for (auto it_Message = playerData.begin(); it_Message < playerData.end(); it_Message++)
		{
			(*it_Client)->zClient->sendData((*it_Message));
		}
	}
}

void Host::SendAnimalUpdates()
{
	if(!HasPlayers())
		return;

	std::vector<std::string> animalData;
	std::string mess = "";

	//Fetch player data
	std::vector<AnimalActor*> pl = this->zActorHandler->GetAnimals();
	for (auto it_Animal = pl.begin(); it_Animal < pl.end(); it_Animal++)
	{
		Vector3 pos = (*it_Animal)->GetPosition();
		Vector3 dir = (*it_Animal)->GetDirection();
		Vector4 rot = (*it_Animal)->GetRotation();

		mess =  this->zMessageConverter.Convert(MESSAGE_TYPE_UPDATE_ANIMAL, (*it_Animal)->GetID());
		mess += this->zMessageConverter.Convert(MESSAGE_TYPE_POSITION, pos.x, pos.y, pos.z);
		mess += this->zMessageConverter.Convert(MESSAGE_TYPE_ROTATION, rot.x, rot.y, rot.z, rot.w);
		mess += this->zMessageConverter.Convert(MESSAGE_TYPE_STATE, (*it_Animal)->GetState());

		animalData.push_back(mess);
	}

	//Send Data to clients
	for (auto it_Client = zClients.begin(); it_Client < zClients.end(); it_Client++)
	{
		for (auto it_Message = animalData.begin(); it_Message < animalData.end(); it_Message++)
		{
			(*it_Client)->zClient->sendData((*it_Message));
		}
	}
}

bool Host::HasPlayers() const
{
	return !this->zClients.empty();
}

void Host::ReadMessages()
{
	static unsigned int MAX_MESSAGES_TO_READ = 10;
	unsigned int nrOfMessages = this->GetEventQueueSize();

	//No new messsages
	if(nrOfMessages == 0)
		return;

	nrOfMessages = min(nrOfMessages, MAX_MESSAGES_TO_READ);
	MaloW::ProcessEvent* pe;

	for (unsigned int i = 0; i < nrOfMessages; i++)
	{
		pe = PeekEvent();

		if(pe)
		{
			MaloW::NetworkPacket* np = dynamic_cast<MaloW::NetworkPacket*>(pe);
			if(np)
				this->zMessages.push_back(np);
			else
				SAFE_DELETE(pe);
		}
		
	}
}

void Host::HandleRecivedMessages()
{
	
	if(this->zMessages.empty())
		return;

	int c_index;
	PlayerActor* p_actor = NULL; 

	for (auto it = this->zMessages.begin(); it < this->zMessages.end(); it++)
	{
		//If null, skip and continue
		if(!(*it))
			continue;
		
		std::vector<std::string> msgArray;
		msgArray = this->zMessageConverter.SplitMessage((*it)->getMessage()); 

		//If empty, skip and continue
		if(msgArray.empty())
			continue;

		char key[128];
		sscanf_s(msgArray[0].c_str(), "%s ", &key, sizeof(key));
		
		c_index = SearchForClient((*it)->getID());
		p_actor = dynamic_cast<PlayerActor*>(this->zActorHandler->GetActor((*it)->getID(), ACTOR_TYPE_PLAYER));

		//Handles updates from client.
		if(strcmp(key, M_CLIENT_DATA.c_str()) == 0 && (p_actor))
		{
			HandlePlayerUpdate(p_actor, this->zClients[c_index], msgArray);
		}
		//Handles key presses from client.
		else if(strcmp(key, M_KEY_DOWN.c_str()) == 0 && (p_actor))
		{
			HandleKeyPress(p_actor, msgArray[0]);
		}
		//Handles key releases from client.
		else if(strcmp(key, M_KEY_UP.c_str()) == 0 && (p_actor))
		{
			HandleKeyRelease(p_actor, msgArray[0]);
		}
		//Handles Pings from client.
		else if(strcmp(key, M_PING.c_str()) == 0 && (c_index != -1))
		{
			HandlePingMsg(this->zClients[c_index]);
		}
		//Handles user data from client. Used when the player is new.
		else if(strcmp(key, M_USER_DATA.c_str()) == 0 && (c_index != -1))
		{
			CreateNewPlayer(this->zClients[c_index], msgArray);
		}
		//Handles if client disconnects.
		else if(strcmp(key, M_CONNECTION_CLOSED.c_str()) == 0)
		{
			KickClient((*it)->getID());
		}
		//Handles if not of the above.
		else
		{
			MaloW::Debug("Warning: The host cannot handle the message \""+(*it)->getMessage()+"\" in HandleRecivedMessages.");
		}

		SAFE_DELETE((*it));
	}
}

void Host::HandleKeyPress( PlayerActor* pl, const std::string& key )
{
	//Hard coded for test
	int keyz = this->zMessageConverter.ConvertStringToInt(M_KEY_DOWN, key);

	switch (keyz)
	{
	case KEY_FORWARD:
		pl->SetKeyState(KEY_FORWARD, true);
		break;
	case KEY_BACKWARD:
		pl->SetKeyState(KEY_BACKWARD, true);
		break;
	case KEY_LEFT:
		pl->SetKeyState(KEY_LEFT, true);
		break;
	case KEY_RIGHT:
		pl->SetKeyState(KEY_RIGHT, true);
		break;
	case KEY_SPRINT:
		pl->SetKeyState(KEY_SPRINT, true);
		break;
	case KEY_DUCK:
		pl->SetKeyState(KEY_DUCK, true);
		break;
	case KEY_JUMP:
		pl->SetKeyState(KEY_JUMP, true);
		break;
	default:
		break;
	}

}

void Host::HandleKeyRelease( PlayerActor* pl, const std::string& key )
{
	//Hard coded for test
	int keyz = this->zMessageConverter.ConvertStringToInt(M_KEY_UP, key);

	switch (keyz)
	{
	case KEY_FORWARD:
		pl->SetKeyState(KEY_FORWARD, false);
		break;
	case KEY_BACKWARD:
		pl->SetKeyState(KEY_BACKWARD, false);
		break;
	case KEY_LEFT:
		pl->SetKeyState(KEY_LEFT, false);
		break;
	case KEY_RIGHT:
		pl->SetKeyState(KEY_RIGHT, false);
		break;
	case KEY_SPRINT:
		pl->SetKeyState(KEY_SPRINT, false);
		break;
	case KEY_DUCK:
		pl->SetKeyState(KEY_DUCK, false);
		break;
	case KEY_JUMP:
		pl->SetKeyState(KEY_JUMP, false);
		break;
	default:
		break;
	}
}

void Host::HandlePingMsg( ClientData* cd )
{
	//Hard coded
	if(cd->zTotalPingTime > 60.0f)
		cd->ResetPingCounter();

	cd->zTotalPingTime += cd->zCurrentPingTime;
	cd->zNrOfPings++;

	cd->zPinged = false;
	cd->zCurrentPingTime = 0.0f;
}

void Host::HandlePlayerUpdate( PlayerActor* pl, ClientData* cd, const std::vector<std::string> &data )
{
	
	for(auto it = data.begin() + 1; it < data.end(); it++)
	{
		char key[512];
		sscanf_s((*it).c_str(), "%s ", &key, sizeof(key));
		
		if(strcmp(key, M_DIRECTION.c_str()) == 0)
		{
			Vector3 dir = this->zMessageConverter.ConvertStringToVector(M_DIRECTION, (*it));
			pl->SetDirection(dir);
		}
		else if(strcmp(key, M_UP.c_str()) == 0)
		{
			Vector3 up = this->zMessageConverter.ConvertStringToVector(M_UP, (*it));
			pl->SetUpVector(up);
		}
		else if(strcmp(key, M_ROTATION.c_str()) == 0)
		{
			Vector4 rot = this->zMessageConverter.ConvertStringToQuaternion(M_ROTATION, (*it));
			pl->SetRotation(rot);
		}
		else if(strcmp(key, M_FRAME_TIME.c_str()) == 0)
		{
				float frameTime = this->zMessageConverter.ConvertStringToFloat(M_FRAME_TIME, (*it));
				pl->SetFrameTime(frameTime);
		}
		else
			MaloW::Debug("Unknown message in HandlePlayerUpdate.");
	}

	//Update Latency
	if( cd->zNrOfPings == 0)
		return;

	float latency = cd->zTotalPingTime / cd->zNrOfPings;
	pl->SetLatency(latency);
}

int Host::SearchForClient( const int ID ) const
{

	if(!HasPlayers())
		return -1;

	for (unsigned int i = 0; i < this->zClients.size(); i++)
	{
		if(this->zClients.at(i)->zClient->getClientID() == ID)
		{
			return i;
		}
	}

	return -1;
}

void Host::BroadCastServerShutdown()
{
	std::string mess = this->zMessageConverter.Convert(MESSAGE_TYPE_SERVER_SHUTDOWN);
	SendToAllClients(mess);
}

void Host::PingClients()
{
	if(!HasPlayers())
		return;

	ClientData* cd; 

	for(unsigned int i = 0; i < (unsigned int)zClients.size(); i++)
	{
		cd = zClients.at(i);

		//If client has not been pinged.
		if(!cd->zPinged)
		{
			//If it was x sec ago we sent a ping, don't send a ping.
			if(cd->zCurrentPingTime < zPingMessageInterval)
				cd->IncPingTime(zDeltaTime);

			//else send ping.
			else
			{
				cd->zCurrentPingTime = 0.0f;
				cd->zClient->sendData(this->zMessageConverter.Convert(MESSAGE_TYPE_PING));
				cd->zPinged = true;
			}
		}
		//If he have sent a ping.
		else
		{
			//If we sent a ping x sec ago, drop the client.
			if(cd->zCurrentPingTime > zTimeOut)
			{
				KickClient(cd->zClient->getClientID());
			}
			else
				cd->IncPingTime(zDeltaTime);

		}
	}
}

float Host::Update()
{
	//Update Timer
	INT64 currentTime;
	float timeDifference;

	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	timeDifference = (float)(currentTime - this->zStartime);

	this->zDeltaTime = timeDifference * this->zSecsPerCnt;

	this->zStartime = currentTime;

	return this->zDeltaTime;
}

void Host::UpdatePl()
{
	this->zActorHandler->UpdatePl(zDeltaTime);	
}

bool Host::KickClient( const int ID, bool sendAMessage /*= false*/, std::string reason /*= ""*/ )
{

	int index = SearchForClient(ID);

	ClientData* temp_c = zClients.at(index);

	std::string mess;
	bool removed = false;

	if(index == -1)
	{
		MaloW::Debug("Can't find client to kick in KickClient in Host.cpp.");
		return false;
	}

	if(sendAMessage)
	{
		mess = this->zMessageConverter.Convert(MESSAGE_TYPE_KICKED, reason);

		temp_c->zClient->sendData(mess);
	}

	//create a remove player message.
	mess = this->zMessageConverter.Convert(MESSAGE_TYPE_REMOVE_PLAYER, ID);

	//remove the player


	PlayerActor* temp_p = NULL;
	this->zClients.erase(zClients.begin() + index);
	temp_p = dynamic_cast<PlayerActor*>(this->zActorHandler->RemovePlayerActor(ID));

	SAFE_DELETE(temp_c);
	SAFE_DELETE(temp_p);

	removed = true;
	MaloW::Debug("Client"+MaloW::convertNrToString(ID)+" removed from server.");
	

	//Notify clients
	this->SendToAllClients(mess);

	return removed;
}

bool Host::IsAlive() const
{
	return this->stayAlive;
}

void Host::CreateNewPlayer(ClientData* cd, const std::vector<std::string> &data )
{
	std::string mess;
	std::string uModel;
	Vector3 uDir;
	Vector3 uUp;


	PlayerActor* pi = new PlayerActor(cd->zClient->getClientID());

	for (auto it_m = data.begin() + 1; it_m < data.end(); it_m++)
	{
		char key[512];
		sscanf_s((*it_m).c_str(), "%s ", &key, sizeof(key));

		if(strcmp(key, M_MESH_MODEL.c_str()) == 0)
		{
			uModel = this->zMessageConverter.ConvertStringToSubstring(M_MESH_MODEL, (*it_m));
		}
		else if(strcmp(key, M_DIRECTION.c_str()) == 0)
		{
			uDir = this->zMessageConverter.ConvertStringToVector(M_DIRECTION, (*it_m));
		}
		else if(strcmp(key, M_UP.c_str()) == 0)
		{
			uUp = this->zMessageConverter.ConvertStringToVector(M_UP, (*it_m));
		}
	}

	if(uModel != "")
		pi->SetActorModel(uModel);

	pi->SetUpVector(uUp);
	pi->SetDirection(uDir);
	//Debug Pos
	pi->SetPosition(Vector3(pi->GetID()*25,0,1)); 
	this->zActorHandler->AddNewPlayer(pi);

	//Collect player infos
	std::vector<std::string> temp;
	std::vector<PlayerActor *> players = this->zActorHandler->GetPlayers();
	int count = 0;
	int newPlayerindex = 0;

	for(auto it = players.begin(); it < players.end(); it++)
	{
		Vector3 pos = (*it)->GetPosition();
		Vector3 scale = (*it)->GetScale();
		Vector3 dir = (*it)->GetDirection();
		Vector4 rot = (*it)->GetRotation();

		mess =  this->zMessageConverter.Convert(MESSAGE_TYPE_NEW_PLAYER, (*it)->GetID());
		mess += this->zMessageConverter.Convert(MESSAGE_TYPE_POSITION, pos.x, pos.y, pos.z);
		mess += this->zMessageConverter.Convert(MESSAGE_TYPE_SCALE, scale.x, scale.y, scale.z);
		mess += this->zMessageConverter.Convert(MESSAGE_TYPE_ROTATION, rot.x, rot.y, rot.z, rot.w);
		mess += this->zMessageConverter.Convert(MESSAGE_TYPE_MESH_MODEL, (*it)->GetActorModel());
		mess += this->zMessageConverter.Convert(MESSAGE_TYPE_STATE, (*it)->GetState());
		//mess += this->zMessageConverter.Convert(MESSAGE_TYPE_DIRECTION, dir.x, dir.y, dir.z);

		temp.push_back(mess);

		if((*it)->GetID() == pi->GetID())
			newPlayerindex = count;

		count++;
	}

	//Send players to new player
	MaloW::ClientChannel* cc = cd->zClient;

	std::vector<std::string>::iterator sIt;
	for (sIt = temp.begin(); sIt < temp.end(); sIt++)
	{
		cc->sendData(*sIt);
	}

	//Send new player to players
	for (unsigned int i = 0; i < (unsigned int)this->zClients.size(); i++)
	{
		if(zClients[i]->zClient->getClientID() != cc->getClientID())
			this->zClients[i]->zClient->sendData(temp[newPlayerindex]);
	}
}