#include "LayerStack.h"

LayerStack::~LayerStack()
{
	for (Layer* layer : mLayers)
	{
		layer->OnDetach();
		delete layer;
	}
}

void LayerStack::PushLayer(Layer* layer)
{
	mLayers.emplace_back(layer);
}

void LayerStack::PopLayer(Layer* layer)
{
	auto it = std::find(mLayers.begin(), mLayers.end(), layer);
	if (it != mLayers.end())
	{
		layer->OnDetach();
		mLayers.erase(it);
	}
}
