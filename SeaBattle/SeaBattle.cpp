#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <thread>
#include <chrono>
#include <random>
#include <strstream>
#include <string>

const int MAP_SIZE = 11;
const int MILLISECONDS_PAUSE = 500;

std::vector<int> ships;
std::map<std::string, std::string> map_marks;

int difficulty;

int player_map[MAP_SIZE + 1][MAP_SIZE + 1];
int enemy_map[MAP_SIZE + 1][MAP_SIZE + 1];

void pre_start();
void start();

void update();
std::pair<bool, std::string> get_player_turn();
std::pair<bool, std::string> get_enemy_turn();

void draw_map(int map[MAP_SIZE + 1][MAP_SIZE + 1], bool ships_hidden = false);

void choose_difficulty();
void get_player_ships_placement();
void generate_enemy_ships();

bool check_alive(int map[MAP_SIZE + 1][MAP_SIZE + 1]);

int main()
{
	std::pair<bool, std::string> player_turn_info;
	std::pair<bool, std::string> enemy_turn_info;

	std::thread background_tasks_thread([]()
		{
			generate_enemy_ships();
		});

	pre_start();
	start();

	background_tasks_thread.join();

	while (true)
	{
		update();
		player_turn_info = get_player_turn();

		if (player_turn_info.first)
		{
			std::cout << player_turn_info.second;
			std::cout << "\n\n";
			std::cout << "Now the enemy's turn...";

			std::this_thread::sleep_for(std::chrono::milliseconds(MILLISECONDS_PAUSE));

			enemy_turn_info = get_enemy_turn();

			if (enemy_turn_info.first)
			{
				std::cout << "\n";
				std::cout << enemy_turn_info.second;
				std::cout << "\n\n";
				std::cout << "Now your turn...";

				std::this_thread::sleep_for(std::chrono::milliseconds(MILLISECONDS_PAUSE));
			}
		}
		else
		{
			std::cout << "\n";
			std::cout << player_turn_info.second;

			std::this_thread::sleep_for(std::chrono::milliseconds(MILLISECONDS_PAUSE));
		}
	}

	return 0;
}

void pre_start()
{
	int initial_code = 64;

	for (int i = 0; i < MAP_SIZE; i++)
	{
		player_map[0][i] = (initial_code) * 100 * 1000;
		player_map[MAP_SIZE][i] = (initial_code) * 100 * 1000;
		player_map[i][0] = (initial_code) * 100 * 1000;
		player_map[i][MAP_SIZE] = (initial_code) * 100 * 1000;

		enemy_map[0][i] = (initial_code) * 100 * 1000;
		enemy_map[MAP_SIZE][i] = (initial_code) * 100 * 1000;
		enemy_map[i][0] = (initial_code) * 100 * 1000;
		enemy_map[i][MAP_SIZE] = (initial_code) * 100 * 1000;

		initial_code++;
	}

	player_map[11][11] = (initial_code) * 100 * 1000;

	ships.push_back(4);
	ships.push_back(3);
	ships.push_back(3);
	ships.push_back(2);
	ships.push_back(2);
	ships.push_back(2);
	ships.push_back(1);
	ships.push_back(1);
	ships.push_back(1);
	ships.push_back(1);

	map_marks.insert(std::pair<std::string, std::string>("WATER", "_"));
	map_marks.insert(std::pair<std::string, std::string>("SHIP", "o"));
	map_marks.insert(std::pair<std::string, std::string>("DEAD", "x"));
}

void start()
{
	choose_difficulty();
	//get_player_ships_placement();
}

void update()
{
	system("cls");

	draw_map(player_map);

	std::cout << "\n";

	if (!check_alive(enemy_map))
	{
		draw_map(enemy_map);

		std::cout << "\n";
		std::cout << "You winner!\n";

		exit(0);
	}
	else if (!check_alive(player_map))
	{
		draw_map(enemy_map);

		std::cout << "\n";
		std::cout << "You loser!\n";

		exit(0);
	}

	draw_map(enemy_map);

}

std::pair<bool, std::string> get_player_turn()
{
	std::string input;

	int ship_position_x;
	int ship_position_y;

	while (true)
	{
		std::cout << "\n";

		std::cout << "Enter coordinates to attack: ";

		std::getline(std::cin, input);

		ship_position_x = static_cast<int>(input[0]) - 65;
		ship_position_y = static_cast<int>(input[1]) - 48;

		if (std::empty(input) || input.length() != 2 || !isalpha(input[0]) || ship_position_x < 0 || ship_position_x > 9 || !isdigit(input[1]) || ship_position_y < 0 || ship_position_y > 9)
		{
			return std::pair<bool, std::string>(false, "Incorrect coordinates!");
		}

		break;
	}

	if (enemy_map[ship_position_y][ship_position_x] > 0)
	{
		enemy_map[ship_position_y][ship_position_x] = -1;

		return std::pair<bool, std::string>(true, "You hit an enemy's ship!");
	}

	return std::pair<bool, std::string>(true, "You missed!");
}

std::pair<bool, std::string> get_enemy_turn()
{
	std::random_device random_device;
	std::mt19937 generator(random_device());
	std::uniform_int_distribution<> distribution(1, 10);

	int attempts = difficulty;

	for (int i = 0; i < attempts; i++)
	{
		int attack_position_x = distribution(generator);
		int attack_position_y = distribution(generator);

		if (player_map[attack_position_y][attack_position_x] > 0)
		{
			player_map[attack_position_y][attack_position_x] = -1;

			return std::pair<bool, std::string>(true, "Enemy hit you ship!");
		}
	}

	return std::pair<bool, std::string>(true, "Enemy missed!");
}

void draw_map(int map[MAP_SIZE + 1][MAP_SIZE + 1], bool ships_hidden)
{
	if (ships_hidden)
	{
		for (int i = 0; i < MAP_SIZE; i++)
		{
			for (int j = 0; j < MAP_SIZE; j++)
			{
				if (!i && !j)
				{
					std::cout << "  ";
				}
				else if (!j)
				{
					std::cout << i - 1 << " ";
				}
				else if (map[i][j] > 100 * 1000)
				{
					std::cout << (char)(map[i][j] / 100 / 1000) << ' ';
				}
				else if (map[i][j] > 0)
				{
					std::cout << map_marks["WATER"] << " ";
				}
				else if (map[i][j] < 0)
				{
					std::cout << map_marks["DEAD"] << " ";
				}
				else
				{
					std::cout << map_marks["WATER"] << " ";
				}
			}

			std::cout << std::endl;
		}
	}
	else
	{
		for (int i = 0; i < MAP_SIZE; i++)
		{
			for (int j = 0; j < MAP_SIZE; j++)
			{
				if (!i && !j)
				{
					std::cout << "  ";
				}
				else if (!j)
				{
					std::cout << i - 1 << " ";
				}
				else if (map[i][j] > 100 * 1000)
				{
					std::cout << (char)(map[i][j] / 100 / 1000) << ' ';
				}
				else if (map[i][j] > 0)
				{
					std::cout << map_marks["SHIP"] << " ";
				}
				else if (map[i][j] < 0)
				{
					std::cout << map_marks["DEAD"] << " ";
				}
				else
				{
					std::cout << map_marks["WATER"] << " ";
				}
			}

			std::cout << std::endl;
		}
	}
}

void choose_difficulty()
{
	std::string input;

	while (true)
	{
	while_mark:
		system("cls");

		std::cout << "Enter game difficulty: \n";
		std::cout << "1 - Easy\n";
		std::cout << "2 - Medium\n";
		std::cout << "3 - Hard\n\n";
		std::cout << "Your choice: ";

		std::getline(std::cin, input);

		if (std::empty(input) || input.length() != 1 || !isdigit(input[0]))
		{
			continue;
		}

		break;
	}

	difficulty = std::stoi(input);

	if (difficulty < 1 || difficulty > 3)
	{
		goto while_mark;
	}

	std::cout << "Game difficulty - " << difficulty;

	std::this_thread::sleep_for(std::chrono::milliseconds(MILLISECONDS_PAUSE));
}

void get_player_ships_placement()
{
	std::string input;
	std::string str_helper;

	int ship_placement;

	int x;
	int y;

	int current_ship_length;

	for (size_t i = 0; i < ships.size(); i++)
	{
		current_ship_length = ships[i];

		while (true)
		{
		while_mark_1:
			system("cls");

			str_helper = "";

			for (int i = 0; i < current_ship_length; i++)
			{
				str_helper += "x";
			}

			draw_map(player_map);

			std::cout << "\n";
			std::cout << "Place one " << str_helper << "-ship:\n";
			std::cout << "1 - Horizontal\n";
			std::cout << "2 - Vertical\n\n";
			std::cout << "Your choice: ";

			std::getline(std::cin, input);

			if (std::empty(input) || input.length() != 1 || !isdigit(input[0]))
			{
				continue;
			}

			break;
		}

		ship_placement = std::stoi(input);

		if (ship_placement < 1 || ship_placement > 2)
		{
			goto while_mark_1;
		}

		std::cout << "Ship placement - " << ship_placement;

		std::this_thread::sleep_for(std::chrono::milliseconds(MILLISECONDS_PAUSE));

		int temp_table[MAP_SIZE + 1][MAP_SIZE + 1] = {};

		int ship_code = 0;

		while (true)
		{
		while_mark_2:
			system("cls");

			draw_map(player_map);

			std::cout << "\n";
			std::cout << "Ship placement - " << ship_placement << "\n";
			std::cout << "Enter ship coordinates: ";

			std::getline(std::cin, input);

			x = (int)input[0] - 65;
			y = (int)input[1] - 48;

			x++;
			y++;

			if (std::empty(input) || input.length() != 2 || !isalpha(input[0]) || x < 0 || x > MAP_SIZE - 1 || !isdigit(input[1]) || y < 0 || y > MAP_SIZE - 1)
			{
				continue;
			}

			break;
		}

		std::cout << "\n";

		for (int i = 0; i < MAP_SIZE + 1; i++)
		{
			for (int j = 0; j < MAP_SIZE + 1; j++)
			{
				temp_table[i][j] = player_map[i][j];
			}
		}

		if (ship_placement == 1)
		{
			for (int i = 0; i < current_ship_length; i++)
			{
				for (int j = y - 1; j < y + 2; j++)
				{
					for (int k = x - 1; k < x + 2; k++)
					{
						if (temp_table[j][k + i] != 0 && temp_table[j][k + i] != current_ship_length * 2 + ship_code && temp_table[j][k + i] < 100 * 1000)
						{
							std::cout << "Incorrect coordinates!\n";

							std::this_thread::sleep_for(std::chrono::milliseconds(MILLISECONDS_PAUSE));

							goto while_mark_2;
						}
					}
				}

				temp_table[y][x + i] = current_ship_length * 2 + ship_code;
			}
		}
		else
		{
			for (int i = 0; i < current_ship_length; i++)
			{
				for (int j = y - 1; j < y + 2; j++)
				{
					for (int k = x - 1; k < x + 2; k++)
					{
						if (temp_table[j + i][k] != 0 && temp_table[j + i][k] != current_ship_length * 2 + ship_code && temp_table[j + i][k] < 100 * 1000)
						{
							std::cout << "Incorrect coordinates!\n";

							std::this_thread::sleep_for(std::chrono::milliseconds(MILLISECONDS_PAUSE));

							goto while_mark_2;
						}
					}
				}

				temp_table[y + i][x] = current_ship_length * 2 + ship_code;
			}
		}

		ship_code++;

		for (int i = 0; i < MAP_SIZE + 1; i++)
		{
			for (int j = 0; j < MAP_SIZE + 1; j++)
			{
				player_map[i][j] = temp_table[i][j];
			}
		}
	}
}

void generate_enemy_ships()
{
	std::random_device random_device;
	std::mt19937 generator(random_device());

	int ship_placement;

	int x;
	int y;

	int current_ship_length;

	for (size_t i = 0; i < ships.size(); i++)
	{
		current_ship_length = ships[i];

		std::uniform_int_distribution<> distribution(1, 2);

		ship_placement = distribution(generator);

		int temp_table[MAP_SIZE + 1][MAP_SIZE + 1] = {};

		int ship_code = 0;

		while (true)
		{
		while_mark:
			std::uniform_int_distribution<> dis(1, 10);

			x = dis(generator);
			y = dis(generator);

			break;
		}

		for (int i = 0; i < MAP_SIZE + 1; i++)
		{
			for (int j = 0; j < MAP_SIZE + 1; j++)
			{
				temp_table[i][j] = enemy_map[i][j];
			}
		}

		if (ship_placement == 1)
		{
			for (int i = 0; i < current_ship_length; i++)
			{
				for (int j = y - 1; j < y + 2; j++)
				{
					for (int k = x - 1; k < x + 2; k++)
					{
						if (temp_table[j][k + i] != 0 && temp_table[j][k + i] != current_ship_length * 2 + ship_code && temp_table[j][k + i] < 100 * 1000)
						{
							goto while_mark;
						}
					}
				}

				temp_table[y][x + i] = current_ship_length * 2 + ship_code;
			}
		}
		else
		{
			for (int i = 0; i < current_ship_length; i++)
			{
				for (int j = y - 1; j < y + 2; j++)
				{
					for (int k = x - 1; k < x + 2; k++)
					{
						if (temp_table[j + i][k] != 0 && temp_table[j + i][k] != current_ship_length * 2 + ship_code && temp_table[j + i][k] < 100 * 1000)
						{
							goto while_mark;
						}
					}
				}

				temp_table[y + i][x] = current_ship_length * 2 + ship_code;
			}
		}

		ship_code++;

		for (int i = 0; i < MAP_SIZE + 1; i++)
		{
			for (int j = 0; j < MAP_SIZE + 1; j++)
			{
				enemy_map[i][j] = temp_table[i][j];
			}
		}
	}
}

bool check_alive(int table[MAP_SIZE + 1][MAP_SIZE + 1])
{
	return true;

	for (int i = 0; i < MAP_SIZE; i++)
	{
		for (int j = 0; j < MAP_SIZE; j++)
		{
			if (table[i][j] > 0)
			{
				return true;
			}
		}
	}

	return false;
}