#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <wex.h>
#include "cStarterGUI.h"
#include "sProblem.h"

class cGUI : public cStarterGUI
{
public:
    cGUI(sProblem &P);

private:
    sProblem &myP;

    void draw(wex::shapes &S);
};

void cBox::locate(double x, double y)
{
    xloc = x;
    yloc = y;
}
void cBox::rotate(int index)
{

    double temp;
    switch (index)
    {
    case 0:
    {
        cxy br(xloc + x, yloc + y);
        cxy ntl(-br.x, -br.y);
        xloc = ntl.x;
        yloc = ntl.y;
    }
    break;

    case 1:
    {
        cxy tr(xloc + x, yloc);
        cxy ntl(tr.y, -tr.x);
        xloc = ntl.x;
        yloc = ntl.y;
        temp = x;
        x = y;
        y = temp;
    }
    break;

    case 2:
        return;

    case 3:
    {
        cxy bl(xloc, yloc + y);
        cxy ntl(-bl.y, bl.x);
        xloc = ntl.x;
        yloc = ntl.y;
        temp = x;
        x = y;
        y = temp;
    }
    break;
    default:
        throw std::runtime_error(
            "sTriangle::rotate bad quadrant");
    }
}
cxy cBox::bottomright()
{
    return cxy(xloc + x, yloc + y);
}
cxy cBox::topright()
{
    return cxy(xloc + x, yloc);
}

void sQuadrant::pack(cBox &box)
{
    switch (myBoxes.size())
    {
    case 0:
        box.locate(0, 0);
        break;
    case 1:
        box.locate(myBoxes[0].x, 0);
        break;
    case 2:
        box.locate(0, myBoxes[0].y);
        break;
    default:
        return;
    }
    myBoxes.push_back(box);
}
void sQuadrant::rotate(int index)
{
    for (auto &B : myBoxes)
        B.rotate(index);
}
cxy sQuadrant::maxBottomRight()
{
    cxy ret(0, 0);
    for (auto &B : myBoxes)
    {
        cxy br = B.bottomright();
        if (br.x + br.y > ret.x + ret.y)
            ret = br;
    }
    return ret;
}

void sProblem::genRandom(int min, int max, int count)
{
    srand(77);
    myTriDim = 50;
    myBoxes.clear();
    for (int k = 0; k < count; k++)
    {
        double x = rand() % (max - min) + min;
        // double y = rand() % (max - min) + min;
        double y = 2 * x;
        myBoxes.emplace_back(x, y);
    }
}
sProblem::sProblem()
{
    myQuads.resize(4);
}
void sProblem::sort()
{
    std::sort(myBoxes.begin(), myBoxes.end(),
              [](const cBox &a, const cBox &b)
              {
                  return a.x * a.y > b.x * b.y;
              });
}
void sProblem::pack()
{
    /* Sort boxes into order of decreasing volume

    This permits the smaller boxes to be packed into
    the spaces left behind by the larger boxes previously packed.

    */
    sort();

    /* Pack boxes, round robin fashion, into each quadrant

    This ensures that each quadrant packs 
    boxes of approximately the same total

    One way I like to think of this is that it simulates crystal growth from a saturated solution.
    The boxes are molecules floating around in the solution.  Occasionally a box finds the
    perfect spot on the growing crystal where it fits perfectly.

    */
    for (int k = 0; k < myBoxes.size(); k += 4)
    {
        myQuads[0].pack(myBoxes[k]);
        myQuads[1].pack(myBoxes[k + 1]);
        myQuads[2].pack(myBoxes[k + 2]);
        myQuads[3].pack(myBoxes[k + 3]);
    }

    // Rotate the quadrants back into their correct positions

    for (int q = 0; q < 4; q++)
        myQuads[q].rotate(q);
}

cGUI::cGUI(sProblem &P)
    : cStarterGUI(
          "Packer",
          {50, 50, 1000, 500}),
      myP(P)
{
    show();
    run();
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
            cxy tl(scale * (B.xloc + xoff), scale * (B.yloc + yoff));
            cxy wh(scale * B.x, scale * B.y);
            S.rectangle(tl, wh);
        }
    }
}

main()
{
    sProblem P;
    P.genRandom(5, 10, 20);
    P.pack();
    cGUI theGUI(P);
    return 0;
}
