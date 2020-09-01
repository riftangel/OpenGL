#pragma once

#include "libs.h"
#include <map>

struct cb_data_info {
	int scancode;
	int action;
	int mods;
};

class ogInputManager {

public:
	ogInputManager();
	~ogInputManager();
	void mRegisterWindowCallback(GLFWwindow* aWinddow);
	void mDeRegisterWindowCallback(GLFWwindow* aWindow);	
	void mGetMousePos(double& xpos, double& ypos);
	void mGetScrollPos(double& xpos, double& ypos);
	void mSetScrollRange(float aMinRange, float aMaxRange, float aIncrement);
	float mGetScrollFactor();
	void mClearKeyMap(int aKey);
	std::map<int, cb_data_info>& mGetKeyMap();

private:
	void mKeyCallback(GLFWwindow* aWindow, int key, int scancode, int action, int mods);
	static void mKeyCallbackStatic(GLFWwindow* aWindow, int key, int scancode, int action, int mods);

	void mMouseCallback(GLFWwindow* aWindow, double xpos, double ypos);
	static void mMouseCallbackStatic(GLFWwindow* aWindow, double xpos, double ypos);

	void mScrollCallback(GLFWwindow* aWindow, double xpos, double ypos);
	static void mScrollCallbackStatic(GLFWwindow* aWindow, double xpos, double ypos);

	std::map<int, cb_data_info> m_keymap;
	double m_xpos;
	double m_ypos;
	double m_xscroll;
	double m_yscroll;
	float  m_scroll_min;
	float  m_scroll_max;
	float  m_scroll_inc;
	float  m_scroll_factor;
};