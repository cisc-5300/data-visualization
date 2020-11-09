#include "Window.h"
#include "Graph.h"

using namespace Graph_lib;

struct Distribution {
    int year, young, middle, old;
};

istream& operator>>(istream& is, Distribution& d) {
    char ch1 = 0;
    char ch2 = 0;
    char ch3 = 0;
    Distribution dd;

    if (is >> ch1 >> dd.year >> ch2 >> dd.young >> dd.middle >> dd.old >> ch3){
        if (ch1 != '(' || ch2 != ':' | ch3 != ')') {
            is.clear(ios_base::failbit);
            return is;
        }
    }
    else
        return is;
    d = dd;
    return is;
}

class Scale {
    int cbase;
    int vbase;
    double scale;
    public:
        Scale(int b, int vb, double s): cbase{b}, vbase{vb}, scale{s} {}
        int operator()(int v) const {
            return cbase + (v - vbase)* scale;
        }
};

int main() {
    string filename = "japanese-age-data.txt";
    ifstream ifs{filename};
    if (!ifs) error("Can't open " + filename);

    constexpr int xmax = 600;
    constexpr int ymax = 400;

    constexpr int xoffset = 100;
    constexpr int yoffset = 60;

    constexpr int xspace = 40;
    constexpr int yspace = 40;

    constexpr int xlength = xmax - xoffset - xspace;
    constexpr int ylength = ymax - yoffset - yspace;

    constexpr int base_year = 1960;
    constexpr int end_year = 2040;

    constexpr double xscale = double(xlength) / (end_year - base_year);
    constexpr double yscale = double(ylength) / 100;

    Scale xs { xoffset, base_year, xscale };
    Scale ys { ymax-yoffset, 0, -yscale };

    Graph_lib::Window win {Point{100, 100}, 600, 400, "Functions"};

    Axis x {
        Axis::x,
        Point{xoffset, ymax-yoffset},
        xlength,
        (end_year - base_year) / 10,
        "year    1960    1970    1980    1990    "
        "2000    2010    2020    2030    2040"};
    x.label.move(-200, 0);

    Axis y {
        Axis::y,
        Point{xoffset, ymax - yoffset},
        ylength,
        10,
        "% of population"
    };

    Line current_year {Point{xs(2020), ys(0)}, Point({xs(2020), ys(100)})};
    current_year.set_style(Line_style::dash);

    Open_polyline children;
    Open_polyline adults;
    Open_polyline aged;

    for (Distribution d; ifs >> d;) {
        const int x = xs(d.year);
        children.add(Point{x, ys(d.young)});
        adults.add(Point{x, ys(d.middle)});
        aged.add(Point{x, ys(d.old)});
    }

    Text children_label {Point{20, children.point(0).y}, "age 0-14"};
    children.set_color(Color::red);
    children_label.set_color(Color::red);

    Text adults_label{Point{20, adults.point(0).y}, "age 15-64"};
    adults.set_color(Color::blue);
    adults_label.set_color(Color::blue);

    Text aged_label{Point{20, aged.point(0).y}, "age 65+"};
    aged.set_color(Color::dark_green);
    aged_label.set_color(Color::dark_green);

    win.attach(x);
    win.attach(y);
    win.attach(current_year);
    win.attach(children);
    win.attach(adults);
    win.attach(aged);
    win.attach(children_label);
    win.attach(adults_label);
    win.attach(aged_label);

    gui_main();
}