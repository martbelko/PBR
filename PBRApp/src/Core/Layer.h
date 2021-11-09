#pragma once

#include "Timestep.h"

class Event;

class Layer
{
public:
	Layer() = default;
	virtual ~Layer() = default;

	virtual void OnAttach() {}
	virtual void OnDetach() {}
	virtual void OnUpdate(Timestep ts) {}
	virtual void OnImGuiRender() {}
	virtual void OnEvent(Event& e) {}
};
