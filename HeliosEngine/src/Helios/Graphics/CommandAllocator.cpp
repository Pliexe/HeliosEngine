#include "Helios/Graphics/Graphics.h"
#include "CommandAllocator.h"

namespace Helios
{
    CommandAllocator &CommandAllocator::GetForCurrentThread()
    {
        return Graphics::GetMainDevice()->GetAllocatorForThread();
    }
}