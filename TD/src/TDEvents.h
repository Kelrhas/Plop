#pragma once

#include <Events/Event.h>

enum class GameEventType : S32
{
	EnemyKilled,
	LifeLost,
	GameOver
};