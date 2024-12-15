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
    double scale = 400 / myP.myTriDim;
    int xoff = myP.myTriDim;
    int yoff = myP.myTriDim;

    for (int q = 0; q < 4; q++)
    {
        for (auto &B : myP.myQuads[q].myBoxes)
        {
            cxy tl(scale * (B.loc.x + xoff), scale * (B.loc.y + yoff));
            cxy wh(scale * B.wh.x, scale * B.wh.y);
            S.rectangle(tl, wh);
        }
    }
}
