#pragma once

constexpr int SCREEN_WIDTH = 960;
constexpr int SCREEN_HEIGHT = 540;

constexpr double WALL_SPEED = -0.000025;

constexpr double PLAYER_ROTATION_SPEED = 250;
constexpr double MAX_PLAYER_SPEED = 400;
constexpr double PLAYER_ACCELERATION = 200;
constexpr double PLAYER_DRAG = 30;
constexpr double PLAYER_STARTING_LIFE = 10000;
constexpr double PLAYER_HIT_DAMAGE = 2000;

constexpr double BULLET_SPEED = 500;
constexpr double BULLET_COOLDOWN = 250;

constexpr unsigned int LIFE_BAR_FULL_WIDTH = 150;
constexpr unsigned int LIFE_BAR_HEIGHT = 15;

constexpr double SPEED_POWERUP_COST = PLAYER_STARTING_LIFE * 0.15;
constexpr double SPEED_POWERUP_BOOST = MAX_PLAYER_SPEED;
constexpr double DRAG_REDUCTION = PLAYER_DRAG * 2 / 3;
constexpr double BULLET_POWERUP_COST = PLAYER_STARTING_LIFE * 0.15;
constexpr double BULLET_COOLDOWN_REDUCTION = BULLET_COOLDOWN * 0.2;
constexpr double BULLET_DAMAGE_POWERUP_COST = PLAYER_STARTING_LIFE * 0.15;
constexpr double BULLET_EXTRA_DAMAGE = PLAYER_HIT_DAMAGE * 0.25;
