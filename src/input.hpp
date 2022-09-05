#pragma once
#include <GLFW/glfw3.h>

class Input {
private:
	GLFWwindow* window;
	double x, y, lastX = 0.0, lastY = 0.0, deltaX = 0.0, deltaY = 0.0;
public:
	Input(GLFWwindow* window, double x = 0.0, double y = 0.0);
	~Input();
	void setPosition(double x, double y);
	double getX();
	double getY();
	void setLastPosition(double lastX, double lastY);
	double getLastX();
	double getLastY();
	void update();
	double getDeltaX();
	double getDeltaY();
};
