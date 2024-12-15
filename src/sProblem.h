class cBox : public cxy
{
public:
    double xloc, yloc;

    /// @brief CTOR
    /// @param ix width
    /// @param iy height

    cBox(double ix, double iy);
    void locate(double x, double y);
    void rotate(int quadrant);
    cxy topright();
    cxy bottomright();
};
/**
 * The 2D space around the central point is divided into 4 quadrants
 *
 * Quadrant indices:
 * 0 : top left
 * 1 : top right
 * 3 : bottom right
 * 4 : bottom left
 *
 * Each quadrant is packed individually.  So that the same code can be used
 * to pack all quadrants each quadrant is rotated into the orientation of the
 * bottom right quadrant, packed, and then rotated back into its final position.
 *
 * The 3rd quadrant was chosen because the distance of a point from the central packing point
 * is simply calculated by summing the x and y co-ordinates.
 */
struct sQuadrant
{
    std::vector<cBox> myBoxes;  // the boxes packed into this quadrant
    std::vector<cBox> mySpaces; // remaining unpacked spaces

    sQuadrant();
    void pack(cBox &box);

    void rotate(int index);

    cxy maxBottomRight();

private:
    int findBestSpace(const cBox &box);
    void splitSpace(
        int space,
        const cBox &box);
};

struct sProblem
{
    std::vector<cBox> myBoxes;      // the boxes to be packed
    std::vector<sQuadrant> myQuads; // the quadrants around the central point
    int myTriDim;                   // length of non-hypotenous

    sProblem();

    /// @brief generate randomly some boxes to be packed
    /// @param min minimum magnitude of any dimension
    /// @param max maximum magnitude of any dimension
    /// @param count number of boxes to generate
    void genRandom(int min, int max, int count);

    /// @brief sort boxes into decreasing volume
    void sort();

    /// @brief pack all the boxes
    void pack();
};
