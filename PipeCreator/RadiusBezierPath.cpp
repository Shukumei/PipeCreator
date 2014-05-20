/**

This class demonstrates the code discussed in these two articles:

http://devmag.org.za/2011/04/05/bzier-curves-a-tutorial/
http://devmag.org.za/2011/06/23/bzier-path-algorithms/

Use this code as you wish, at your own risk. If it blows up
your computer, makes a plane crash, or otherwise cause damage,
injury, or death, it is not my fault.

@author Herman Tulleken, dev.mag.org.za

*/

#include "RadiusBezierPath.h"

using namespace std;


/**
Constructs a new empty Bezier curve. Use one of these methods
to add points: SetControlPoints, Interpolate, SamplePoints.
*/
RadiusBezierPath::RadiusBezierPath(vector<radiusPoint>* segmentPoints, vector<int>* segmentsLengthVec)
{
	controlPoints = new vector<radiusPoint>();
	segmentForCurveVec = new vector<int>();
	//skip 0 since that is 0
	for (int i = 1; i < segmentsLengthVec->size(); i++)
	{
		segmentForCurveVec->push_back((*segmentsLengthVec)[i]);
	}
	Interpolate(segmentPoints);
}


RadiusBezierPath::~RadiusBezierPath()
{
	delete controlPoints;
}


/**
Returns the control points for this Bezier curve.
*/
vector<radiusPoint>* RadiusBezierPath::GetControlPoints()
{
	return controlPoints;
}

/**
Calculates a Bezier interpolated path for the given points.
*/
// I'm using the w value of the vector4 to store the scale
void RadiusBezierPath::Interpolate(vector<radiusPoint>* segmentPoints)
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
			radiusPoint p1 = (*segmentPoints)[i];
			radiusPoint p2 = (*segmentPoints)[i + 1];

			scale = p2.point.w();

			Vector4 tangent = (p2.point - p1.point);
			radiusPoint q1;
			q1.point = p1.point + tangent * scale;

			controlPoints->push_back(p1);
			controlPoints->push_back(q1);
		}
		else if (i == segmentPoints->size() - 1) //last
		{
			radiusPoint p0 = (*segmentPoints)[i - 1];
			radiusPoint p1 = (*segmentPoints)[i];

			scale = p1.point.w();

			Vector4 tangent = (p1.point - p0.point);
			radiusPoint q0;
			q0.point = p1.point - tangent * scale;

			controlPoints->push_back(q0);
			controlPoints->push_back(p1);
		}
		else
		{
			radiusPoint p0 = (*segmentPoints)[i - 1];
			radiusPoint p1 = (*segmentPoints)[i];
			radiusPoint p2 = (*segmentPoints)[i + 1];

			scale = p1.point.w();

			Vector4 tangent = (p2.point - p0.point);
			tangent.normalize();
			radiusPoint q0;
			q0.point = p1.point - tangent * scale * (p1.point - p0.point).length();
			radiusPoint q1;
			q1.point = p1.point + tangent * scale * (p2.point - p1.point).length();

			controlPoints->push_back(q0);
			controlPoints->push_back(p1);
			controlPoints->push_back(q1);
		}
	}
}


/**
Gets the drawing points. This implementation simply calculates a certain number
of points per curve.

This is a slightly different inplementation from the one above.
*/
void RadiusBezierPath::GetDrawingPoints(vector<radiusPoint>* drawingPoints)
{
	drawingPoints->clear();

	int segmentsForCurve = 10;
	int segmentInc = 0;

	for (int i = 0; i < controlPoints->size() - 3; i += 3)
	{
		radiusPoint p0 = (*controlPoints)[i];
		radiusPoint p1 = (*controlPoints)[i + 1];
		radiusPoint p2 = (*controlPoints)[i + 2];
		radiusPoint p3 = (*controlPoints)[i + 3];

		segmentsForCurve = (*segmentForCurveVec)[segmentInc];
		segmentInc++;

		if (i == 0) //only do this for the first end point. When i != 0, this coincides with the end point of the previous segment,
		{
			drawingPoints->push_back(CalculateBezierPoint(0, p0, p1, p2, p3, segmentsForCurve));
		}

		for (int j = 1; j <= segmentsForCurve; j++)
		{
			float t = j / (float)segmentsForCurve;
			drawingPoints->push_back(CalculateBezierPoint(t, p0, p1, p2, p3, segmentsForCurve));
		}
	}
}

/**
Caluclates a point on the Bezier curve represented with the four controlpoints given.
*/
radiusPoint RadiusBezierPath::CalculateBezierPoint(float t, radiusPoint p0, radiusPoint p1, radiusPoint p2, radiusPoint p3, int numSegments)
{
	float t2 = 0.1 / numSegments;

	float u = 1 - t;
	float tt = t * t;
	float uu = u * u;
	float uuu = uu * u;
	float ttt = tt * t;

	radiusPoint p;
	p.point = p0.point * uuu; //first term

	p.point += p1.point * 3 * uu * t; //second term
	p.point += p2.point * 3 * u * tt; //third term
	p.point += p3.point * ttt; //fourth term

	u = 1 - t2;
	tt = t2 * t2;
	uu = u * u;
	uuu = uu * u;
	ttt = tt * t2;

	radiusPoint pTwo;
	pTwo.point = p0.point * uuu; //first term

	pTwo.point += p1.point * 3 * uu * t2; //second term
	pTwo.point += p2.point * 3 * u * tt; //third term
	pTwo.point += p3.point * ttt; //fourth term

	p.tangent = pTwo.point - p.point;
	p.tangent.normalize();

	return p;

}