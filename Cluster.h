class Cluster{
public:
    Cluster(float new_x, float new_y, float new_z){
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

    float get_x(){
        return x;
    }

    float get_y(){
        return y;
    }

    float get_z(){
        return z;
    }

    bool update_cluster_coord(){
        // if(x/(sum_x/size)>0.9999 && y/(sum_y/size)>0.9999 && z/(sum_z/size)>0.9999){
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
private:
    float x;
    float y;
    float z;

    float sum_x;
    float sum_y;
    float sum_z;

    int size;
};