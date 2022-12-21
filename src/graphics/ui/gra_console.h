#pragma once

namespace AirSoft {

    struct Command
    {
        std::string                 CommandString;
        std::vector<std::string>    Args;
    };

    class UIConsole
    {
    private:
        Rectangle   fOuterRect;
        bool        fDragging;
        Vector2     fDraggingOffset;
        char*       fCommandText;

        bool        fVisible;
    public:
        UIConsole();

        void ToggleVisibility();

        void Render();
    private:
        /**
         * @brief Processes the current command text and resets it
         * 
         * @return Command Information about the Command
         */
        Command ProcessCurrentCommand();
        void HandleCommand(const Command& command);
    };

}