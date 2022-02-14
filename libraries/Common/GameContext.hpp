#ifndef GAME_CONTEXT_H
#define GAME_CONTEXT_H

#include "../GUI/GeneralGUI.hpp"

#include <vector>
#include <memory>

class GameContext{
public:
	static std::vector<std::shared_ptr<GameContext>> stack;
	
	std::shared_ptr<GameContext> clone(){
		return std::shared_ptr<GameContext>(new GameContext(*this));
	}

	GameContext(){
		id = 12;
		stack.push_back(clone());
		
		manager();
	}

	uint id;
};

std::vector<std::shared_ptr<GameContext>> GameContext::stack;

#endif