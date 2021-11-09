#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include "Point.h"
#include <chrono>

using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

vector<Point> read_csv();
vector<Point> init_clusters(int k, int n, vector<Point> &points);
void compute_dist(vector<Point> &points, vector<Point> &clusters);
void update_clusters(int k, vector<Point> &points, vector<Point> &clusters);
void output(vector<Point> &points);

int main(){
    auto t1 = high_resolution_clock::now();

    int k = 5; //number of clusters
    int n = 10000; //number of data points
    int epochs = 100; //iterations
    vector<Point> points = read_csv();
    vector<Point> clusters = init_clusters(k, n, points);

    for(int i = 0; i < epochs; ++i){
        compute_dist(points, clusters);
        update_clusters(k, points, clusters);
    }

    output(points);

    auto t2 = high_resolution_clock::now();
    duration<double, std::milli> ms_double = t2 - t1;
    std::cout << ms_double.count() << "ms";
    return 0;
}

void compute_dist(vector<Point> &points, vector<Point> &clusters){
    for (vector<Point>::iterator it = points.begin(); it != points.end(); it++) {
        for (vector<Point>::iterator c = clusters.begin(); c != clusters.end(); c++){
            int clusterId = c - clusters.begin();
            Point p = *it;
            double dist = c->distance(p);
            if (dist < p.min_dist) {
                p.min_dist = dist;
                p.cluster = clusterId;
            }
            *it = p;
        }
    }
}

void update_clusters(int k, vector<Point> &points, vector<Point> &clusters){
    vector<int> nPoints (k); //Points per cluster
    vector<double> sumX (k), sumY (k), sumZ (k); //Sum of coordinates from all points

    // Iterate over points to append data to centroids
    for (vector<Point>::iterator it = points.begin(); it != points.end(); it++) {
        int clusterId = it->cluster;
        nPoints[clusterId] += 1;
        sumX[clusterId] += it->x; //Parallelisable
        sumY[clusterId] += it->y;
        sumZ[clusterId] += it->z;

        it->min_dist = __DBL_MAX__;  // reset distance
    }
    // Compute the new centroids
    for (vector<Point>::iterator c = clusters.begin(); c != clusters.end(); c++) {
        int clusterId = c - clusters.begin();
        c->x = sumX[clusterId] / nPoints[clusterId]; //Parallelisable
        c->y = sumY[clusterId] / nPoints[clusterId];
        c->z = sumZ[clusterId] / nPoints[clusterId];
    }
}

void output(vector<Point> &points){
    ofstream myfile;
    myfile.open("output.csv");

    for (vector<Point>::iterator it = points.begin(); it != points.end(); it++) {
        myfile << it->x << "," << it->y << "," << it->z << "," << it->cluster << endl;
    }
    myfile.close();
}

vector<Point> init_clusters(int k, int n, vector<Point> &points){
    vector<Point> clusters;
    srand(time(0)); //random seed based on time(0), which is the time since Unix epoch
    for (int i = 0; i < k; ++i) {
        clusters.push_back(points[rand() % n]); //random number in the range of 0 to n-1
    }
    return clusters;
}

vector<Point> read_csv(){ //read all points
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