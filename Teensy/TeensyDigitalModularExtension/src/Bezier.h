#pragma once

struct Point
{
	float x;
	float y;

	Point(const float x, const float y) { this->x = x; this->y = y; };

	Point operator*(const float& m)
	{
		return Point{ x * m, y * m };
	}

	Point operator+(const float& m)
	{
		return Point{ x + m, y + m };
	}

	Point operator+(const Point& m)
	{
		return Point{ x + m.x, y + m.y };
	}
};

class CubicBezier
{
	public:

		CubicBezier(const float& x1, const float& y1, const float& x2, const float& y2) : weightA(x1, y1), weightB(x2, y2) {};

		float evaluate(const float& x)
		{
			float a1 = weightA.y * x;
			float b1 = weightA.y * x + weightB.y * (1 - x);
			float c1 = weightB.y * x + (1 - x) * 1;
			float a2 = a1 * x + b1 * (1-x);
			float b2 = b1 * x + c1 * (1 - x);
			return (a2 * x + b2 * (1-x));
		}

		void adjustWeightA(const float& x, const float& y)
		{
			weightA.x = x;
			weightA.y = y;
		}

		void adjustWeightB(const float& x, const float& y)
		{
			weightB.x = x;
			weightB.y = y;
		}

	private:
		Point weightA, weightB;
};

class QuadraticBezier
{
	public:
		QuadraticBezier(const float& x, const float& y) : weight(x, y) {};

		float evaluateAsFunction(const float& x)
		{
			float t = (weight.x == 0.5) ? x : (weight.x - sqrt(sq(weight.x) - 2 * weight.x * x + x)) / (2 * weight.x - 1);
			float ax = weight.y * t;
			float bx = weight.y * (1 - t) + t;
			return ax * (1 - t) + bx * t;
		}

		float evaluate(const float& x)
		{
			float ax = weight.y * x;
			float bx = weight.y * (1 - x) + x;
			return ax * (1 - x) + bx * x;
		}

		void adjustWeight(const float& x, const float& y)
		{
			weight.x = x;
			weight.y = y;
		}

	private:
		Point weight;
};
