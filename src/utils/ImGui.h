#pragma once

class MyImGui {
public:
	MyImGui(const MyImGui&) = delete;
	MyImGui& operator=(const MyImGui&) = delete;

	static MyImGui& GetInstance();

private:
	MyImGui();
};