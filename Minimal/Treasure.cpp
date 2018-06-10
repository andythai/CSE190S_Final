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
	// Scale
	models[PEDESTAL_IND]->scale(vec3(2.0f, 3.5f, 2.0f));
	models[TREASURE_IND]->scale(0.85f);

	// Translate
	models[PEDESTAL_IND]->translate(vec3(0, -1.3f, 1.0f));
	models[TREASURE_IND]->translate(vec3(0, 1.1f - 1.3f, 1.0f));
}

/*------ MORE GAME RELATED FUNCTIONS ------*/
void Treasure::draw(Shader shader, glm::mat4 P, glm::mat4 V) {
	for (Model * parts : models) {
		parts->draw(shader, P, V);
	}
}