#include <wex.h>
#include "sProblem.h"

class cGUI
{
public:
    cGUI(sProblem &P);

private:
    wex::gui &fm;
    sProblem& myP;

     void draw(wex::shapes &S);
};