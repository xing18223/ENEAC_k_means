class Point{
public:
    Point(float new_x, float new_y, float new_z){
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

    float get_x(){
        return x;
    }

    float get_y(){
        return y;
    }

    float get_z(){
        return z;
    }
    float get_min_distance(){
        return min_distance;
    }

    int get_cluster_id(){
        return cluster_id;
    }

    void update_cluster_id(int new_cluster_id){
        cluster_id = new_cluster_id;
    }

    void update_min_distance(float new_min_distance){
        min_distance = new_min_distance;
    }
private:
    float x;
    float y;
    float z;
    int cluster_id;
    float min_distance;
};