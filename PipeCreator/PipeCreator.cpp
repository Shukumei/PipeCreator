#include "BezierPath.h"

#include <iostream>
#include <fstream>
using namespace std;

#define M_PI 3.14159265358979323846

#define NUMPOINTSINCIRCLE 20
#define RADIUS 100

#define PATHPOINTSPERGAP 120

// multiply path points per gap by 2, divide gaps per onetex by 2
#define GAPSPERONETEX 2.5

struct Vertex {
	float point[3];
	float texCoord[2];
	float normal[3];
};

void createTubeCircle(loadedPoint current, vector<Vertex>* finalPoints)
{
	float angleInc = (M_PI * 2.0) / NUMPOINTSINCIRCLE;

	int round = RADIUS / 15;
	if (round < 1.0)
	{
		round = 1;
	}
	float texInc = ((float) round) / (float) NUMPOINTSINCIRCLE;

	Vertex resVertex;
	Vector4 pointPosition;
	Vector4 subtract;

	for (int i = 0; i < (NUMPOINTSINCIRCLE + 1); i++)
	{
		float rotAngle = angleInc * i;

		if (i == NUMPOINTSINCIRCLE)
		{
			rotAngle = 0.0;
		}

		Matrix4 M = Matrix4::rotate(current.tangent, rotAngle);
		Vector4 dir = M * current.normal;
		dir.normalize();

		pointPosition.set(current.position.x(), current.position.y(), current.position.z());
		pointPosition.add(dir*RADIUS);

		resVertex.point[0] = pointPosition.x();
		resVertex.point[1] = pointPosition.y();
		resVertex.point[2] = pointPosition.z();

		subtract.set(current.position.x(), current.position.y(), current.position.z());
		subtract.subtract(pointPosition);
		subtract.normalize();

		resVertex.normal[0] = subtract.x();
		resVertex.normal[1] = subtract.y();
		resVertex.normal[2] = subtract.z();

		//do texture coords here
		resVertex.texCoord[0] = (float)i * texInc;
		resVertex.texCoord[1] = 0.0f;

		//using the loadedPoint tangent, you could set the vertex's tangent here


		finalPoints->push_back(resVertex);
	}
}

void quadToTriangle(FILE* myFile, int v0Index, int v1Index, int v2Index, int v3Index){

	fprintf(myFile, "f %d/%d/%d %d/%d/%d %d/%d/%d\n", v0Index, v0Index, v0Index, v1Index, v1Index, v1Index, v2Index, v2Index, v2Index);

	fprintf(myFile, "f %d/%d/%d %d/%d/%d %d/%d/%d\n", v0Index, v0Index, v0Index, v2Index, v2Index, v2Index, v3Index, v3Index, v3Index);
}


int main() {

	vector<Vector4>* startTrackPoints = new vector<Vector4>();
	vector<loadedPoint>* finishedPathPoints = new vector<loadedPoint>();
	vector<loadedPoint>* pathPointsForCircles = new vector<loadedPoint>();

	//add points for track

	Vector4 point;

	//w value is scale

	point.set(0.0f, 0.0f, 0.0f, 0.5f);
	startTrackPoints->push_back(point);

	point.set(2000.0f, 0.0f, -2000.0f, 0.5f);
	startTrackPoints->push_back(point);

	point.set(4000.0f, 0.0f, -4000.0f, 0.5f);
	startTrackPoints->push_back(point);

	point.set(6000.0f, 2000.0f, -3000.0f, 0.5f);
	startTrackPoints->push_back(point);

	point.set(7000.0f, -2000.0f, 0.0f, 0.5f);
	startTrackPoints->push_back(point);

	point.set(5000.0f, 0.0f, 2000.0f, 0.5f);
	startTrackPoints->push_back(point);

	point.set(3000.0f, 0.0f, 3000.0f, 0.5f);
	startTrackPoints->push_back(point);

	point.set(-1000.0f, 0.0f, 1000.0f, 0.5f);
	startTrackPoints->push_back(point);

	point.set(0.0f, 0.0f, 0.0f, 0.5f);
	startTrackPoints->push_back(point);


	BezierPath* path = new BezierPath(startTrackPoints);
	path->GetDrawingPoints1(finishedPathPoints);

	// Reduce number of circle points
	bool divideByPointsPerGap = true;
	int totalPath = finishedPathPoints->size();
	int modPath = totalPath % PATHPOINTSPERGAP;
	if (modPath != 0)
	{
		divideByPointsPerGap = false;
	}

	int gapPoints = totalPath / PATHPOINTSPERGAP;

	for (int g = 0; g < gapPoints; g++) {
		pathPointsForCircles->push_back((*finishedPathPoints)[g*PATHPOINTSPERGAP]);
	}

	if (!divideByPointsPerGap) {
		pathPointsForCircles->push_back((*finishedPathPoints)[finishedPathPoints->size() - 1]);
	}


	//TODO : Reducing the number of points will change the normals used to orient the circles
	// Might need to go and have it reference the equivalent finishedPathPoints for the normals


	//create obj file from points

	vector<Vertex>* tubeVertices = new vector<Vertex>();

	for (int i = 0; i < pathPointsForCircles->size() - 1; i++)
	{
		createTubeCircle((*pathPointsForCircles)[i], tubeVertices);
	}

	int totalPointsPerCircle = NUMPOINTSINCIRCLE + 1;

	Vertex repeatPoint;
	for (int t = 0; t < totalPointsPerCircle; t++){
		repeatPoint = (*tubeVertices)[t];
		tubeVertices->push_back(repeatPoint);
	}

	FILE * myFile;
	
	errno_t errorCode = fopen_s(&myFile, "example.obj", "w");
	if (errorCode == 0)
	{
		//vertices
		for (int j = 0; j < tubeVertices->size(); j++)
		{
			fprintf(myFile, "v %.6f %.6f %.6f\n", (*tubeVertices)[j].point[0], (*tubeVertices)[j].point[1], (*tubeVertices)[j].point[2]);
		}

		//texture coordinates
		int round = pathPointsForCircles->size() / GAPSPERONETEX;

		float vTexInc = ((float)round) / (pathPointsForCircles->size() - 1.0);
		for (int q = 0; q < pathPointsForCircles->size(); q++)
		{
			for (int r = 0; r < totalPointsPerCircle; r++)
			{
				fprintf(myFile, "vt %.6f %.6f\n", (*tubeVertices)[q*totalPointsPerCircle + r].texCoord[0], vTexInc*q);
			}
		}

		//normals
		for (int l = 0; l < tubeVertices->size(); l++)
		{
			fprintf(myFile, "vn %.6f %.6f %.6f\n", (*tubeVertices)[l].normal[0], (*tubeVertices)[l].normal[1], (*tubeVertices)[l].normal[2]);
		}

		int jump = totalPointsPerCircle;
		int count = 1;
		int loopInc;
		//faces
		for (int m = 0; m <= pathPointsForCircles->size() - 2; m++)
		{
			loopInc = m*jump;
			for (int n = 1; n < jump; n++) //indices start at 1 for obj files
			{
				int val = n + loopInc;
				fprintf(myFile, "s %d\n", count++);
				quadToTriangle(myFile, val, val + 1, val + 1 + jump, val + jump);
			}
		}

		/*loopInc += jump;

		for (int p = 1; p < jump; p++)
		{
			int val = p + loopInc;
			fprintf(myFile, "s %d\n", count++);
			quadToTriangle(myFile, val, val + 1, p + 1, p);
		}*/


		fclose(myFile);
	}
	else cout << "Unable to open file";

	delete startTrackPoints;
	delete finishedPathPoints;
	delete pathPointsForCircles;
	delete path;
	delete tubeVertices;

	return 0;
}