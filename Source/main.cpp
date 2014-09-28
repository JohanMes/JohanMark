#include "JohanEngine\Renderer.h"
#include "JohanEngine\Dialogs.h"
#include "JohanEngine\Skybox.h"
#include "JohanEngine\TimeEvent.h"
#include "resource.h"

void OnKeyDown(WPARAM wParam,LPARAM lParam,bool handled) {
	if(handled) {
		return;
	}
	
	switch(wParam) {
		case 'W': {
			camera->Move(5*camera->GetDir());
			break;
		}
		case 'S': {
			camera->Move(-5*camera->GetDir());
			break;
		}
	}
}
void OnUpdateTime(void* sender,double data) {
	
	// Use 2D sine wave along these coordinates
	float fs = 0.002; // spatial frequency
	float ft = 3; // time frequency
	float A = 30; // amplitude
	
	double t = renderer->GetTime()/renderer->GetTimeMulti();

	for(int i = 0;i < (int)teapots.size();i++) {
		for(int j = 0;j < (int)teapots[i].size();j++) {
			Object* teapot = teapots[i][j];

			float3 translation;
			translation.x = teapot->GetTranslation().x;
			translation.z = teapot->GetTranslation().z;
			translation.y = A + A * sin(2 * pi * fs * translation.x + ft * t) * sin(2 * pi * fs * translation.z + ft * t);
			
			// And rotate along own axis in the process like a ship
			float dx = A * cos(2 * pi * fs * translation.x + ft * t);
			float dz = 0;//A * cos(2 * pi * fs * x + ft * t);// * cos(2 * pi * fs * z + ft * t);
			float4x4 rotation;
			rotation.EulerRotationDeg(float3(dx,50 * t,dz));
	
			// Use small steps...
			teapot->BeginUpdate();
			teapot->SetTranslation(translation);
			teapot->SetRotation(rotation);
			teapot->EndUpdate();
		}
	}
}
void ToggleFullscreen(void* data) {
	renderer->ToggleFullScreen();
}
void SaveBuffers(void* data) {
	renderer->ScheduleSaveBuffers();
}
void ToggleConsole(void* data) {
	console->Toggle();
}
void ToggleOptions(void* data) {
	options->Toggle();
}

void LoadScene() {
	
	int size = 40;
	int dx = 50;
		
	camera->BeginUpdate();
	camera->SetPos(float3(20*(size-3),150,20*(size-3)));
	camera->SetLookAt(float3(0,30,0));
	camera->SetFOV(DegToRad(60));
	camera->EndUpdate();
	
	// En er was licht
	skybox = new Skybox();
	skybox->SetSunlight("Scene\\Sun.obj","Scene\\Sun.mtl"); // casts shadows too
	skybox->SetSkybox("Scene\\Skybox.obj","Scene\\Skybox.mtl");
	
	Object* ground = scene->objects->AddPlane("Ground","teapot\\WoodBoards.mtl",0,0,4000,16,15,NULL);
	ground->castshadows = false;
	
	teapots.resize(size);
	for(int i = 0;i < (int)teapots.size();i++) {
		teapots[i].resize(size);
	}
	
	for(int i = 0;i < (int)teapots.size();i++) {
		for(int j = 0;j < (int)teapots[i].size();j++) {
			teapots[i][j] = new Object(
				"Teapot",
				"teapot\\Teapot.obj",
				"teapot\\White.mtl",
				float3(dx*(i - size/2) - dx/2,0,dx*(j - size/2) - dx/2),
				0,
				2);
			teapots[i][j]->material->color = 2.0f * float4(
				0.0f + i/(float)size,
				0.0f + j/(float)size,
				0,
				1);
		}
		
		char buffer[128];
		snprintf(buffer,128,"Loading... (%.1f%%)",100 * (i + 1)/(float)teapots.size());
		renderer->Begin(true);
		renderer->DrawTextLine(buffer,5,5);
		renderer->End();
	}
	
	ui->AddKey(new Key(VK_F2,ToggleFullscreen));
	ui->AddKey(new Key(VK_F3,SaveBuffers));
	ui->AddKey(new Key(VK_F11,ToggleConsole));
	ui->AddKey(new Key(VK_F12,ToggleOptions));
	
	ui->OnKeyDown = OnKeyDown;
}
void InitBench() {
	renderer->SetTime(9,0);
	renderer->ShowTooltip(3);
	renderer->SetTimeMulti(1500);
	scene->OnUpdateTime->Add(OnUpdateTime,NULL);
	LoadScene();
	benchmark = new Benchmark();
	benchmark->Start(8.5 * 3600);
}

void DeleteBench() {
	delete skybox;
	delete benchmark;
}

// This is where Windows sends user input messages
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	
	// send message to 3D interface
	ui->OnMessage(hwnd,Message,wParam,lParam);
	
	// Perform more handling
	switch(Message) {
		case WM_DESTROY: {
			PostQuitMessage(0); // we are asked to close: kill main thread
			break;
		}
		default: {
			return DefWindowProc(hwnd, Message, wParam, lParam);
		}
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEXA wc = {0};
	MSG Msg = {0};

	// Create a window with these properties
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.lpfnWndProc   = WndProc;
	wc.hInstance     = hInstance;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wc.lpszClassName = "WindowClass";
	wc.hIcon         = LoadIcon(hInstance,"A"); // laad projecticoon
	wc.hIconSm       = LoadIcon(hInstance,"A");
	
	// Say hi to Windows
	if(!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}
	
	// Set up a window with 1024x768 usable pixels
	RECT result = {0,0,1024,768};
	AdjustWindowRect(&result,WS_VISIBLE|WS_OVERLAPPEDWINDOW,false);
	
	// Create a window with a border and 'client rect' of 1024x768
	hwnd = CreateWindow("WindowClass","JohanMark",WS_VISIBLE|WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, // x
		CW_USEDEFAULT, // y
		result.right - result.left, // width
		result.bottom - result.top, // height
		NULL,NULL,hInstance,NULL);
	if(hwnd == NULL) {
		MessageBox(NULL,"Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}
	
	// Init render loop
	InitEngine();
	
	InitBench();
	
	// Handle user input. If done, render a frame. Goto 1
	while(Msg.message != WM_QUIT) {
		while(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
		
		if(!renderer->paused) {
			renderer->Begin(false);
			renderer->DrawScene(scene);
			renderer->End();
		} else {
			Sleep(100);
		}
	}
	
	DeleteBench();
	
	// Render loop stopped due to Alt+F4 etc? Delete everything
	DeleteEngine();

	return Msg.wParam;
}
