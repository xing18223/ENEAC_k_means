class Point{
public:
    Point(double new_x, double new_y, double new_z){
        x = new_x;
        y = new_y;
        z = new_z;
        cluster_id = -1;
        min_distance = __DBL_MAX__;
    }

    Point(){
        x = 0;
        y = 0;
        z = 0;
        cluster_id = -1;
        min_distance = __DBL_MAX__;
    }

    double get_x(){
        return x;
    }

    double get_y(){
        return y;
    }

    double get_z(){
        return z;
    }
    double get_min_distance(){
        return min_distance;
    }

    int get_cluster_id(){
        return cluster_id;
    }

    void update_cluster_id(int new_cluster_id){
        cluster_id = new_cluster_id;
    }

    void update_min_distance(double new_min_distance){
        min_distance = new_min_distance;
    }
private:
    double x;
    double y;
    double z;
    int cluster_id;
    double min_distance;
};