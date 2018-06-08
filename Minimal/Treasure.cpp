#include "Treasure.h"

// Macro definitions
#define PEDESTAL_IND 0
#define TREASURE_IND 1

using glm::vec3;

/**------------ CONSTRUCTOR/ DESTRUCTOR -----------*/
Treasure::Treasure(Model * pedestal, Model * treasure) {
	models.push_back(pedestal);
	models.push_back(treasure);

	initialize();
}

Treasure::~Treasure() { }

/*------------- HELPER FUNCTIONS --------------*/
void Treasure::initialize() {
	health = 25;
	// Scale
	//models[PEDESTAL_IND]->scale(0.8f);

	// Translate
	models[PEDESTAL_IND]->translate(vec3(0, 0, 0.7f));
	models[TREASURE_IND]->translate(vec3(0, 0.52f, 0.7f));
}

/*------ MORE GAME RELATED FUNCTIONS ------*/
void Treasure::update() {

}

int Treasure::getHealth() {
	return health;
}

void Treasure::draw(Shader shader, glm::mat4 P, glm::mat4 V, glm::mat4 C) {
	for (Model * parts : models) {
		parts->draw(shader, P, V, C);
	}
}

void Treasure::draw(Shader shader, glm::mat4 P, glm::mat4 V) {
	for (Model * parts : models) {
		parts->draw(shader, P, V);
	}
}