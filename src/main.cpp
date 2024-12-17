#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>

#include "cGUI.h"
#include "sProblem.h"

cBox::cBox(double ix, double iy)
    : userID(-1),
      wh(ix, iy)
{
}
cBox::cBox(int id, double ix, double iy)
    : userID(id),
      wh(ix, iy)
{
}

void cBox::locate(double x, double y)
{
    loc = cxy(x, y);
}
void cBox::locate(const cBox &other)
{
    loc = other.loc;
}
void cBox::rotate(int index)
{
    double temp;
    switch (index)
    {
    case 0:
    {
        cxy br = bottomright();
        cxy ntl(-br.x, -br.y);
        loc = ntl;
    }
    break;

    case 1:
    {
        cxy tr = topright();
        cxy ntl(tr.y, -tr.x);
        loc = ntl;
        temp = wh.x;
        wh.x = wh.y;
        wh.y = temp;
    }
    break;

    case 2:
        return;

    case 3:
    {
        cxy bl = bottomleft();
        cxy ntl(-bl.y, bl.x);
        loc = ntl;
        temp = wh.x;
        wh.x = wh.y;
        wh.y = temp;
    }
    break;
    default:
        throw std::runtime_error(
            "sTriangle::rotate bad quadrant");
    }
}
cxy cBox::bottomright() const
{
    return cxy(loc.x + wh.x, loc.y + wh.y);
}
cxy cBox::topright() const
{
    return cxy(loc.x + wh.x, loc.y);
}
cxy cBox::bottomleft() const
{
    return cxy(loc.x, loc.y + wh.y);
}

sQuadrant::sQuadrant()
{
    clear();
}

void sQuadrant::clear()
{
    myBoxes.clear();
    mySpaces.clear();
    cBox sp(1000, 1000);
    sp.locate(0, 0);
    mySpaces.push_back(sp);
}
int sQuadrant::findBestSpace(const cBox &box)
{
    if (!myBoxes.size())
        return 0;

    int bestSpaceIndex = 0;
    double leastWastage = INT_MAX;
    double leastDistance = INT_MAX;

    for (int s = 0; s < mySpaces.size(); s++)
    {
        // check for remains of a split space
        if (mySpaces[s].loc.x < 0)
            continue;
        // check that space is tall enough for box
        if (mySpaces[s].wh.y < box.wh.y)
            continue;

        // the box could be fitted into this space
        // apply best space algorithm

        switch (sProblem::bestSpace())
        {

        case sProblem::eBestSpace::firstFit:
            return s;
            break;

        case sProblem::eBestSpace::minGap:
        {
            double wastage = mySpaces[s].wh.y - box.wh.y;
            if (wastage < leastWastage)
            {
                leastWastage = wastage;
                bestSpaceIndex = s;
            }
        }
        break;

        case sProblem::eBestSpace::minDist:
        {
            double distance = mySpaces[s].loc.x + mySpaces[s].loc.y;
            if (distance < leastDistance)
            {
                leastDistance = distance;
                bestSpaceIndex = s;
            }
        }
        break;
        }
    }

    return bestSpaceIndex;
}

void sQuadrant::splitSpace(
    int ispace,
    const cBox &box)
{
    cBox &sp0 = mySpaces[ispace];
    cBox sp1(sp0.wh.x - box.wh.x, box.wh.y);
    sp1.locate(sp0.loc.x + box.wh.x, sp0.loc.y);
    cBox sp2(sp0.wh.x, sp0.wh.y - box.wh.y);
    sp2.locate(sp0.loc.x, sp0.loc.y + box.wh.y);
    mySpaces.push_back(sp1);
    mySpaces.push_back(sp2);
    // mySpaces.erase(space);
    mySpaces[ispace].loc.x = -1;
}
void sQuadrant::pack(cBox &box)
{
    // find index of space where the box will fit
    int space = findBestSpace(box);

    // move the box into the space
    box.locate(mySpaces[space]);

    // split the space into two smaller spaces
    // one to the right, one below
    splitSpace(space, box);

    /* Add the box to the quadrant

    Each quadrant keeps a copy of the boxes packed into it
    This is profligate with memory,
    but makes the code simpler when rotating the quadrants into their final positions.
    It should be of no concern unless there are millions of boxes
    */
    myBoxes.push_back(box);
}

int sQuadrant::maxDim() const
{
    if (!myBoxes.size())
        return 50;
    int ret = 0;
    for (auto &box : myBoxes)
    {
        cxy br = box.bottomright();
        if (br.x > ret)
            ret = br.x;
        if (br.y > ret)
            ret = br.y;
    }
    return ret;
}

void sQuadrant::rotate(int index)
{
    for (auto &B : myBoxes)
        B.rotate(index);
}

void sProblem::input(const std::string &sin)
{
    std::istringstream is(sin);
    int w, h;
    myBoxes.clear();
    int userID = 0;
    is >> w >> h;
    while (is.good())
    {
        myBoxes.emplace_back(userID, w, h);
        userID++;
        is >> w >> h;
    }
}

std::string sProblem::output() const
{
    std::stringstream ss;
    for (int q = 0; q < 4; q++)
    {
        for (auto &B : myQuads[q].myBoxes)
        {
            ss << B.userID << " "
               << B.loc.x << " "
               << B.loc.y << "\n";
        }
    }
    return ss.str();
}

void sProblem::genRandom(int min, int max, int count)
{
    srand(77);
    myBoxes.clear();
    for (int k = 0; k < count; k++)
    {
        double x = rand() % (max - min) + min;
        double y = rand() % (max - min) + min;
        myBoxes.emplace_back(x, y);

        std::cout << "( " << x << " " << y << "),";
    }
}

sProblem::eBestSpace sProblem::myBestSpace;

sProblem::sProblem()
{
    myBestSpace = eBestSpace::firstFit;

    // construct 4 quadrants around central point
    myQuads.resize(4);
}

void sProblem::sort()
{
    std::sort(
        myBoxes.begin(), myBoxes.end(),
        [](const cBox &a, const cBox &b)
        {
            return a.wh.x * a.wh.y > b.wh.x * b.wh.y;
        });
}
void sProblem::pack()
{
    /* Sort boxes into order of decreasing volume

    This permits the smaller boxes to be packed into
    the spaces left behind by the larger boxes previously packed.

    */
    sort();

    // clear the quadrants

    for (auto &q : myQuads)
        q.clear();

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
        if (k + 1 == myBoxes.size())
            break;
        myQuads[1].pack(myBoxes[k + 1]);
        if (k + 2 == myBoxes.size())
            break;
        myQuads[2].pack(myBoxes[k + 2]);
        if (k + 3 == myBoxes.size())
            break;
        myQuads[3].pack(myBoxes[k + 3]);
    }

    // Rotate the quadrants back into their correct positions

    for (int q = 0; q < 4; q++)
        myQuads[q].rotate(q);

    // maximum location along x or y axis
    // used to scale graphical display
    // calculated in bottom right quadrant so both x and y axis are positive
    // assumes algorithm works well to even out the spread of all quadrants

    mySpread = myQuads[2].maxDim();
}

bool sProblem::test()
{
    sProblem T;
    T.input(
        "5 8\n"
        "32 19\n");
    if (T.myBoxes.size() != 2)
        return false;
    if (T.myBoxes[1].wh.x != 32)
        return false;

    T.pack();

    auto output = T.output();

    if (output != "1 -32 -19\n0 0 -5\n")
        return false;

    return true;
}

main()
{
    sProblem P;
    if (!P.test())
    {
        std::cout << "Unit tests failed\n";
        exit(1);
    }
    P.genRandom(2, 50, 50);
    P.pack();
    cGUI theGUI(P);
    return 0;
}
