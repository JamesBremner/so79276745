#include "cGUI.h"

cGUI::cGUI(sProblem &P)
    : myP(P),
      fm(wex::maker::make())
{
    {
        fm.move({50, 50, 1000, 500});
        fm.text("Packer");

        fm.events().draw(
            [&](PAINTSTRUCT &ps)
            {
                wex::shapes S(ps);
                draw(S);
            });
    }

    fm.show();
    fm.run();
}

void cGUI::draw(wex::shapes &S)
{   
    double scale = 450 / ( 1.2 * myP.mySpread );
    int xoff = myP.mySpread ;
    int yoff = myP.mySpread ;

    for (int q = 0; q < 4; q++)
        for (auto &B : myP.myQuads[q].myBoxes)
            S.rectangle(
                cxy(scale * (B.loc.x + xoff), scale * (B.loc.y + yoff)),
                cxy(scale * B.wh.x, scale * B.wh.y));

}
