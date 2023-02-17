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

void Canvas::begin(PrimitiveType type)
{
	if (this->currType != UNDEFINED) {
		std::cout << "Must end previous shape to begin a new one." << std::endl;
		return;
	}
	this->currType = type;
}

void Canvas::end()
{
	currType = UNDEFINED;
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
	for (int i = a.coordinate.first; i < b.coordinate.first; ++i) {
		Pixel c;
		c.r = (a.coordinate.first / i + a.coordinate.second / y) / 2 * a.color.r + (b.coordinate.first / i + b.coordinate.second / y) / 2 * b.color.r;
		c.r = (a.coordinate.first / i + a.coordinate.second / y) / 2 * a.color.g + (b.coordinate.first / i + b.coordinate.second / y) / 2 * b.color.g;
		c.r = (a.coordinate.first / i + a.coordinate.second / y) / 2 * a.color.b + (b.coordinate.first / i + b.coordinate.second / y) / 2 * b.color.b;
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

void Canvas::draw_high(vert a, vert b) {
	int x = a.coordinate.first;
	int W = b.coordinate.first - a.coordinate.first;
	int H = b.coordinate.second - a.coordinate.second;
	int dx = 1;
	if (W < 0) {
		dx = -1;
		W = -W;
	}
	int F = 2 * W - H;
	for (int i = a.coordinate.second; i < b.coordinate.second; ++i) {
		Pixel c;
		c.r = (i / a.coordinate.second + x / a.coordinate.first) / 2 * a.color.r + (i / b.coordinate.second + x / b.coordinate.first) / 2 * b.color.r;
		c.r = (i / a.coordinate.second + x / a.coordinate.first) / 2 * a.color.g + (i / b.coordinate.second + x / b.coordinate.first) / 2 * b.color.g;
		c.r = (i / a.coordinate.second + x / a.coordinate.first) / 2 * a.color.b + (i / b.coordinate.second + x / b.coordinate.first) / 2 * b.color.b;
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

void Canvas::vertex(int x, int y)
{
	if (this->currType == LINES) {
		vert coloredV;
		coloredV.coordinate.first = x;
		coloredV.coordinate.second = y;
		coloredV.color = currColor;
		vertices.push_back(coloredV);
		if (vertices.size() == 2) {
			vert a;
			vert b;
			a = vertices[0];
			b = vertices[1];
			int W = b.coordinate.first - a.coordinate.first;
			int H = b.coordinate.second - a.coordinate.second;
			if (abs(H) < abs(W)) {
				if (a.coordinate.first > b.coordinate.first) {
					a = vertices[1];
					b = vertices[0];
				}
				draw_low(a, b);
			}
			else {
				if (a.coordinate.second > b.coordinate.second) {
					a = vertices[1];
					b = vertices[0];
				}
				draw_high(a, b);
			}
		}
	}
	else if (currType == TRIANGLES) {
		vert v;
		v.coordinate.first = x;
		v.coordinate.second = y;
		v.color = currColor;
		vertices.push_back(v);
		if (vertices.size() % 3 == 0) {
			// sort vertices counterclockwise
			std::pair<int, int> centroid;
			centroid.first = (x + vertices[vertices.size() - 3].coordinate.first + vertices[vertices.size() - 2].coordinate.first) / 3;
			centroid.second = (y + vertices[vertices.size() - 3].coordinate.second + vertices[vertices.size() - 2].coordinate.second) / 3;
			float a1 = atan2(x - centroid.first, y - centroid.second)*(180 / M_PI) + 360 % 360;
			float a2 = atan2(vertices[vertices.size() - 3].coordinate.first - centroid.first, vertices[vertices.size() - 3].coordinate.second - centroid.second) * (180 / M_PI) + 360 % 360;
			float a3 = atan2(vertices[vertices.size() - 2].coordinate.first - centroid.first, vertices[vertices.size() - 2].coordinate.second - centroid.second) * (180 / M_PI) + 360 % 360;
			vert a;
			vert b;
			vert c;
			if (a1 < a2 && a1 < a3) {
				a.coordinate.first = x;
				a.coordinate.second = y;
				a.color = currColor;
				if (a2 < a3) {
					b.coordinate.first = vertices[vertices.size() - 2].coordinate.first;
					b.coordinate.second = vertices[vertices.size() - 2].coordinate.second;
					b.color = vertices[vertices.size() - 2].color;
					c.coordinate.first = vertices[vertices.size() - 3].coordinate.first;
					c.coordinate.second = vertices[vertices.size() - 3].coordinate.second;
					c.color = vertices[vertices.size() - 3].color;
				}
				else {
					c.coordinate.first = vertices[vertices.size() - 2].coordinate.first;
					c.coordinate.second = vertices[vertices.size() - 2].coordinate.second;
					c.color = vertices[vertices.size() - 2].color;
					b.coordinate.first = vertices[vertices.size() - 3].coordinate.first;
					b.coordinate.second = vertices[vertices.size() - 3].coordinate.second;
					b.color = vertices[vertices.size() - 3].color;
				}
			} else if (a2 < a1 && a2 < a3) {
				a.coordinate.first = vertices[vertices.size() - 3].coordinate.first;
				a.coordinate.second = vertices[vertices.size() - 3].coordinate.second;
				a.color = vertices[vertices.size() - 3].color;
				if (a1 < a3) {
					c.coordinate.first = x;
					c.coordinate.second = y;
					c.color = currColor;
					b.coordinate.first = vertices[vertices.size() - 3].coordinate.first;
					b.coordinate.second = vertices[vertices.size() - 3].coordinate.second;
					b.color = vertices[vertices.size() - 3].color;
				}
				else {
					b.coordinate.first = x;
					b.coordinate.second = y;
					b.color = currColor;
					c.coordinate.first = vertices[vertices.size() - 3].coordinate.first;
					c.coordinate.second = vertices[vertices.size() - 3].coordinate.second;
					c.color = vertices[vertices.size() - 3].color;
				}
			} else {
				a.coordinate.first = vertices[vertices.size() - 2].coordinate.first;
				a.coordinate.second = vertices[vertices.size() - 2].coordinate.first;
				a.color = vertices[vertices.size() - 2].color;
				if (a1 < a2) {
					b.coordinate.first = x;
					b.coordinate.second = y;
					b.color = currColor;
					c.coordinate.first = vertices[vertices.size() - 3].coordinate.first;
					c.coordinate.second = vertices[vertices.size() - 3].coordinate.second;
					c.color = vertices[vertices.size() - 3].color;
				}
				else {
					c.coordinate.first = x;
					c.coordinate.second = y;
					c.color = currColor;
					b.coordinate.first = vertices[vertices.size() - 3].coordinate.first;
					b.coordinate.second = vertices[vertices.size() - 3].coordinate.second;
					b.color = vertices[vertices.size() - 3].color;
				}
			}
			// draw triangle with no shared sides
			int xmax = max(max(a.coordinate.first, b.coordinate.first), c.coordinate.first);
			int xmin = min(min(a.coordinate.first, b.coordinate.first), c.coordinate.first);
			int ymax = max(max(a.coordinate.second, b.coordinate.second), c.coordinate.second);
			int ymin = min(min(a.coordinate.second, b.coordinate.second), c.coordinate.second);
			for (int i = ymin; i <= ymax; i++) {
				for (int j = xmin; j <= xmax; j++) {
					float alpha = (c.coordinate.first - b.coordinate.first) * (i - b.coordinate.second) + (c.coordinate.second - b.coordinate.second) * (j - b.coordinate.first);
					float beta = (a.coordinate.first - c.coordinate.first) * (i - c.coordinate.second) + (a.coordinate.second - c.coordinate.second) * (j - c.coordinate.first);
					float gamma = (b.coordinate.first - a.coordinate.first) * (i - a.coordinate.second) + (b.coordinate.second - a.coordinate.second) * (j - a.coordinate.first);
					if (alpha > 0 && beta > 0 && gamma > 0) {
						Pixel interpolated;
						interpolated.r = (alpha * a.color.r + beta * b.color.r + gamma * c.color.r) / 3;
						interpolated.g = (alpha * a.color.g + beta * b.color.g + gamma * c.color.g) / 3;
						interpolated.b = (alpha * a.color.b + beta * b.color.b + gamma * c.color.b) / 3;
						_canvas.set(i, j, interpolated);
					}
				}
			}
		}
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

