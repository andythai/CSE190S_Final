#include "Transform.h"

/** CONSTRUCTORS AND DESTRUCTOR **/
Transform::Transform() {
	M = glm::mat4(1.0f);
}
Transform::Transform(glm::mat4 trans_mat) { 
	M = trans_mat;
}
Transform::~Transform() {
	for (unsigned int i = 0; i < child_ptrs.size(); i++) {
		delete(child_ptrs.back());
		child_ptrs.pop_back();
	}
}

/** LIST CHANGE FUNCTIONS **/
// Add child into list of Node*'s
void Transform::addChild(Node* toAdd) { 
	child_ptrs.push_back(toAdd);
}
// Remove child from list of Node*'s
void Transform::removeChild(Node* toDelete) { 
	child_ptrs.remove(toDelete);
}

/** TRANSFORMATION FUNCTIONS**/
void Transform::draw(Shader shader, glm::mat4 P, glm::mat4 V, glm::mat4 C) {
	for (Node* ptr : child_ptrs) { ptr->draw(shader, P, V, M * C); }
}
// Getter
glm::mat4 Transform::get_transform() { return M; }
// Setter
void Transform::change_transform(glm::mat4 toIns) { M = toIns; }

void Transform::update() { }