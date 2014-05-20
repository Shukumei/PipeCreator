#pragma once

#include <Windows.h>
#include <vector>
#include <stack>

#include "Vector4.h"

struct radiusPoint {
	Vector4 point;
	Vector4 tangent;
};

using namespace std;

	/**
	Class for representing a Bezier path, and methods for getting suitable points to
	draw the curve with line segments.
	*/
	class RadiusBezierPath
	{
	private:
		vector<radiusPoint>* controlPoints;
		vector<int>* segmentForCurveVec;

		void Interpolate(vector<radiusPoint>*);
		radiusPoint CalculateBezierPoint(float, radiusPoint, radiusPoint, radiusPoint, radiusPoint, int);
	public:
		RadiusBezierPath(vector<radiusPoint>*, vector<int>*);
		~RadiusBezierPath();

		vector<radiusPoint>* GetControlPoints();
		void GetDrawingPoints(vector<radiusPoint>*);
	};