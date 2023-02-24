#define _USE_MATH_DEFINES
#include "canvas.h"
#include <cmath>
#include <cassert>
using namespace std;
using namespace agl;

Canvas::Canvas(int w, int h) : _canvas(w, h), currType(UNDEFINED)
{
}

Canvas::~Canvas()
{
	_canvas.cleanUp();
}

void Canvas::save(const std::string& filename)
{
   _canvas.save(filename);
}

void Canvas::begin(PrimitiveType type, bool fill)
{
	if (this->currType != UNDEFINED) {
		std::cout << "Must end previous shape to begin a new one." << std::endl;
		return;
	}
	this->filled = fill;
	this->currType = type;
}

void Canvas::end()
{
	currType = UNDEFINED;
	filled = false;
	vertices.clear();
}

void Canvas::draw_low(vert a, vert b) {
	int y = a.coordinate.second;
	int W = b.coordinate.first - a.coordinate.first;
	int H = b.coordinate.second - a.coordinate.second;
	int dy = 1;
	if (H < 0) {
		dy = -1;
		H = -H;
	}
	int F = 2 * H - W;
	Pixel c;
	for (int i = a.coordinate.first; i < b.coordinate.first; ++i) {
		float t = (sqrt(pow(i - a.coordinate.first, 2) + pow(y - a.coordinate.second, 2)) / sqrt(pow(b.coordinate.first - a.coordinate.first, 2) + pow(b.coordinate.second - a.coordinate.second, 2)));
		c.r = a.color.r + (b.color.r - a.color.r) * t;
		c.g = a.color.g + (b.color.g - a.color.g) * t;
		c.b = a.color.b + (b.color.b - a.color.b) * t;
		if (i >= _canvas.width() || y >= _canvas.height()) {
			_canvas.set(y, i, c);
		}
		_canvas.set(y, i, c);
		if (F > 0) {
			y += dy;
			F += 2 * (H - W);
		}
		else {
			F += 2 * H;
		}
	}
}

void Canvas::draw_high(vert a, vert b) 
{
	int x = a.coordinate.first;
	int W = b.coordinate.first - a.coordinate.first;
	int H = b.coordinate.second - a.coordinate.second;
	int dx = 1;
	if (W < 0) {
		dx = -1;
		W = -W;
	}
	int F = 2 * W - H;
	Pixel c;
	for (int i = a.coordinate.second; i < b.coordinate.second; ++i) {
		float t = (sqrt(pow(x - a.coordinate.first, 2) + pow(i - a.coordinate.second, 2)) / sqrt(pow(b.coordinate.first - a.coordinate.first, 2) + pow(b.coordinate.second - a.coordinate.second, 2)));
		c.r = a.color.r + (b.color.r - a.color.r) * t;
		c.g = a.color.g + (b.color.g - a.color.g) * t;
		c.b = a.color.b + (b.color.b - a.color.b) * t;
		if (x >= _canvas.width() || i >= _canvas.height()) {
			_canvas.set(i, x, c);
		}
		_canvas.set(i, x, c);
		if (F > 0) {
			x += dx;
			F += 2 * (W - H);
		}
		else {
			F += 2 * W;
		}
	}
}

void Canvas::draw_line(vert a, vert b) {
	int W = b.coordinate.first - a.coordinate.first;
	int H = b.coordinate.second - a.coordinate.second;
	if (abs(H) < abs(W)) {
		if (a.coordinate.first > b.coordinate.first) {
			vert tmp = a;
			a = b;
			b = tmp;
		}
		draw_low(a, b);
	}
	else {
		if (a.coordinate.second > b.coordinate.second) {
			vert tmp = a;
			a = b;
			b = tmp;
		}
		draw_high(a, b);
	}
}

void Canvas::draw_triangle(vert a, vert b, vert c)
{
	// sort vertices counterclockwise
	std::pair<int, int> centroid;
	centroid.first = (a.coordinate.first + b.coordinate.first + c.coordinate.first) / 3;
	centroid.second = (a.coordinate.second + b.coordinate.second + c.coordinate.second) / 3;
	float a1 = atan2(a.coordinate.second - centroid.second, a.coordinate.first - centroid.first) * (180 / M_PI) + 360 % 360;
	float a2 = atan2(b.coordinate.second - centroid.second, b.coordinate.first - centroid.first) * (180 / M_PI) + 360 % 360;
	float a3 = atan2(c.coordinate.second - centroid.second, c.coordinate.first - centroid.first) * (180 / M_PI) + 360 % 360;
	if (a1 < a2 && a1 < a3) {
		if (a3 < a2) {
			vert tmp = b;
			b = c;
			c = tmp;
		}
	}
	else if (a2 < a1 && a2 < a3) {
		vert tmp = a;
		a = b;
		if (a1 < a3) {
			b = tmp;
		}
		else {
			b = c;
			c = tmp;
		}
	}
	else if (a3 < a2 && a3 < a1) {
		vert tmp = a;
		a = c;
		if (a1 > a2) {
			c = b;
			b = tmp;
		}
		else {
			c = tmp;
		}
	}
	else { // a1 == a3
		vert tmp = b;
		b = c;
		c = tmp;
	}
	// draw triangle with no shared sides
	int xmax = max(max(a.coordinate.first, b.coordinate.first), c.coordinate.first);
	int xmin = min(min(a.coordinate.first, b.coordinate.first), c.coordinate.first);
	int ymax = max(max(a.coordinate.second, b.coordinate.second), c.coordinate.second);
	int ymin = min(min(a.coordinate.second, b.coordinate.second), c.coordinate.second);
	float falpha = (b.coordinate.second - c.coordinate.second) * a.coordinate.first + (c.coordinate.first - b.coordinate.first) * a.coordinate.second + b.coordinate.first * c.coordinate.second - c.coordinate.first * b.coordinate.second;
	float fbeta = (c.coordinate.second - a.coordinate.second) * b.coordinate.first + (a.coordinate.first - c.coordinate.first) * b.coordinate.second + c.coordinate.first * a.coordinate.second - a.coordinate.first * c.coordinate.second;
	float fgamma = (a.coordinate.second - b.coordinate.second) * c.coordinate.first + (b.coordinate.first - a.coordinate.first) * c.coordinate.second + a.coordinate.first * b.coordinate.second - b.coordinate.first * a.coordinate.second;
	if (!filled) {
		for (int i = ymin; i <= ymax; i++) {
			for (int j = xmin; j <= xmax; j++) {
				float alpha = ((b.coordinate.second - c.coordinate.second) * j + (c.coordinate.first - b.coordinate.first) * i + b.coordinate.first * c.coordinate.second - c.coordinate.first * b.coordinate.second) / falpha;
				float beta = ((c.coordinate.second - a.coordinate.second) * j + (a.coordinate.first - c.coordinate.first) * i + c.coordinate.first * a.coordinate.second - a.coordinate.first * c.coordinate.second) / fbeta;
				float gamma = ((a.coordinate.second - b.coordinate.second) * j + (b.coordinate.first - a.coordinate.first) * i + a.coordinate.first * b.coordinate.second - b.coordinate.first * a.coordinate.second) / fgamma;
				if ((alpha >= 0) && (beta >= 0) && (gamma >= 0)) {
					if (alpha == 0 || beta == 0 || gamma == 0) {
						Pixel interpolated;
						interpolated.r = alpha * a.color.r + beta * b.color.r + gamma * c.color.r;
						interpolated.g = alpha * a.color.g + beta * b.color.g + gamma * c.color.g;
						interpolated.b = alpha * a.color.b + beta * b.color.b + gamma * c.color.b;
						_canvas.set(i, j, interpolated);
					}
				}
			}
		}
	}
	else {
		for (int i = ymin; i <= ymax; i++) {
			for (int j = xmin; j <= xmax; j++) {
				float alpha = ((b.coordinate.second - c.coordinate.second) * j + (c.coordinate.first - b.coordinate.first) * i + b.coordinate.first * c.coordinate.second - c.coordinate.first * b.coordinate.second) / falpha;
				float beta = ((c.coordinate.second - a.coordinate.second) * j + (a.coordinate.first - c.coordinate.first) * i + c.coordinate.first * a.coordinate.second - a.coordinate.first * c.coordinate.second) / fbeta;
				float gamma = ((a.coordinate.second - b.coordinate.second) * j + (b.coordinate.first - a.coordinate.first) * i + a.coordinate.first * b.coordinate.second - b.coordinate.first * a.coordinate.second) / fgamma;
				if ((alpha >= 0) && (beta >= 0) && (gamma >= 0)) {
					if (((alpha > 0) || (falpha * ((b.coordinate.second - c.coordinate.second) * -2.3 + (c.coordinate.first - b.coordinate.first) * -1.1 + b.coordinate.first * c.coordinate.second - c.coordinate.first * b.coordinate.second) > 0))
						&& ((beta > 0) || (fbeta * ((c.coordinate.second - a.coordinate.second) * -2.3 + (a.coordinate.first - c.coordinate.first) * -1.1 + c.coordinate.first * a.coordinate.second - a.coordinate.first * c.coordinate.second) > 0))
						&& ((gamma > 0) || (fgamma * ((a.coordinate.second - b.coordinate.second) * -2.3 + (b.coordinate.first - a.coordinate.first) * -1.1 + a.coordinate.first * b.coordinate.second - b.coordinate.first * a.coordinate.second) > 0))) {
						Pixel interpolated;
						interpolated.r = alpha * a.color.r + beta * b.color.r + gamma * c.color.r;
						interpolated.g = alpha * a.color.g + beta * b.color.g + gamma * c.color.g;
						interpolated.b = alpha * a.color.b + beta * b.color.b + gamma * c.color.b;
						_canvas.set(i, j, interpolated);
					}
				}
			}
		}
	}
}

void Canvas::draw_rectangle(int w, int h, int cx, int cy) 
{
	vert a;
	std::pair<int, int> aCords;
	aCords.first = cx - (w / 2);
	aCords.second = cy + (h / 2);
	a.coordinate = aCords;
	a.color = currColor;

	vert b; 
	std::pair<int, int> bCords;
	bCords.first = cx + (w / 2);
	bCords.second = cy + (h / 2);
	b.coordinate = bCords;
	b.color = currColor;	
	
	vert c;
	std::pair<int, int> cCords;
	cCords.first = cx - (w / 2);
	cCords.second = cy - (h / 2);
	c.coordinate = cCords;
	c.color = currColor;	
	
	vert d;
	std::pair<int, int> dCords;
	dCords.first = cx + (w / 2);
	dCords.second = cy - (h / 2);
	d.coordinate = dCords;
	d.color = currColor;

	draw_line(a, b);
	draw_line(a, c);
	draw_line(b, d);
	draw_line(c, d);
}

void Canvas::circleHelper(int xc, int yc, int x, int y)
{
	// draw 8 pixels per iteration, one in each octant of the circle
	_canvas.set(xc + x, yc + y, currColor);
	_canvas.set(xc - x, yc + y, currColor);
	_canvas.set(xc + x, yc - y, currColor);
	_canvas.set(xc - x, yc - y, currColor);
	_canvas.set(xc + y, yc + x, currColor);
	_canvas.set(xc - y, yc + x, currColor);
	_canvas.set(xc + y, yc - x, currColor);
	_canvas.set(xc - y, yc - x, currColor);
}

// using Bresenham's circle drawing algorithm
void Canvas::draw_circle(int r, int cx, int cy)
{
	int x = 0;
	int y = r;
	int d = 3 - 2 * r;
	circleHelper(cx, cy, x, y);
	while (y >= x)
	{
		// for each pixel we will
		// draw all eight pixels

		x++;

		// check for decision parameter
		// and correspondingly
		// update d, x, y
		if (d > 0)
		{
			y--;
			d = d + 4 * (x - y) + 10;
		}
		else
			d = d + 4 * x + 6;
		circleHelper(cx, cy, x, y);
	}
}

void Canvas::vertex(int x, int y)
{
	vert v;
	v.coordinate.first = x;
	v.coordinate.second = y;
	v.color = currColor;
	vertices.push_back(v);
	if (currType == POINTS) {
		_canvas.set(x, y, currColor);
	}
	else if ((this->currType == LINES) && (vertices.size() == 2)) {
		draw_line(vertices[vertices.size() - 2], v);
	}
	else if ((currType == TRIANGLES) && (vertices.size() % 3 == 0)) {
		draw_triangle(vertices[vertices.size() - 3], vertices[vertices.size() - 2], vertices[vertices.size() - 1]);
	} 
}

void Canvas::color(unsigned char r, unsigned char g, unsigned char b)
{
	Pixel c;
	c.r = r;
	c.g = g;
	c.b = b;
	currColor = c;
}

void Canvas::background(unsigned char r, unsigned char g, unsigned char b)
{
	for (int i = 0; i < _canvas.width() * _canvas.height() * _canvas.channels_num(); ++i) {
		_canvas.data()[i] = r;
		_canvas.data()[i + 1] = g;
		_canvas.data()[i + 2] = b;
	}
}

