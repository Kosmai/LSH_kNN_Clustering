#include <iostream>
#include <list>
#include <string>
#include <algorithm>
#include <random>
#include <cfloat>
#include "readInput.hpp"
#include "point.hpp"
#include "randGen.hpp"
#include "cluster.hpp"

#define DIMS 128

void findRandomCentroids(std::vector<Point*>& points, int k, std::vector<Point*>& centroids){
	//int size = points.size();
	//std::cout << size << std::endl;
	std::vector<Point*> shuffledPoints = points;
	std::random_shuffle(std::begin(shuffledPoints), std::end(shuffledPoints));
	for(int i = 0; i < k; i++){
		centroids.push_back(shuffledPoints[i]);
	}
}

void findPlusPlusCentroids(std::vector<Point*>& points, int k, std::vector<Point*>& centroids){
    int size = points.size();
    //std::cout << size << std::endl;
    std::vector<Point*> shuffledPoints = points;
    std::random_shuffle(std::begin(shuffledPoints), std::end(shuffledPoints));
    centroids.push_back(shuffledPoints[0]); //assign 1 centroid at random
    shuffledPoints.erase(shuffledPoints.begin());
    k--;

    int id;
    double dist;
    double distances[size];

    while(k > 0){
        //initialize distances to INF
        for(int i = 0; i < size; i++){
            distances[i] = DBL_MAX;
        }

        //loop through all points and find distance to closest centroid for each one
        for(auto point : shuffledPoints){
            id = stoi(point->getId());
            for(auto centroid : centroids){

                dist = point->l2Distance(centroid);

                //square dist TODO

                if(dist < distances[id]){
                    distances[id] = dist;
                }

            }
        }

        //add all closest distances together
        double totalDists = 0;
        for(int i = 0; i < size; i++){
            if(distances[i] != DBL_MAX){
                totalDists += distances[i];
            }
        }

        //Choose a random float from 0-1
        int idx = 0;
        float chosen = getUniformRandomFloat();

        //reduce it by dist[point]/totalDists (normalized probability)
        //this way, when the number becomes negative we stop, and we
        //have randomly selected a point with the required probability
        while(chosen > 0){
            if(distances[idx] != DBL_MAX){
                //std::cout << chosen << std::endl;
                chosen -= distances[idx]/totalDists;
            }
            idx++;
        }

        //get the id of the point so we can delete it from the list
        std::string targetId = shuffledPoints[idx]->getId();

        //add it to the centroids list
        centroids.push_back(shuffledPoints[idx]);

        //delete it from the points list
        std::vector<Point*>::iterator it;
        for(it = shuffledPoints.begin(); it != shuffledPoints.end(); it++){
            if((*it)->getId() == targetId){
                shuffledPoints.erase(it);
                break;
            }
        }
        k--;
    }
}



int main(){
	int k = 10;
	setRandomSeed(time(NULL));
    std::vector<Point*> points;
	std::vector<Point*> queries;
    std::vector<Point*> centroids;
    std::string inputFile = "datasets/input_small_id";
	std::string queryFile = "datasets/query_small_id";
    readDataSet(inputFile, ' ', points);
	readDataSet(queryFile, ' ', queries);

	findPlusPlusCentroids(points, k, centroids);
    //findRandomCentroids(points, k, centroids);

	for(int i = 0; i < k; i++){
		std::cout << centroids[i]->getId() << std::endl;
	}

    Cluster* clusters = new Cluster[k];
    for(int i = 0; i < k; i++){
        clusters[i] = Cluster(DIMS);
        clusters[i].setCentroid(centroids[i]);
        //clusters[i].setCentroid(queries[i]);
    }

    double dist, minDist;
    int minIndex;


    for(int iter = 0; iter < 10; iter++){
        for(auto point : points){
            minDist = DBL_MAX;
            minIndex = -1;
            for(int i = 0; i < k; i++){
                dist = clusters[i].getCentroid().l2Distance(point);
                if(dist <= minDist){
                    minDist = dist;
                    minIndex = i;
                }
            }
            clusters[minIndex].insertPoint(point);
        }
        for(int i = 0; i < k; i++){
            clusters[i].recenter();
            if(iter<9)clusters[i].clearList();
        }
    }

    for(int i = 0; i < k; i ++){
        std::cout << "Items in cluster " << i << ": " << clusters[i].count() << std::endl;
    }
    return 0;
}