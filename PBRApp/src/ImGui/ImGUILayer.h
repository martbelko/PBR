#pragma once

#include "Core/Layer.h"

#include "Events/WindowEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"

class ImGUILayer : public Layer
{
public:
	ImGUILayer() = default;
	virtual ~ImGUILayer() override = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(Event& e) override;

	void Begin();
	void End();

	void BlockEvents(bool block) { mBlockEvents = block; }

	void SetDarkThemeColors();
private:
	bool mBlockEvents = true;
};
