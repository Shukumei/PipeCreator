#pragma once

#include <Windows.h>
#include <vector>
#include <stack>

#include "Vector4.h"

using namespace std;

	/**
	Class for representing a Bezier path, and methods for getting suitable points to
	draw the curve with line segments.
	*/
	class BezierPath
	{
	private:
		const float DISTANCE_BETWEEN_POINTS = 1.0;

		vector<Vector4>* controlPoints;

		int curveCount; //how many bezier curves in this path?

		void Interpolate(vector<Vector4>*);
		Vector4 CalculateBezierPoint(int, float);
		Vector4 CalculateBezierPoint(float, Vector4, Vector4, Vector4, Vector4);
		double bezier_length(Vector4, Vector4, Vector4, Vector4);
		double bezier_length(int);
	public:
		BezierPath(vector<Vector4>*);
		~BezierPath();

		vector<Vector4>* GetControlPoints();
		void GetDrawingPoints0(vector<Vector4>*);
		void GetDrawingPoints1(vector<Vector4>*, vector<int>*);
	};