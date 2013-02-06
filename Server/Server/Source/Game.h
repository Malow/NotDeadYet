#pragma once

#include <Observer.h>
#include <map>
#include <KeyStates.h>
#include "ActorManager.h"
#include "ActorSynchronizer.h"
#include "PhysicsEngine.h"

class ClientData;
class World;
class GameMode;
class Player;
class Behavior;
class ActorSynchronizer;
class KeyStates;

class Game : public Observer, public Observed
{
	ActorManager* zActorManager;
	GameMode* zGameMode;
	World* zWorld;

	std::map<ClientData*, Player*> zPlayers;
	std::map<Player*, Behavior*> zPlayerBehaviors;
	std::map<Player*, KeyStates*> zKeyStates;
	std::set<Behavior*> zBehaviors;

public:
	Game(ActorSynchronizer* syncher, GameMode* mode, const std::string& worldFile);
	virtual ~Game();

	// Returns false if game has finished
	bool Update( float dt );

	// Event input
	void OnEvent( Event* e );

	// Sets The Behavior Of Player
	void SetPlayerBehavior( Player* player, PlayerBehavior* behavior );

private:
	PhysicsEngine* zPhysicsEngine;
};