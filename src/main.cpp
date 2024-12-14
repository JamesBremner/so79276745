#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <wex.h>
#include "cStarterGUI.h"

class cBox : public cxy
{
public:
    double xloc, yloc;

    cBox(double ix, double iy)
        : cxy(ix, iy)
    {
    }
    void locate(double x, double y);
    void rotate(int quadrant);
};
struct sTriangle
{
    std::vector<cBox> myBoxes;

    void pack(cBox &box);
    void rotate(int index);
};

struct sProblem
{
    std::vector<cBox> myBoxes;
    std::vector<sTriangle> myTriangles;
    int myTriDim; // length of non-hypotenous

    sProblem();
    void genRandom(int min, int max, int count);
    void sort();
    void pack();
};

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
        cxy ntl(0, -tr.x);
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
        cxy ntl(-bl.y, 0);
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

void sTriangle::pack(cBox &box)
{
    if (!myBoxes.size())
    {
        box.locate(0, 0);
        myBoxes.push_back(box);
    }
}
void sTriangle::rotate(int index)
{
    for (auto &B : myBoxes)
        B.rotate(index);
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
    myTriangles.resize(4);
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
    sort();

    for (int k = 0; k < myBoxes.size(); k += 4)
    {
        myTriangles[0].pack(myBoxes[k]);
        myTriangles[1].pack(myBoxes[k + 1]);
        myTriangles[2].pack(myBoxes[k + 2]);
        myTriangles[3].pack(myBoxes[k + 3]);
    }

    for (int q = 0; q < 4; q++)
        myTriangles[q].rotate(q);
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
        for (auto &B : myP.myTriangles[q].myBoxes)
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
