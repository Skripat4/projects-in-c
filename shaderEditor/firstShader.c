#include <windows.h>
#include "glad.h"
#include <stdio.h>

#define nScreenWidth 1024
#define nScreenHeight 768

GLuint prog;

int time = 0;
int mousex = 0;
int mousey = 0;

char *LoadFile(char *nameFile)
{
	static int len = 8000;
	char *text = malloc(len);
	memset(text, 0, len);
	FILE *f;
	f = fopen(nameFile, "rb");
	if (f)
	{
		fread(text, 1, len, f);
		fclose(f);
	}
	return text;
}

GLuint LoadShader(char *FileName, GLuint type)
{
	char *txt = LoadFile(FileName);
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &txt, NULL);
	glCompileShader(shader);

	GLuint ok; 
	GLchar log[2000];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
	if (!ok)
	{
		glGetShaderInfoLog(shader, 2000, NULL, log);
		FILE *f = fopen("error.txt", "wt");
		fwrite(log, 1, 2000, f);
		fclose(f);
		//printf("%s\n",log);
	}

	free(txt);
	return shader;
}

void Init()
{
	gladLoadGL();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, nScreenWidth, nScreenHeight, 0.0f, 0.0f, 1.0f);

	GLuint shader = LoadShader("test.frag", GL_FRAGMENT_SHADER);

	prog = glCreateProgram();
	glAttachShader(prog, shader);
	glLinkProgram(prog);

}

void Show()
{
	
	glUseProgram(prog);
	
	int t = glGetUniformLocation(prog, "time");
	glUniform1i(t, time++);

	t = glGetUniformLocation(prog, "mousex");
	glUniform1i(t, mousex);

	t = glGetUniformLocation(prog, "mousey");
	glUniform1i(t, mousey);

	glColor3f(1, 0, 0);
	glBegin(GL_QUADS);
	 glVertex2f(0, 0);
	 glVertex2f(nScreenWidth, 0);
	 glVertex2f(nScreenWidth, nScreenHeight);
	 glVertex2f(0, nScreenHeight);
	glEnd();
	glUseProgram(0);
}
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    float theta = 0.0f;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "firstShader",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          nScreenWidth,
                          nScreenHeight,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

	Init(); 

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */

            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

			Show();

            SwapBuffers(hDC);

            Sleep (1);

        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;

}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;
		case WM_MOUSEMOVE:
			mousex = LOWORD(lParam);
			mousey = HIWORD(lParam);
			break;
        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                break;
            }
        }
        break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}