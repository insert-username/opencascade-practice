#include <iostream>

#include <gp_Pnt.hxx>

int main() {
    std::cout << "OpenCascade hip flask example https://old.opencascade.com/doc/occt-7.3.0/overview/html/occt__tutorial.html" << std::endl;

    double myHeight = 70;
    double myWidth = 50;
    double myThickness = 30;

    gp_Pnt aPnt1(-myWidth / 2.0, 0, 0);
    gp_Pnt aPnt2(-myWidth / 2.0, -myThickness / 4.0, 0);
    gp_Pnt aPnt3(0, -myThickness / 2.0, 0);
    gp_Pnt aPnt4(myWidth / 2.0, -myThickness / 4.0, 0);
    gp_Pnt aPnt5(myWidth / 2.0, 0, 0);
}
