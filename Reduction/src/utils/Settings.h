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

constexpr double RED_PLAYER_START_X = (SCREEN_WIDTH / 2) - (SCREEN_HEIGHT / 2) + 40;
constexpr double RED_PLAYER_START_Y = SCREEN_HEIGHT / 2;
constexpr double RED_PLAYER_START_DIRECTION = 270;
constexpr double BLUE_PLAYER_START_X = (SCREEN_WIDTH / 2) + (SCREEN_HEIGHT / 2) - 40;
constexpr double BLUE_PLAYER_START_Y = SCREEN_HEIGHT / 2;
constexpr double BLUE_PLAYER_START_DIRECTION = 90;
constexpr double GREY_PLAYER_START_X = SCREEN_WIDTH / 2;
constexpr double GREY_PLAYER_START_Y = SCREEN_HEIGHT - 40;
constexpr double GREY_PLAYER_START_DIRECTION = 180;

constexpr double BULLET_SPEED = 500;
constexpr double BULLET_COOLDOWN = 250;
constexpr double BULLET_DIRECTION_OFFSET_MAX = 10;
constexpr double BULLET_KNOCKBACK = 10;

constexpr unsigned int LIFE_BAR_FULL_WIDTH = 150;
constexpr unsigned int LIFE_BAR_HEIGHT = 15;

constexpr double SPEED_POWERUP_COST = PLAYER_STARTING_LIFE * 0.15;
constexpr double SPEED_POWERUP_BOOST = MAX_PLAYER_SPEED;
constexpr double ACCURACY_POWERUP_COST = PLAYER_STARTING_LIFE * 0.15;
constexpr double ACCURACY_BULLET_OFFSET_MAX = 5;
constexpr double DRAG_REDUCTION = PLAYER_DRAG * 2 / 3;
constexpr double BULLET_POWERUP_COST = PLAYER_STARTING_LIFE * 0.15;
constexpr double BULLET_COOLDOWN_REDUCTION = BULLET_COOLDOWN * 0.2;
constexpr double BULLET_DAMAGE_POWERUP_COST = PLAYER_STARTING_LIFE * 0.15;
constexpr double BULLET_EXTRA_DAMAGE = PLAYER_HIT_DAMAGE * 0.25;

constexpr unsigned int SHORT_GAME_POINTS_TO_WIN = 3;
constexpr unsigned int MEDIUM_GAME_POINTS_TO_WIN = 5;
constexpr unsigned int LONG_GAME_POINTS_TO_WIN = 7;

constexpr int BARRIER_DISTANCE_FROM_CENTER = SCREEN_HEIGHT / 6;
constexpr int BARRIER_LENGTH = 70;
constexpr int BARRIER_WIDTH = 15;