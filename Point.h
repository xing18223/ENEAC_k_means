struct Point{
    double x,y,z;
    int cluster;
    double min_dist;

    Point() : x(0.0), y(0.0), z(0.0), cluster(-1), min_dist(__DBL_MAX__) {}
    Point(double x, double y, double z) : x(x), y(y), z(z), cluster(-1), min_dist(__DBL_MAX__) {}

    double distance(Point p) {
        return (p.x - x) * (p.x - x) + (p.y - y) * (p.y - y) + (p.z - z) * (p.z - z);
    }
};
