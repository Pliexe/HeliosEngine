#include "RenderResources.h"

#include "Helios/Graphics/Graphics.h"

using namespace Helios;

uint8 RenderResources::GetCurrentFrameIndex() const { return Graphics::GetCurrentFrameIndex(); }
uint8 RenderResources::GetNextFrameIndex() const { return Graphics::GetNextFrameIndex(); }
uint8 RenderResources::GetMaxFramesInFlight() const { return Graphics::GetMaxFramesInFlight(); }
