#include <wex.h>
#include "sProblem.h"

class cGUI
{
public:
    cGUI(sProblem &P);

private:
    wex::gui &fm;
    wex::menu* myAlgoMenu;

    sProblem &myP;

    void menus();
    void draw(wex::shapes &S);
};