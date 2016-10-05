#pragma once
class OpenGLApp
{
private:
	int m_windowWidth;
	int m_windowHeight;
	float m_angle;


public:
	OpenGLApp();
	~OpenGLApp();

	bool Initialize(); // initialize your opengl scene
	bool Shutdown();  // shut down your scene

	// used to setup your opengl camera and drawing area
	void SetupProjection(int width, int height);

	void Prepare(float dt);
	void Render();

	void ShowGrid();

};

