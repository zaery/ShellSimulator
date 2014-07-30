#pragma once
#include <glm\glm.hpp>
#include "ExportHeader.h"
#include "Renderable.h"
#include "Renderer.h"
#include "GameComponent.h"

class ENGINE_SHARED GameObject
{
public:
	glm::vec3 position;
	Renderable* renderable;
	Renderer* renderer;

	std::vector<GameComponent*> components;

	GameObject(void);
	GameObject(glm::vec3 inposition, Renderable* inrenderable,
		Renderer* inrenderer);
	void update(float deltaT);

	template <typename T>
	T getComponent();

	void addComponent(GameComponent* component);
};