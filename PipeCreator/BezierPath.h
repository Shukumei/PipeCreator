#pragma once

#include <Windows.h>
#include <vector>
#include <stack>
//#include <cmath>

#include "Vector4.h"
#include "Matrix4.h"

using namespace std;

struct loadedPoint {
	Vector4 position;
	Vector4 tangent;
	Vector4 normal;
};

	/**
	Class for representing a Bezier path, and methods for getting suitable points to
	draw the curve with line segments.
	*/
	class BezierPath
	{
	private:
		const float DISTANCE_BETWEEN_POINTS = 1.0;

		vector<Vector4>* controlPoints;

		Vector4 prevTangent;
		Vector4 prevNormal;
		bool firstTangent;

		int curveCount; //how many bezier curves in this path?

		void Interpolate(vector<Vector4>*);
		Vector4 CalculateBezierPoint(int, float);
		Vector4 CalculateBezierPoint(float, Vector4, Vector4, Vector4, Vector4);
		loadedPoint CalculateBezierPoint(int, float, int);
		loadedPoint CalculateBezierPoint(float, Vector4, Vector4, Vector4, Vector4, int);
		double bezier_length(Vector4, Vector4, Vector4, Vector4);
		double bezier_length(int);
	public:
		BezierPath(vector<Vector4>*);
		~BezierPath();

		vector<Vector4>* GetControlPoints();
		void GetDrawingPoints0(vector<loadedPoint>*);
		void GetDrawingPoints1(vector<loadedPoint>*);
	};