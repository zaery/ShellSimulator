#pragma once
class GameObject;
class GameComponent{
public:
	GameObject* parent;
	virtual void update(float deltaT)=0;
	GameComponent(){};
	GameComponent(GameObject* object){
		parent = object;
	};
};