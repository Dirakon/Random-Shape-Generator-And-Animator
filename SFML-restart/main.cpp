#include <SFML/Graphics.hpp>
#include "wtypes.h"
#include <iostream>
#include <cmath>
#define ONLY_CONVEXs
#ifdef ONLY_CONVEX
#define Shape ConvexShape
#endif // ONLY_CONVEX

using namespace std;


using namespace sf;


const float minDist = 0.1f, maxDist = 3;
const float minRaz = 0, maxRaz = 100;
const int granMin = 2, granMax = 10;
const int pointMin = 3, pointMax = 10;
const float minScale = 10, maxScale = 50;
long horizontal;
long vertical;
int InBetweenTwoInts(int min, int max, long seed) {
	return seed % (max - min + 1) + min;
}
float InBetweenTwoFloats(float min, float max, long seed) {
	return min + (max - min) * (seed) / RAND_MAX;
}
void SetPosition(Shape * s, float minX, float maxX, float minY, float maxY, long seed1, long seed2) {
	float pos[] = { InBetweenTwoFloats(minX, maxX, seed1), InBetweenTwoFloats(minY, maxY, seed2) };
	for (int i = 0; i < 2; ++i) {
		if (pos[i] < 0) {
			pos[i] = 0;
		}
	}
	s->setPosition(pos[0] > (float)horizontal ? horizontal : pos[0], pos[1] > vertical ? vertical : pos[1]);
}
float ChangeMagnitude(float minMagn, float maxMagn, long seed1) {
	float m = InBetweenTwoFloats(minMagn, maxMagn, seed1);
	if (m < minDist) {
		m = minDist;
	}
	else if (m > maxDist) {
		m = maxDist;
	}
	return m;
}
void SetRotation(Shape * s, float minA, float maxA, long seed) {
	s->setRotation(InBetweenTwoFloats(minA, maxA, seed));
}
void SetScale(Shape * s, float minX, float maxX, float minY, float maxY, long seed1, long seed2) {
	float scale[] = { InBetweenTwoFloats(minX, maxX, seed1), InBetweenTwoFloats(minY, maxY, seed2) };
	for (int i = 0; i < 2; ++i) {
		if (scale[i] < minScale) {
			scale[i] = minScale;
		}
		else if (scale[i] > maxScale) {
			scale[i] = maxScale;
		}
	}
	s->setScale(scale[0], scale[1]);
}
int* colorData;
const int ColorAmount = 13;
Color colors[ColorAmount]{ Color(204,0,0),Color(204,102,0),Color(204,204,0),Color(102,204,0),Color(0,204,0),Color(0,204,102),Color(0,204,204),Color(0,102,204),Color(0,0,204),Color(102,0,204),Color(204,0,204),Color(204,0,102),Color(96,96,96) };
void SetColor(Shape * s, int minR, int maxR, int minG, int maxG, int minB, int maxB, long seed1, long seed2, long seed3) {
	int rgb[] = { InBetweenTwoInts(minR, maxR, seed1),InBetweenTwoInts(minG, maxG, seed2), InBetweenTwoInts(minB, maxB, seed3) };
	for (int i = 0; i < 3; ++i) {
		if (rgb[i] < 0) {
			rgb[i] = 0;
		}
		else if (rgb[i] > 255) {
			rgb[i] = 255;
		}
	}

	s->setFillColor(Color(rgb[0], rgb[1], rgb[2]));
}
void SetColor(int id, Shape * s, int min, int max, int chance, long seed) {
	if (rand() % 100 >= chance)
		return;
	int i = InBetweenTwoInts(min, max, seed);

	if (i < 0) {
		i = 0;
	}
	else if (i >= ColorAmount) {
		i = ColorAmount - 1;
	}

	colorData[id] = i;
	s->setFillColor(colors[i]);
}
void SetupConvexShape(ConvexShape * cs, int pointAmount) {
	float meta = 0;
	float prop;
	float rad, distFromCenter;
	Vector2f cur;
	int j = 0;
	float* raz = new float[pointAmount];
	for (j = 0; j < pointAmount; ++j) {
		meta += InBetweenTwoFloats(minRaz, maxRaz, rand());
		raz[j] = meta;
	}
	for (j = 0; j < pointAmount; ++j) {
		prop = raz[j] / meta;
		rad = prop * 3.14159265358979323846 * 2;
		distFromCenter = InBetweenTwoFloats(minDist, maxDist, rand());
		cur = Vector2f(distFromCenter * cos(rad), distFromCenter * sin(rad));
		cs->setPoint(j, cur);
	}
	delete[] raz;
}
Shape** SpawnShapes(Shape * *shapes, long horizontal, long vertical, int shapeCount) {
	if (shapes != nullptr) {
		delete[] shapes;
	}
	shapes = new Shape * [shapeCount];
	for (int i = 0; i < shapeCount; ++i) {
#ifndef ONLY_CONVEX
		const int optAmount = 4;
#else
		const int optAmount = 3;
#endif
		int granAmount;
		switch (rand() % optAmount) {
		default://Grans
#ifndef ONLY_CONVEX


			granAmount = InBetweenTwoInts(granMin, granMax, rand());
			if (granAmount < 3) {
				shapes[i] = new CircleShape(1);
			}
			else {
				shapes[i] = new CircleShape(1, granAmount);
			}
			break;
#endif // !Only_Convex
		case 1:
		case 2:
		case 3:
			int pointAmount = InBetweenTwoInts(pointMin, pointMax, rand());
			ConvexShape* cs = new ConvexShape(pointAmount);
			SetupConvexShape(cs, pointAmount);
			shapes[i] = cs;
			break;
	}
		SetPosition(shapes[i], 0, horizontal, 0, vertical, rand(), rand());
		SetRotation(shapes[i], 0, 360, rand());
		SetScale(shapes[i], minScale, maxScale, minScale, maxScale, rand(), rand());
		SetColor(i, shapes[i], 0, ColorAmount - 1, 100, rand());
}
	return shapes;
}
const int minShapes = 3000, maxShapes = 5000;
const float maxMoveSdvig = 10;
const int colorSdvigChance = 25;
const float maxAngleSdvig = 10;
const float maxScaleSdvig = 5;
const float magnitudeSdvig = 0.1f;

void MakeTransformation(int id, Shape * s, long seed) {
#ifndef ONLY_CONVEX
	const int opAmount = 4;
#else
	const int opAmount = 5;
#endif
	Vector2f vec;
	Color col;
	float f;
	int i;
	Vector2f vec2;
	switch (seed % opAmount) {
	case 0://Move
		vec = s->getPosition();
		SetPosition(s, vec.x - maxMoveSdvig, vec.x + maxMoveSdvig, vec.y - maxMoveSdvig, vec.y + maxMoveSdvig, rand(), rand());
		break;
	case 1://ChangeColor
		col = s->getFillColor();
		SetColor(id, s, colorData[id] - 1, colorData[id] + 1, colorSdvigChance, rand());
		break;
	case 2://Rotate
		f = s->getRotation();
		SetRotation(s, f - maxAngleSdvig, f + maxAngleSdvig, rand());
		break;
	case 3://Scale
		vec = s->getScale();
		SetScale(s, vec.x - maxScaleSdvig, vec.x + maxScaleSdvig, vec.y - maxScaleSdvig, vec.y + maxScaleSdvig, rand(), rand());
		break;
#ifdef ONLY_CONVEX
	case 4://Point change
		i = rand() % s->getPointCount();
		vec = s->getPoint(i);
		f = sqrt(vec.x * vec.x + vec.y * vec.y);
		vec.x /= f;
		vec.y /= f;
		f = ChangeMagnitude(f - magnitudeSdvig, f + magnitudeSdvig, rand());
		vec.x *= f;
		vec.y *= f;
		s->setPoint(i, vec);
		break;
#endif
	}
}
int main()
{
	srand(time(0));
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	horizontal = desktop.right;
	vertical = desktop.bottom;
	sf::RenderWindow window(sf::VideoMode(horizontal, vertical), "RSTAM");
	int shapeCount = InBetweenTwoInts(minShapes, maxShapes, rand());
	Shape** shapes = nullptr;
	colorData = new int[shapeCount] {};
	shapes = SpawnShapes(shapes, horizontal, vertical, shapeCount);
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				delete[] shapes;
				delete[] colorData;
				window.close();
				return 0;
			}
			else if (event.type == sf::Event::KeyPressed) {
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
					shapes = SpawnShapes(shapes, horizontal, vertical, shapeCount);
				}
			}
		}
		window.clear();
		for (int i = 0; i < shapeCount; ++i) {
			MakeTransformation(i, shapes[i], rand());
			window.draw(*shapes[i], RenderStates::Default);
		}
		window.display();
	}

	return 0;
}