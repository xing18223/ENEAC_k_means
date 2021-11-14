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
#include <immintrin.h> //AVX intrinsics, not available on aarch64

#include "tbb/tbb.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"

using namespace std;
using namespace tbb;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

vector<Point> input();
vector<Cluster> init_clusters(int k, int max_range);
void compute_dist(vector<Point> &points, vector<Cluster> &clusters, int k, int n);
bool update_clusters(int k, vector<Cluster> &clusters);
void output(vector<Point> &points);
double euclidean_distance(Cluster &cluster, Point &point);

int main(){
    auto t1 = high_resolution_clock::now();

    int k = 20; //number of clusters
    int n = 10000; //number of data points
    int epochs = 250; //iterations
    int max_range = 999999999; //Space in which centroids can be generated
    bool convergence = false;
    int iterations;

    vector<Point> points = input();
    auto t2 = high_resolution_clock::now();

    vector<Cluster> clusters = init_clusters(k, max_range);
    auto t3 = high_resolution_clock::now();

    while(!convergence && iterations < epochs){
        iterations ++;
        // auto t3i = high_resolution_clock::now();
        compute_dist(points, clusters, k, n);
        // auto t3a = high_resolution_clock::now();
        convergence = update_clusters(k, clusters);
        // auto t3b = high_resolution_clock::now();
        // duration<double, std::milli> comp_dist_time = t3a - t3i;
        // duration<double, std::milli> upd_cls_time = t3b - t3a;
        // std::cout << "Compute distance time: " << comp_dist_time.count() << "ms" <<endl;
        // std::cout << "Update clusters time: " << upd_cls_time.count() << "ms" <<endl;
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

void compute_dist(vector<Point> &points, vector<Cluster> &clusters, int k, int n){
    // for (vector<Cluster>::iterator c = clusters.begin(); c != clusters.end(); c++){
    //     int clusterId = c - clusters.begin();
    //     vector<Point>::iterator p = points.begin(); //Improvement, instead of creating a new pointer, use the same and increas it
    //     parallel_for(blocked_range<size_t>(0,n), [&](blocked_range<size_t> &r){
    //         for(size_t i=r.begin(); i!=r.end(); ++i){
    //             double dist = euclidean_distance(*c, *p);
    //             if (dist < p->get_min_distance()) {
    //                 p->update_min_distance(dist);
    //                 p->update_cluster_id(clusterId);
    //             }
    //             c->add_point(*p);
    //             p++;
    //         }
    //     });
    // }

    // parallel_for(blocked_range<size_t>(0,k), [&](blocked_range<size_t> &r){
    //     for(size_t j=r.begin(); j!=r.end(); ++j){
    //         Cluster c = clusters.at(j);
    //         parallel_for(blocked_range<size_t>(0,n), [&](blocked_range<size_t> &r){
    //             for(size_t i=r.begin(); i!=r.end(); ++i){
    //                 Point p = points.at(i);
    //                 double dist = euclidean_distance(c, p);
    //                 if (dist < p.get_min_distance()) {
    //                     p.update_min_distance(dist);
    //                     p.update_cluster_id(j);
    //                 }
    //                 c.add_point(p);
    //                 points.at(i) = p;
    //             }
    //         });
    //         clusters.at(j) = c;
    //     }
    // });

    parallel_for(blocked_range<int>(0,k), [&](blocked_range<int> &r){
        for(int j=r.begin(); j!=r.end(); ++j){
            Cluster* c = &clusters.at(j);
    // for (vector<Cluster>::iterator c = clusters.begin(); c != clusters.end(); c++){
    //         int cluster_id = c - clusters.begin();
            parallel_for(blocked_range<int>(0,n), [&](blocked_range<int> &r){
                for(int i=r.begin(); i!=r.end(); ++i){
                    Point* p = &points.at(i); //Use pointers to make the code faster
                    double dist = euclidean_distance(*c, *p);
                    if (dist < p->get_min_distance()) {
                        p->update_min_distance(dist);
                        p->update_cluster_id(j);
                    }
                    c->add_point(*p);
                }
            });
    }
    });

    // for (vector<Cluster>::iterator c = clusters.begin(); c != clusters.end(); c++){
    //     int clusterId = c - clusters.begin();
    //     // Point* p = points.data(); //This implementation doesn't work because p++ inside parallel_for has no meaning, the loop is not serial
    //     parallel_for(blocked_range<size_t>(0,n), [&](blocked_range<size_t> &r){
    //         for(size_t i=r.begin(); i!=r.end(); ++i){
    //             Point p = points.at(i);
    //             double dist = euclidean_distance(*c, p);
    //             if (dist < p.get_min_distance()) {
    //                 p.update_min_distance(dist);
    //                 p.update_cluster_id(clusterId);
    //             }
    //             c->add_point(p);
    //             points.at(i) = p;
    //         }
    //     });
    // }
}

double euclidean_distance(Cluster &cluster, Point &point){
    double distance; //AVX intrinsics, not applicable to the ARM64 architecture
    __m256d vec1 = {point.get_x(), point.get_y(), point.get_z()};
    __m256d vec2 = {cluster.get_x(), cluster.get_y(), cluster.get_z()};
    __m256d diff = _mm256_sub_pd(vec1, vec2);
    __m256d square = _mm256_mul_pd(diff, diff);
    __m256d sum = _mm256_hadd_pd(square, square);
    distance = sqrt(((double*)&sum)[0] + ((double*)&sum)[2]);

    // double distance = sqrt(pow(cluster.get_x()-point.get_x(), 2) + pow(cluster.get_y()-point.get_y(), 2) + pow(cluster.get_z()-point.get_z(), 2));
    return distance;
}

bool update_clusters(int k, vector<Cluster> &clusters){
    // Compute the new centroids

    // vector<Cluster>::iterator c = clusters.begin();
    // parallel_for(blocked_range<size_t>(0,k), [&](blocked_range<size_t> &r){
    //         for(size_t i=r.begin(); i!=r.end(); ++i){
    //             c->update_cluster_coord();
    //             c++;
    //         }
    //     });

    bool convergence = true;

    for (vector<Cluster>::iterator c = clusters.begin(); c != clusters.end(); c++) {
        convergence = convergence && c->update_cluster_coord();
        c->reset();
    }

    return convergence;
}

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

void output(vector<Point> &points){
    ofstream myfile;
    myfile.open("output.csv");

    for (vector<Point>::iterator i = points.begin(); i != points.end(); i++) {
        myfile << i->get_x() << "," << i->get_y() << "," << i->get_z() << "," << i->get_cluster_id() << endl;
    }
    myfile.close();
}