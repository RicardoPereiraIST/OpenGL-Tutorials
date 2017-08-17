#include "GameLevel.h"
#include <fstream>
#include <sstream>

std::map<unsigned int, glm::vec3> GameLevel::colors = {
	{ 1, glm::vec3(0.8f, 0.8f, 0.7f) },
	{ 2, glm::vec3(0.2f, 0.6f, 1.0f) },
	{ 3, glm::vec3(0.0f, 0.7f, 0.0f) },
	{ 4, glm::vec3(0.8f, 0.8f, 0.4f) },
	{ 5, glm::vec3(1.0f, 0.5f, 0.0f) }
};

GameLevel::GameLevel() {}

void GameLevel::load(const char *file, unsigned int levelWidth, unsigned int levelHeight)
{
	bricks.clear();
	unsigned int tileCode;
	GameLevel level;
	std::string line;
	std::ifstream fstream(file);
	std::vector<std::vector<unsigned int>> tileData;
	if (fstream)
	{
		while (std::getline(fstream, line)) // Read each line from level file
		{
			std::istringstream sstream(line);
			std::vector<unsigned int> row;
			while (sstream >> tileCode) // Read each word seperated by spaces
				row.push_back(tileCode);
			tileData.push_back(row);
		}
		if (tileData.size() > 0)
			init(tileData, levelWidth, levelHeight);
	}
	else
		std::cout << "File " << file << " not found!" << std::endl;
}

void GameLevel::draw(std::string renderer)
{
	for (GameObject &tile : bricks)
		if (!tile.destroyed)
			tile.draw(renderer);
}

bool GameLevel::isCompleted()
{
	for (GameObject &tile : bricks)
		if (!tile.isSolid && !tile.destroyed)
			return false;
	return true;
}

void GameLevel::init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight)
{
	// Calculate dimensions
	unsigned int height = tileData.size();
	unsigned int width = tileData[0].size(); // Note we can index vector at [0] since this function is only called if height > 0
	float unit_width = levelWidth / static_cast<float>(width), unit_height = levelHeight / height;
	// Initialize level tiles based on tileData		
	for (unsigned int y = 0; y < height; ++y)
	{
		for (unsigned int x = 0; x < width; ++x)
		{
			if (tileData[y][x] == 0) continue;

			glm::vec2 pos(unit_width * x, unit_height * y);
			glm::vec2 size(unit_width, unit_height);

			GameObject obj(pos, size, tileData[y][x] == 1 ? "block_solid" : "block", colors[tileData[y][x]]);
			obj.isSolid = tileData[y][x] == 1 ? true : false;

			bricks.push_back(obj);
		}
	}
}