//File: ServerEngineWrapper.cpp

#include "../Wrapper/ServerEngineWrapper.h"
#include "../EngineFiles/ServerEngine.h"

ServerEngineWrapper::ServerEngineWrapper()
{
	zServerEngine = new ServerEngine();
}

ServerEngineWrapper::~ServerEngineWrapper()
{
	SAFE_DELETE(zServerEngine);
}

char* ServerEngineWrapper::Init(const int port, const int maxPlayers) const
{
	return zServerEngine->InitHost(port, maxPlayers);
}

void ServerEngineWrapper::ShutdownHost() const
{
	this->zServerEngine->ShutDownHost();
}

bool ServerEngineWrapper::StartHost() const
{
	return this->zServerEngine->StartHost();
}

int ServerEngineWrapper::GetNrOfPlayers() const
{
	return zServerEngine->GetNrOfPlayers();
}

char* ServerEngineWrapper::ProcessText(char* msg)
{
	//return zServerEngine->ProcessText(msg);
	return NULL;
}

bool ServerEngineWrapper::IsRunning() const
{
	return zServerEngine->IsRunning();
}

