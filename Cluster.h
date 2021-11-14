class Cluster{
public:
    Cluster(double new_x, double new_y, double new_z){
        sum_x = 0;
        sum_y = 0;
        sum_z = 0;
        size = 0;
        x = new_x;
        y = new_y;
        z = new_z;
    }

    Cluster(){
        sum_x = 0;
        sum_y = 0;
        sum_z = 0;
        size = 0;
        x = 0;
        y = 0;
        z = 0;
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

    bool update_cluster_coord(){
        // if(x/(sum_x/size)>0.9 && y/(sum_y/size)>0.9 && z/(sum_z/size)>0.9){
        //     return true; //90% is the similarity between old and new clusters
        // }
        x = sum_x/size;
        y = sum_y/size;
        z = sum_z/size;
        return false;
    }

    void reset(){
        sum_x = 0;
        sum_y = 0;
        sum_z = 0;
        size = 0;
    }

    void add_point(Point new_point){
        sum_x += new_point.get_x();
        sum_y += new_point.get_y();
        sum_z += new_point.get_z();
        size ++;
    }

    int return_size(){
        return size;
    }

    double return_sum_x(){
        return sum_x;
    }

    double return_sum_y(){
        return sum_y;
    }

    double return_sum_z(){
        return sum_z;
    }
private:
    double x;
    double y;
    double z;

    double sum_x;
    double sum_y;
    double sum_z;

    int size;
};