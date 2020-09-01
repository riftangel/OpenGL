#include "ogInputManager.h"

ogInputManager::ogInputManager() {
}

ogInputManager::~ogInputManager() {

}

void ogInputManager::mRegisterWindowCallback(GLFWwindow* aWindow) {

	glfwSetWindowUserPointer(aWindow, this);
	glfwSetKeyCallback(aWindow, (GLFWkeyfun)mKeyCallbackStatic);
	glfwSetCursorPosCallback(aWindow, (GLFWcursorposfun)mMouseCallbackStatic);
	glfwSetScrollCallback(aWindow, (GLFWscrollfun)mScrollCallbackStatic);

	this->m_xpos = -1;
	this->m_ypos = -1;

	this->m_xscroll = -1;
	this->m_yscroll = -1;

	this->m_scroll_min = 0;
	this->m_scroll_max = 1.0;
	this->m_scroll_inc = 0.05;
}

void ogInputManager::mDeRegisterWindowCallback(GLFWwindow* aWindow)
{
}


void ogInputManager::mKeyCallback(GLFWwindow* aWindow, int key, int scancode, int action, int mods) {

	// actin in [ GLFW_PRESS, GLFW_REPEAT or GLFW_RELEASE, GLFW_KEY_UNKNOWN ]
#if 0
	std::cout << std::dec << "key: " << key << " / " << (char)key << std::showbase << std::hex << " scancode: " << scancode << " action: " << action << " mods " << mods << std::endl;
#endif
	
	struct cb_data_info cb_data_new;
	
	// Store keymap info if keypressed or repeat
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		cb_data_new.scancode = scancode;
		cb_data_new.action = action;
		cb_data_new.mods = mods;
		this->m_keymap[key] = cb_data_new;
#if 0
		const char* key_name = glfwGetKeyName(key, scancode);
		if (key_name)
			std::cout << "keyname : "<< key_name << std::endl;
#endif
	}

	if (action == GLFW_RELEASE)
		this->m_keymap.erase(key);
}

std::map<int, cb_data_info>& ogInputManager::mGetKeyMap() {
	return this->m_keymap;
}

void ogInputManager::mGetMousePos(double& xpos, double& ypos) {
	xpos = this->m_xpos;
	ypos = this->m_ypos;
}

void ogInputManager::mClearKeyMap(int aKey) {
	this->m_keymap.erase(aKey);
}

void ogInputManager::mGetScrollPos(double& xpos, double& ypos) {

	xpos = this->m_xscroll;
	ypos = this->m_yscroll;
}

void ogInputManager::mSetScrollRange(float aMinRange, float aMaxRange, float aIncrement)
{
	this->m_scroll_min = aMinRange;
	this->m_scroll_max = aMaxRange;
	this->m_scroll_inc = aIncrement;
}

void ogInputManager::mMouseCallback(GLFWwindow* aWindow, double xpos, double ypos) {

	this->m_xpos = xpos;
	this->m_ypos = ypos;
}

void ogInputManager::mKeyCallbackStatic(GLFWwindow* aWindow, int key, int scancode, int action, int mods)
{
	// Just being overkill here .. and using the instance callback instead of a static one
	ogInputManager* instance = static_cast<ogInputManager*>(glfwGetWindowUserPointer(aWindow));
	instance->mKeyCallback(aWindow, key, scancode, action, mods);
}

void ogInputManager::mMouseCallbackStatic(GLFWwindow* aWindow, double xpos, double ypos) {

	ogInputManager* instance = static_cast<ogInputManager*>(glfwGetWindowUserPointer(aWindow));
	instance->mMouseCallback(aWindow, xpos, ypos);
}

void ogInputManager::mScrollCallback(GLFWwindow* aWindow, double xpos, double ypos)
{
	this->m_xscroll = xpos;
	this->m_yscroll = ypos;

	if (ypos>0) {
		this->m_scroll_factor += this->m_scroll_inc;
		if (this->m_scroll_factor >= this->m_scroll_max)
			this->m_scroll_factor = this->m_scroll_max;
	}
	else {
		this->m_scroll_factor -= this->m_scroll_inc;
		if (this->m_scroll_factor <= this->m_scroll_min)
			this->m_scroll_factor = this->m_scroll_min;
	}
}

void ogInputManager::mScrollCallbackStatic(GLFWwindow* aWindow, double xpos, double ypos) {

	ogInputManager* instance = static_cast<ogInputManager*>(glfwGetWindowUserPointer(aWindow));
	instance->mScrollCallback(aWindow, xpos, ypos);
}

float ogInputManager::mGetScrollFactor() {
	return this->m_scroll_factor;
}
