#pragma once

#include <vector>
namespace Helios
{
    struct CommandNode 
    {

    };
    
    class CommandList
    {
    public:
        // TODO: return types
        virtual void AquireUniformBuffer() = 0;
        virtual void AquireStorageBuffer() = 0;
        virtual void AquireImage() = 0;

        virtual std::vector<CommandNode> GetCommands() = 0;
        
    private:
        // Some storage that is not set here but on extended
        
    };
}