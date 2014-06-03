#include "BezierPath.h"
#include "RadiusBezierPath.h"
#include "Matrix4.h"

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
	float tangent[3];
};

struct VecPlusRad {
	Vector4 point;
	float radius;
	Vector4 radiusTangent;
};

void circleVerts(int approx, float radius, vector<Vector4>* storeVec)
{
	const double angle((M_PI * 2.0) / (double)approx);
	int idx;
	double x(0.0), y(0.0), z(0.0);
	double original_radius = radius;

	double firstX = radius;
	double firstY = 0.0;

	for (idx = 0; idx<approx; idx++)
	{
		double cosAngle = cos(idx*angle);
		double sinAngle = sin(idx*angle);

		x = radius*cosAngle;
		y = radius*sinAngle;

		storeVec->push_back(Vector4(x, y, z, 1.0));
	}
	//duplicating the first vector for textures
	storeVec->push_back(Vector4(firstX, firstY, 0.0, 1.0));
}

void createTubeCircle(Vector4 current, Vector4 next, Vector4 radiusTangent, vector<Vector4>* circlePoints, vector<Vertex>* finalPoints)
{
	Vector4 circleNormal(0.0, 0.0, -1.0);
	Vector4 defaultRadiusTan(1.0, 0.0, 0.0);
	float cosine;
	float sine;
	float t;
	Vector4 rotationAxis;
	Matrix4 rotationMat;
	Matrix4 tanRotationMat;
	Matrix4 translationMat;
	//float fillArray[4][4];

	//calculate new normal for each point
	Vector4 normal = next - current;

	normal.normalize();

	float angle = acos(circleNormal.dot(normal));
	cosine = circleNormal.dot(normal);
	sine = sqrt(1 - cosine*cosine);
	t = 1 - cosine;
	rotationAxis.set(circleNormal.x(), circleNormal.y(), circleNormal.z());
	rotationAxis.cross(normal);
	rotationAxis.normalize();

	/*fillArray[0][0] = t*rotationAxis.x()*rotationAxis.x() + cosine;
	fillArray[0][1] = t*rotationAxis.x()*rotationAxis.y() - rotationAxis.z()*sine;
	fillArray[0][2] = t*rotationAxis.x()*rotationAxis.z() + rotationAxis.y()*sine;
	fillArray[0][3] = p2.x();

	fillArray[1][0] = t*rotationAxis.x()*rotationAxis.y() + rotationAxis.z()*sine;
	fillArray[1][1] = t*rotationAxis.y()*rotationAxis.y() + cosine;
	fillArray[1][2] = t*rotationAxis.y()*rotationAxis.z() - rotationAxis.x()*sine;
	fillArray[1][3] = p2.y();

	fillArray[2][0] = t*rotationAxis.x()*rotationAxis.z() - rotationAxis.y()*sine;
	fillArray[2][1] = t*rotationAxis.y()*rotationAxis.z() + rotationAxis.x()*sine;
	fillArray[2][2] = t*rotationAxis.z()*rotationAxis.z() + cosine;
	fillArray[2][3] = p2.z();

	fillArray[3][0] = 0.0;
	fillArray[3][1] = 0.0;
	fillArray[3][2] = 0.0;
	fillArray[3][3] = 1.0;

	transformMat = Matrix4(fillArray);*/
	//Matrix4 compare;

	rotationMat = Matrix4::rotate(rotationAxis, angle);
	translationMat = Matrix4::translate(current);

	Vector4 preResult;
	Vector4 result;
	Vector4 subtract;
	Vertex resVertex;

	Vector4 rotAxis;

	int round = RADIUS / 15;
	if (round < 1.0)
	{
		round = 1;
	}
	float texInc = ((float) round) / (circlePoints->size()-1.0);
	for (int i = 0; i < circlePoints->size(); i++)
	{
		preResult = rotationMat * (*circlePoints)[i];
		result = translationMat * preResult;
		resVertex.point[0] = result.x();
		resVertex.point[1] = result.y();
		resVertex.point[2] = result.z();

		subtract.set(current.x(), current.y(), current.z());
		subtract.subtract(result);
		subtract.normalize();

		resVertex.normal[0] = subtract.x();
		resVertex.normal[1] = subtract.y();
		resVertex.normal[2] = subtract.z();

		//do texture coords here
		resVertex.texCoord[0] = (float)i * texInc;
		resVertex.texCoord[1] = 0.0f;

		subtract.cross(normal);
		rotAxis = subtract;
		rotAxis.normalize();

		float rotTanAngle = acos(defaultRadiusTan.dot(radiusTangent));

		tanRotationMat = Matrix4::rotate(rotAxis, rotTanAngle);
		Vector4 finalTan = tanRotationMat * normal;

		resVertex.tangent[0] = finalTan.x();
		resVertex.tangent[1] = finalTan.y();
		resVertex.tangent[2] = finalTan.z();

		finalPoints->push_back(resVertex);
	}
}

void quadToTriangle(FILE* myFile, int v0Index, int v1Index, int v2Index, int v3Index){

	fprintf(myFile, "f %d/%d/%d/%d %d/%d/%d/%d %d/%d/%d/%d\n", v0Index, v0Index, v0Index, v0Index, v1Index, v1Index, v1Index, v1Index, v2Index, v2Index, v2Index, v2Index);

	fprintf(myFile, "f %d/%d/%d/%d %d/%d/%d/%d %d/%d/%d/%d\n", v0Index, v0Index, v0Index, v0Index, v2Index, v2Index, v2Index, v2Index, v3Index, v3Index, v3Index, v3Index);
}


int main() {

	vector<Vector4>* startTrackPoints = new vector<Vector4>();
	vector<float>* trackPointsRadii = new vector<float>();
	vector<Vector4>* finishedPathPointsNoRad = new vector<Vector4>();
	vector<radiusPoint>* finishedRadiusPathPoints = new vector<radiusPoint>();

	vector<VecPlusRad>* finishedPathPointsWithRad = new vector<VecPlusRad>();
	vector<VecPlusRad>* pathPointsForCircles = new vector<VecPlusRad>();

	vector<int>* numSegmentsVec = new vector<int>();

	//add points for track

	Vector4 point;
	float radius;

	//w value is scale

	point.set(0.0f, 0.0f, 0.0f, 0.5f);
	radius = 100.0;
	startTrackPoints->push_back(point);
	trackPointsRadii->push_back(radius);

	point.set(0.0f, 0.0f, -1000.0f, 0.5f);
	radius = 100.0;
	startTrackPoints->push_back(point);
	trackPointsRadii->push_back(radius);

	point.set(-1000.0f, 0.0f, -2500.0f, 0.5f);
	radius = 100.0;
	startTrackPoints->push_back(point);
	trackPointsRadii->push_back(radius);

	point.set(-2000.0f, 0.0f, -3000.0f, 0.5f);
	radius = 100.0;
	startTrackPoints->push_back(point);
	trackPointsRadii->push_back(radius);

	point.set(-4000.0f, 0.0f, -4000.0f, 0.5f);
	radius = 100.0;
	startTrackPoints->push_back(point);
	trackPointsRadii->push_back(radius);

	point.set(-2000.0f, 0.0f, -5000.0f, 0.5f);
	radius = 100.0;
	startTrackPoints->push_back(point);
	trackPointsRadii->push_back(radius);

	point.set(1000.0f, -1000.0f, -5000.0f, 0.5f);
	radius = 100.0;
	startTrackPoints->push_back(point);
	trackPointsRadii->push_back(radius);

	point.set(3000.0f, -1000.0f, -5000.0f, 0.5f);
	radius = 100.0;
	startTrackPoints->push_back(point);
	trackPointsRadii->push_back(radius);

	point.set(4000.0f, -0.0f, -6500.0f, 0.5f);
	radius = 100.0;
	startTrackPoints->push_back(point);
	trackPointsRadii->push_back(radius);

	point.set(3500.0f, 1000.0f, -8000.0f, 0.5f);
	radius = 100.0;
	startTrackPoints->push_back(point);
	trackPointsRadii->push_back(radius);

	point.set(2500.0f, -0.0f, -9500.0f, 0.5f);
	radius = 100.0;
	startTrackPoints->push_back(point);
	trackPointsRadii->push_back(radius);

	point.set(3500.0f, -1000.0f, -9500.0f, 0.5f);
	radius = 100.0;
	startTrackPoints->push_back(point);
	trackPointsRadii->push_back(radius);

	point.set(6000.0f, 0.0f, -4000.0f, 0.5f);
	radius = 100.0;
	startTrackPoints->push_back(point);
	trackPointsRadii->push_back(radius);

	point.set(8000.0f, 0.0f, -3000.0f, 0.5f);
	radius = 100.0;
	startTrackPoints->push_back(point);
	trackPointsRadii->push_back(radius);

	point.set(7000.0f, 0.0f, -2000.0f, 0.5f);
	radius = 100.0;
	startTrackPoints->push_back(point);
	trackPointsRadii->push_back(radius);

	point.set(9000.0f, 0.0f, 0.0f, 0.5f);
	radius = 100.0;
	startTrackPoints->push_back(point);
	trackPointsRadii->push_back(radius);

	point.set(7000.0f, 0.0f, 2000.0f, 0.5f);
	radius = 100.0;
	startTrackPoints->push_back(point);
	trackPointsRadii->push_back(radius);

	point.set(9000.0f, 1000.0f, 4000.0f, 0.5f);
	radius = 100.0;
	startTrackPoints->push_back(point);
	trackPointsRadii->push_back(radius);

	point.set(7000.0f, -500.0f, 5000.0f, 0.5f);
	radius = 100.0;
	startTrackPoints->push_back(point);
	trackPointsRadii->push_back(radius);

	point.set(7000.0f, -1500.0f, 3000.0f, 0.5f);
	radius = 100.0;
	startTrackPoints->push_back(point);
	trackPointsRadii->push_back(radius);

	point.set(7000.0f, -3000.0f, 6000.0f, 0.5f);
	radius = 100.0;
	startTrackPoints->push_back(point);
	trackPointsRadii->push_back(radius);

	point.set(5000.0f, -2500.0f, 4000.0f, 0.5f);
	radius = 100.0;
	startTrackPoints->push_back(point);
	trackPointsRadii->push_back(radius);

	point.set(3000.0f, 2000.0f, 3000.0f, 0.5f);
	radius = 100.0;
	startTrackPoints->push_back(point);
	trackPointsRadii->push_back(radius);

	point.set(1000.0f, -500.0f, 2000.0f, 0.5f);
	radius = 100.0;
	startTrackPoints->push_back(point);
	trackPointsRadii->push_back(radius);

	point.set(0.0f, 0.0f, 1000.0f, 0.5f);
	radius = 100.0;
	startTrackPoints->push_back(point);
	trackPointsRadii->push_back(radius);

	point.set(0.0f, 0.0f, 0.0f, 0.5f);
	radius = 100.0;
	startTrackPoints->push_back(point);
	trackPointsRadii->push_back(radius);


	BezierPath* path = new BezierPath(startTrackPoints);
	numSegmentsVec->push_back(0); //first radii curves value has 0 for x
	path->GetDrawingPoints1(finishedPathPointsNoRad, numSegmentsVec);


	vector<radiusPoint>* radiusTrackPoints = new vector<radiusPoint>();

	radiusPoint rPoint;

	float xPos = 0;
	for (int i = 0; i < startTrackPoints->size(); i++)
	{
		xPos += (*numSegmentsVec)[i];
		rPoint.point.set(xPos, (*trackPointsRadii)[i], 0.0, 0.5);
		radiusTrackPoints->push_back(rPoint);
	}

	RadiusBezierPath* radiusPath = new RadiusBezierPath(radiusTrackPoints, numSegmentsVec);
	radiusPath->GetDrawingPoints(finishedRadiusPathPoints);

	VecPlusRad input;

	for (int k = 0; k < finishedPathPointsNoRad->size(); k++)
	{
		input.point = (*finishedPathPointsNoRad)[k];
		input.radius = (*finishedRadiusPathPoints)[k].point.y();
		input.radiusTangent = (*finishedRadiusPathPoints)[k].tangent;

		finishedPathPointsWithRad->push_back(input);
	}


	// Reduce number of circle points
	bool divideByPointsPerGap = true;
	int totalPath = finishedPathPointsWithRad->size();
	int modPath = totalPath % PATHPOINTSPERGAP;
	if (modPath != 0)
	{
		divideByPointsPerGap = false;
	}

	int gapPoints = totalPath / PATHPOINTSPERGAP;

	for (int g = 0; g < gapPoints; g++) {
		pathPointsForCircles->push_back((*finishedPathPointsWithRad)[g*PATHPOINTSPERGAP]);
	}

	if (!divideByPointsPerGap) {
		pathPointsForCircles->push_back((*finishedPathPointsWithRad)[finishedPathPointsWithRad->size() - 1]);
	}


	//TODO : Reducing the number of points will change the normals used to orient the circles
	// Might need to go and have it reference the equivalent finishedPathPoints for the normals


	//create obj file from points
	vector<Vector4>* circleVec = new vector<Vector4>();

	vector<Vertex>* tubeVertices = new vector<Vertex>();

	int test = 2;

	for (int i = 0; i < pathPointsForCircles->size() - 1; i++)
	{
		circleVec = new vector<Vector4>();
		circleVerts(NUMPOINTSINCIRCLE, (*pathPointsForCircles)[i].radius, circleVec);

		if (i == 125)
		{
			test = 3;
		}
		createTubeCircle((*pathPointsForCircles)[i].point, (*finishedPathPointsWithRad)[i*PATHPOINTSPERGAP + 1].point, (*pathPointsForCircles)[i].radiusTangent, circleVec, tubeVertices);
		//Might cause issues when dividePointsPerGap 
	}


	Vertex repeatPoint;
	for (int t = 0; t < circleVec->size(); t++){
		repeatPoint = (*tubeVertices)[t];
		tubeVertices->push_back(repeatPoint);
	}

	FILE * myFile;
	
	errno_t errorCode = fopen_s(&myFile, "track.trk", "w");
	if (errorCode == 0)
	{
		//vertices
		for (int j = 0; j < tubeVertices->size(); j++)
		{
			fprintf(myFile, "v %.6f %.6f %.6f\n", (*tubeVertices)[j].point[0], (*tubeVertices)[j].point[1], (*tubeVertices)[j].point[2]);
		}

		//texture coordinates
		//for (int k = 0; k < tubeVertices->size(); k++)
		//{
			//fprintf(myFile, "vt %.6f %.6f\n", (*tubeVertices)[k].texCoord[0], (*tubeVertices)[k].texCoord[1]);
		//}

		//temp Tex for right now

		int round = pathPointsForCircles->size() / GAPSPERONETEX;

		float vTexInc = ((float)round) / (pathPointsForCircles->size() - 1.0);
		for (int q = 0; q < pathPointsForCircles->size(); q++)
		{
			for (int r = 0; r < circleVec->size(); r++)
			{
				fprintf(myFile, "vt %.6f %.6f\n", (*tubeVertices)[q*circleVec->size() + r].texCoord[0], vTexInc*q);
			}
		}

		//normals
		for (int l = 0; l < tubeVertices->size(); l++)
		{
			fprintf(myFile, "vn %.6f %.6f %.6f\n", (*tubeVertices)[l].normal[0], (*tubeVertices)[l].normal[1], (*tubeVertices)[l].normal[2]);
		}

		//tangents
		for (int j = 0; j < tubeVertices->size(); j++)
		{
			fprintf(myFile, "vx %.6f %.6f %.6f\n", (*tubeVertices)[j].tangent[0], (*tubeVertices)[j].tangent[1], (*tubeVertices)[j].tangent[2]);
		}


		int jump = circleVec->size();
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


	FILE * pathFile;

	errno_t errorCodePath = fopen_s(&pathFile, "track.path", "w");
	if (errorCodePath == 0)
	{
		//vertices   the -1 is to get rid of the original start point
		for (int j = 0; j < finishedPathPointsWithRad->size() -1; j++)
		{
			fprintf(pathFile, "v %.6f %.6f %.6f %.6f\n", (*finishedPathPointsWithRad)[j].point.x(), (*finishedPathPointsWithRad)[j].point.y(), -1.0 * (*finishedPathPointsWithRad)[j].point.z(), (*finishedPathPointsWithRad)[j].radius);
		}

		fclose(pathFile);
	}
	else cout << "Unable to open file";



	delete startTrackPoints;
	delete finishedPathPointsNoRad;
	delete finishedPathPointsWithRad;
	delete pathPointsForCircles;
	delete path;
	delete circleVec;
	delete tubeVertices;

	return 0;
}