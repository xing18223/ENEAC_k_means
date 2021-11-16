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
// #include <immintrin.h> //AVX intrinsics, not available on aarch64
#include "tbb/tbb.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"
#include <arm_neon.h>

using namespace std;
using namespace tbb;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;
//----------------------------------------------------------------------//
vector<Point> input();
void output(vector<Point> &points);
vector<Cluster> init_clusters(int k, int max_range);
void compute_dist(vector<Cluster> &clusters, vector<Point> &points, int n);
bool update_clusters(vector<Cluster> &clusters, vector<Point> &points);
double euclidean_distance(Cluster &cluster, Point &point);
//----------------------------------------------------------------------//
int main(){
    auto t1 = high_resolution_clock::now();

    int k = 10; //number of clusters
    int n = 10000; //number of data points
    int epochs = 1000; //iterations
    int max_range = 999999999; // space in which clusters can be initialised
    bool convergence = false;
    int iterations = 0;

    vector<Point> points = input();
    auto t2 = high_resolution_clock::now();

    vector<Cluster> clusters = init_clusters(k, max_range);
    auto t3 = high_resolution_clock::now();

    while(!convergence && iterations < epochs){
        iterations ++;
        compute_dist(clusters, points, n);
        convergence = update_clusters(clusters, points);
    }
    auto t4 = high_resolution_clock::now();
    output(points);

    auto t5 = high_resolution_clock::now();
    duration<double, std::milli> ms_double_a = t2 - t1;
    duration<double, std::milli> ms_double_b = t3 - t2;
    duration<double, std::milli> ms_double_c = t4 - t3;
    duration<double, std::milli> ms_double_d = t5 - t4;
    cout << "k: " << k << endl;
    cout << "n :" << n << endl;
    cout << "max coord value: " << max_range << endl;
    cout << "Input: " << ms_double_a.count() << "ms" <<endl;
    cout << "Init clusters: " << ms_double_b.count() << "ms" <<endl;
    cout << "Compute: " << ms_double_c.count() << "ms" <<endl;
    cout << "Output: " << ms_double_d.count() << "ms" <<endl;
    cout << "Total iterations: " << iterations << endl;
    return 0;
}
//----------------------------------------------------------------------//
void compute_dist(vector<Cluster> &clusters, vector<Point> &points, int n){
    for (vector<Cluster>::iterator c = clusters.begin(); c != clusters.end(); c++){
        int cluster_id = c - clusters.begin();
        parallel_for(blocked_range<int>(0,n), [&](blocked_range<int> &r){
            for(int i=r.begin(); i!=r.end(); ++i){
                Point* p = &points.at(i); //Use pointers to make the code faster
                double dist = euclidean_distance(*c, *p);
                if (dist < p->get_min_distance()) {
                    p->update_min_distance(dist);
                    p->update_cluster_id(cluster_id);
                }
            }
        });
    }
}
//----------------------------------------------------------------------//
double euclidean_distance(Cluster &cluster, Point &point){
    double distance; //AVX intrinsics, not applicable to the ARM64 architecture
    float32x4_t vec1 = {point.get_x(), point.get_y(), point.get_z()};
    float32x4_t vec2 = {cluster.get_x(), cluster.get_y(), cluster.get_z()};
    float32x4_t diff = vsubq_f32(vec1, vec2);
    float32x4_t square = vmulq_f32(diff, diff);
    distance = square[0] + square[1] + square[2];

    return distance;
}
//----------------------------------------------------------------------//
bool update_clusters(vector<Cluster> &clusters, vector<Point> &points){
    // Compute the new centroids
    bool convergence = true;
    for (vector<Point>::iterator p = points.begin(); p !=points.end(); p++){
        clusters.at(p->get_cluster_id()).add_point(*p);
    }

    for (vector<Cluster>::iterator c = clusters.begin(); c != clusters.end(); c++) {
        convergence = convergence && c->update_cluster_coord();
        c->reset();
    }

    return convergence;
}
//----------------------------------------------------------------------//
vector<Cluster> init_clusters(int k, int max_range){
    vector<Cluster> clusters;
    
    std::random_device seed;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(seed()); //Standard mersenne_twister_engine seeded with seed()
    std::uniform_int_distribution<> distrib(0, max_range);

    for (int i=0; i<k; i++){
        clusters.push_back(Cluster(distrib(gen), distrib(gen), distrib(gen)));
    }

    return clusters;
}
//----------------------------------------------------------------------//
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

        points.push_back(Point(x, y, z));
    }
    return points;
}
//----------------------------------------------------------------------//
void output(vector<Point> &points){
    ofstream myfile;
    myfile.open("output.csv");

    for (vector<Point>::iterator i = points.begin(); i != points.end(); i++) {
        myfile << i->get_x() << "," << i->get_y() << "," << i->get_z() << "," << i->get_cluster_id() << endl;
    }
    myfile.close();
}