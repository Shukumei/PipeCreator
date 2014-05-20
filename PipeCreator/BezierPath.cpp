/**

This class demonstrates the code discussed in these two articles:

http://devmag.org.za/2011/04/05/bzier-curves-a-tutorial/
http://devmag.org.za/2011/06/23/bzier-path-algorithms/

Use this code as you wish, at your own risk. If it blows up
your computer, makes a plane crash, or otherwise cause damage,
injury, or death, it is not my fault.

@author Herman Tulleken, dev.mag.org.za

*/

#include "BezierPath.h"

using namespace std;


/**
Constructs a new empty Bezier curve. Use one of these methods
to add points: SetControlPoints, Interpolate, SamplePoints.
*/
BezierPath::BezierPath(vector<Vector4>* segmentPoints)
{
	controlPoints = new vector<Vector4>();
	Interpolate(segmentPoints);
}


BezierPath::~BezierPath()
{
	delete controlPoints;
}


/**
Returns the control points for this Bezier curve.
*/
vector<Vector4>* BezierPath::GetControlPoints()
{
	return controlPoints;
}

/**
Calculates a Bezier interpolated path for the given points.
*/
// I'm using the w value of the vector4 to store the scale
void BezierPath::Interpolate(vector<Vector4>* segmentPoints)
{
	controlPoints->clear();

	if (segmentPoints->size() < 2)
	{
		return;
	}

	float scale = 1.0;

	for (int i = 0; i < segmentPoints->size(); i++)
	{
		if (i == 0) // is first
		{
			Vector4 p1 = (*segmentPoints)[i];
			Vector4 p2 = (*segmentPoints)[i + 1];

			scale = p2.w();

			Vector4 tangent = (p2 - p1);
			Vector4 q1 = p1 + tangent * scale;

			controlPoints->push_back(p1);
			controlPoints->push_back(q1);
		}
		else if (i == segmentPoints->size() - 1) //last
		{
			Vector4 p0 = (*segmentPoints)[i - 1];
			Vector4 p1 = (*segmentPoints)[i];

			scale = p1.w();

			Vector4 tangent = (p1 - p0);
			Vector4 q0 = p1 - tangent * scale;

			controlPoints->push_back(q0);
			controlPoints->push_back(p1);
		}
		else
		{
			Vector4 p0 = (*segmentPoints)[i - 1];
			Vector4 p1 = (*segmentPoints)[i];
			Vector4 p2 = (*segmentPoints)[i + 1];

			scale = p1.w();

			Vector4 tangent = (p2 - p0);
			tangent.normalize();
			Vector4 q0 = p1 - tangent * scale * (p1 - p0).length();
			Vector4 q1 = p1 + tangent * scale * (p2 - p1).length();

			controlPoints->push_back(q0);
			controlPoints->push_back(p1);
			controlPoints->push_back(q1);
		}
	}

	curveCount = (controlPoints->size() - 1) / 3;
}

/**
Caluclates a point on the path.

@param curveIndex The index of the curve that the point is on. For example,
the second curve (index 1) is the curve with controlpoints 3, 4, 5, and 6.

@param t The paramater indicating where on the curve the point is. 0 corresponds
to the "left" point, 1 corresponds to the "right" end point.
*/
Vector4 BezierPath::CalculateBezierPoint(int curveIndex, float t)
{
	int nodeIndex = curveIndex * 3;

	Vector4 p0 = (*controlPoints)[nodeIndex];
	Vector4 p1 = (*controlPoints)[nodeIndex + 1];
	Vector4 p2 = (*controlPoints)[nodeIndex + 2];
	Vector4 p3 = (*controlPoints)[nodeIndex + 3];

	return CalculateBezierPoint(t, p0, p1, p2, p3);
}

/**
Gets the drawing points. This implementation simply calculates a certain number
of points per curve.
*/
void BezierPath::GetDrawingPoints0(vector<Vector4>* drawingPoints)
{
	drawingPoints->clear();

	int segmentsForCurve = 10;

	for (int curveIndex = 0; curveIndex < curveCount; curveIndex++)
	{
		if (curveIndex == 0) //Only do this for the first end point. 
			//When i != 0, this coincides with the 
			//end point of the previous segment,
		{
			drawingPoints->push_back(CalculateBezierPoint(curveIndex, 0));
		}

		double distance = bezier_length(curveIndex);

		segmentsForCurve = (int)(distance / DISTANCE_BETWEEN_POINTS);

		for (int j = 1; j <= segmentsForCurve; j++)
		{
			float t = j / (float)segmentsForCurve;
			drawingPoints->push_back(CalculateBezierPoint(curveIndex, t));
		}
	}
}

/**
Gets the drawing points. This implementation simply calculates a certain number
of points per curve.

This is a slightly different inplementation from the one above.
*/
void BezierPath::GetDrawingPoints1(vector<Vector4>* drawingPoints, vector<int>* sectionLengthsVec)
{
	drawingPoints->clear();

	int segmentsForCurve = 10;

	for (int i = 0; i < controlPoints->size() - 3; i += 3)
	{
		Vector4 p0 = (*controlPoints)[i];
		Vector4 p1 = (*controlPoints)[i + 1];
		Vector4 p2 = (*controlPoints)[i + 2];
		Vector4 p3 = (*controlPoints)[i + 3];

		if (i == 0) //only do this for the first end point. When i != 0, this coincides with the end point of the previous segment,
		{
			drawingPoints->push_back(CalculateBezierPoint(0, p0, p1, p2, p3));
		}

		double distance = bezier_length(p0, p1, p2, p3);

		segmentsForCurve = (int)(distance / DISTANCE_BETWEEN_POINTS);

		sectionLengthsVec->push_back(segmentsForCurve);

		for (int j = 1; j <= segmentsForCurve; j++)
		{
			float t = j / (float)segmentsForCurve;
			drawingPoints->push_back(CalculateBezierPoint(t, p0, p1, p2, p3));
		}
	}
}

/**
Caluclates a point on the Bezier curve represented with the four controlpoints given.
*/
Vector4 BezierPath::CalculateBezierPoint(float t, Vector4 p0, Vector4 p1, Vector4 p2, Vector4 p3)
{
	float u = 1 - t;
	float tt = t * t;
	float uu = u * u;
	float uuu = uu * u;
	float ttt = tt * t;

	Vector4 p = p0 * uuu; //first term

	p += p1 * 3 * uu * t; //second term
	p += p2 * 3 * u * tt; //third term
	p += p3 * ttt; //fourth term

	return p;

}

#define STEPS 10

/*
Approximate length of the Bezier curve which starts at "start" and
is defined by "c". According to Computing the Arc Length of Cubic Bezier Curves
there is no closed form integral for it.
*/

double BezierPath::bezier_length(Vector4 p0, Vector4 p1, Vector4 p2, Vector4 p3)
{
	double t;
	int i;
	int steps;
	Vector4 dot;
	Vector4 previous_dot;
	double length = 0.0;
	steps = STEPS;
	for (i = 0; i <= steps; i++) {
		t = (double)i / (double)steps;
		dot = CalculateBezierPoint(t, p0, p1, p2, p3);
		if (i > 0) {
			double x_diff = dot.x() - previous_dot.x();
			double y_diff = dot.y() - previous_dot.y();
			double z_diff = dot.z() - previous_dot.z();
			length += sqrt(x_diff * x_diff + y_diff * y_diff + z_diff * z_diff);
		}
		previous_dot = dot;
	}
	return length;
}

double BezierPath::bezier_length(int curveIndex)
{
	double t;
	int i;
	int steps;
	Vector4 dot;
	Vector4 previous_dot;
	double length = 0.0;
	steps = STEPS;
	for (i = 0; i <= steps; i++) {
		t = (double)i / (double)steps;
		dot = CalculateBezierPoint(curveIndex, t);
		if (i > 0) {
			double x_diff = dot.x() - previous_dot.x();
			double y_diff = dot.y() - previous_dot.y();
			double z_diff = dot.z() - previous_dot.z();
			length += sqrt(x_diff * x_diff + y_diff * y_diff + z_diff * z_diff);
		}
		previous_dot = dot;
	}
	return length;
}