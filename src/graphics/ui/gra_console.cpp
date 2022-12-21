#include "air_graphics.h"

#define CONSOLE_TEXT_SIZE 255

using namespace AirSoft;

UIConsole::UIConsole()
    : fOuterRect({100, 100, 300, 200}), fDragging(false), fDraggingOffset(), fCommandText(new char[255]), fVisible(false)
{
    memset(fCommandText, 0, CONSOLE_TEXT_SIZE);
}

void UIConsole::ToggleVisibility()
{
    fVisible = !fVisible;    
}

void UIConsole::Render()
{
    if (!fVisible) {return;}
    
    GuiPanel(fOuterRect, "Console");
    Rectangle statusBar = fOuterRect;
    statusBar.height = 24; // Statusbar height
    if (IsMouseButtonPressed(0))
    {
        if (CheckCollisionPointRec(GetMousePosition(), statusBar))
        {
            fDragging = true;
            fDraggingOffset = Vector2Subtract(GetMousePosition(), {fOuterRect.x, fOuterRect.y});
        }
    }
    if (IsMouseButtonReleased(0))
    {
        fDragging = false;
    }

    if (fDragging)
    {
        Vector2 newPos = Vector2Subtract(GetMousePosition(), fDraggingOffset);
        fOuterRect.x = newPos.x;
        fOuterRect.y = newPos.y;
    }


#define TEXT_FIELD_HEIGHT 32
    Rectangle text{fOuterRect.x, fOuterRect.y + fOuterRect.height - TEXT_FIELD_HEIGHT, fOuterRect.width, TEXT_FIELD_HEIGHT};
    GuiTextBox(text, fCommandText, CONSOLE_TEXT_SIZE, true);

    if (IsKeyPressed(KEY_ENTER))
    {
        HandleCommand(ProcessCurrentCommand());
    }
}



Command UIConsole::ProcessCurrentCommand()
{
    Command result;

    char* ptr = fCommandText;

    bool command = true;
    int scanStatus;
    char* readingString = new char[255];
    while ((scanStatus = sscanf(ptr, "%s", readingString)) != EOF)
    {
        if (command)
        {
            result.CommandString = readingString;
            command = false;
        }
        else
        {
            result.Args.push_back(readingString);
        }
        ptr += strlen(readingString) + 1;
        memset(readingString, 0, CONSOLE_TEXT_SIZE);
    }

    memset(fCommandText, 0, CONSOLE_TEXT_SIZE);
    return result;
}

void UIConsole::HandleCommand(const Command& command)
{
    
}
