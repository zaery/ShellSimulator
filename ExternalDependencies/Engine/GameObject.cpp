#include "GameObject.h"
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include "Qt\qdebug.h"


GameObject::GameObject(glm::vec3 inposition, Renderable* inrenderable,
	Renderer* inrenderer){
		position = inposition;
		renderable = inrenderable;
		renderer = inrenderer;
}

void GameObject::update(float deltaT = 0.0166667f){
	for (int i=0;i<components.size();i++){
		components[i]->update(deltaT);
	}
}

template <typename T>
T GameObject::getComponent(){
	for (int i = 0; i < components.size(); i++){
		if (dynamic_cast<T>(components[i])!=NULL){
			return components[i];
		}
	}
	return GameComponent();
}

void GameObject::addComponent(GameComponent* newComponent){
	components.push_back(newComponent);
}