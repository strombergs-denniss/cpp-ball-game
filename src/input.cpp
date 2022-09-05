#include "input.hpp"

Input::Input(GLFWwindow* window, double x, double y) {
	this->window = window;
	this->x = x;
	this->y = y;
}

Input::~Input() {}

void Input::setPosition(double x, double y) {
	this->x = x;
	this->y = y;
}

double Input::getX() {
	return x;
}
double Input::getY() {
	return y;
}

void Input::setLastPosition(double lastX, double lastY) {
	this->lastX = lastX;
	this->lastY = lastY;
}

double Input::getLastX() {
	return lastX;
}

double Input::getLastY() {
	return lastY;
}

void Input::update() {
	glfwGetCursorPos(window, &x, &y);
	deltaX = x - lastX;
	deltaY = y - lastY;
}

double Input::getDeltaX() {
	return deltaX;
}

double Input::getDeltaY() {
	return deltaY;
}