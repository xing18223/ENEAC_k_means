#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include "Point.h"
#include "Cluster.h"
#include <chrono>
#include <random>
#include <cmath>
#include <atomic>

using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

vector<Point> input();
vector<Cluster> init_clusters(int k, int max_range);
void compute_dist(vector<Point> &points, vector<Cluster> &clusters);
bool update_clusters(vector<Cluster> &clusters);
void output(vector<Point> &points);
double euclidean_distance(Cluster &cluster, Point &point);

int main(){
    auto t1 = high_resolution_clock::now();

    int k = 20; //number of clusters
    int n = 10000; //number of data points
    int epochs = 250; //iterations
    int max_range = 999999999;
    bool convergence = false;
    int iterations;

    vector<Point> points = input();
    auto t2 = high_resolution_clock::now();
    vector<Cluster> clusters = init_clusters(k, max_range);
    auto t3 = high_resolution_clock::now();

    while(!convergence && iterations < epochs){
        iterations ++;
        compute_dist(points, clusters);
        convergence = update_clusters(clusters);
    }
    auto t4 = high_resolution_clock::now();
    output(points);

    auto t5 = high_resolution_clock::now();
    duration<double, std::milli> ms_double_a = t2 - t1;
    duration<double, std::milli> ms_double_b = t3 - t2;
    duration<double, std::milli> ms_double_c = t4 - t3;
    duration<double, std::milli> ms_double_d = t5 - t4;
    std::cout << "Input: " << ms_double_a.count() << "ms" <<endl;
    std::cout << "Init clusters: " << ms_double_b.count() << "ms" <<endl;
    std::cout << "Compute: " << ms_double_c.count() << "ms" <<endl;
    std::cout << "Output: " << ms_double_d.count() << "ms" <<endl;
    std::cout << "Total iterations: " << iterations << endl;
    return 0;
}

void compute_dist(vector<Point> &points, vector<Cluster> &clusters){
    for (vector<Cluster>::iterator c = clusters.begin(); c != clusters.end(); c++){
        int clusterId = c - clusters.begin();
        for (vector<Point>::iterator p = points.begin(); p != points.end(); p++){
            double dist = euclidean_distance(*c,*p);
            if (dist < p->get_min_distance()) {
                    p->update_min_distance(dist);
                    p->update_cluster_id(clusterId);
            }
            c->add_point(*p);
        }
    }
}

double euclidean_distance(Cluster &cluster, Point &point){
    double distance = sqrt(pow(cluster.get_x()-point.get_x(), 2) + pow(cluster.get_y()-point.get_y(), 2) + pow(cluster.get_z()-point.get_z(), 2));
    return distance;
}

bool update_clusters(vector<Cluster> &clusters){
    // Compute the new centroids
    bool convergence = true;

    for (vector<Cluster>::iterator c = clusters.begin(); c != clusters.end(); c++) {
        convergence = convergence && c->update_cluster_coord();
        c->reset();
    }

    return convergence;
}

vector<Cluster> init_clusters(int k, int max_range){
    vector<Cluster> clusters;
    
    // srand(time(0));
    std::random_device seed;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(seed()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> distrib(0, max_range);
    for (int i=0; i<k; i++){
        // clusters.push_back(Cluster(rand() % max_range, rand() % max_range, rand() % max_range));
        clusters.push_back(Cluster(distrib(gen), distrib(gen), distrib(gen)));
        // cout << ptr->get_x() << endl;
        // cout << ptr->get_y() << endl;
        // cout << ptr->get_z() << endl;
        // cout << "done" << endl;
    }

    // for (vector<Cluster>::iterator c = clusters.begin(); c != clusters.end(); c++){
    //     cout << c->get_x() <<endl;
    //     cout << c->get_y() <<endl;
    //     cout << c->get_z() <<endl;
    //     cout << c->return_size() <<endl;
    //     cout << c->return_sum_x() <<endl;
    //     cout << c->return_sum_y() <<endl;
    //     cout << c->return_sum_z() <<endl;
    //     cout << "done" << endl;
    // }
    return clusters;
}

vector<Point> input(){ //read all points
    vector<Point> points;
    string line;
    ifstream file("input2.csv");

    while (getline(file, line)) {
        stringstream lineStream(line);
        string bit;
        double x, y, z;
        getline(lineStream, bit, ',');
        x = stof(bit);
        getline(lineStream, bit, ',');
        y = stof(bit);
        getline(lineStream, bit, '\n');
        z = stof(bit);

        points.push_back(Point(x,y,z));
    }

    // for (vector<Point>::iterator it = points.begin(); it != points.end(); it++){
    //     cout << it->get_x() <<endl;
    //     cout << it->get_y() <<endl;
    //     cout << it->get_z() <<endl;
    //     cout << it->get_cluster_id() <<endl;
    //     cout << it->get_min_distance() <<endl;
    //     cout << "done" << endl;
    // }
    return points;
}

void output(vector<Point> &points){
    ofstream myfile;
    myfile.open("output.csv");

    for (vector<Point>::iterator i = points.begin(); i != points.end(); i++) {
        myfile << i->get_x() << "," << i->get_y() << "," << i->get_z() << "," << i->get_cluster_id() << endl;
    }
    myfile.close();
}