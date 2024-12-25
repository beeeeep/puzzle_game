#include "abstract_game/puzzle.h"
#include "gui/gui.h"

int main(int argc, char **argv)
{
    return puzzleGame(init_gui_structures, delete_gui_structures);
}