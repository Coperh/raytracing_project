#pragma once
namespace Tmpl8
{

class MyApp : public TheApp
{
public:

	// game flow methods
	void Init();
	void Tick( float deltaTime );
	void Shutdown() { /* implement if you want to do something on exit */ }
	// input handling
	void MouseUp( int button ) { /* implement if you want to detect mouse button presses */ }
	void MouseDown( int button ) { /* implement if you want to detect mouse button presses */ }
	void MouseMove( int x, int y ) { mousePos.x = x, mousePos.y = y; }
	void KeyUp(int key);
	void KeyDown(int key);
	// data members
	int2 mousePos;

	bool turning_left;
	bool turning_right;
};


struct Ray {
	float3 O;
	float3 D;
	float t;
};



} // namespace Tmpl8