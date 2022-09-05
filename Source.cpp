#include <windows.h>
#include <fstream>
//��� ����������� ���� ���������� �������� ��-�� ���������� ������� �����������


//�������� ����� windows 10 ��� ��������� ����������
#pragma comment(linker,"\"/manifestdependency:type='win32' \ name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \ processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

//���������� �����
#define gray_color RGB(135, 146, 112)
#define black_color RGB(0,0,0)
#define font_color RGB(154, 166, 128)

//����������� ������� ��������� ���������
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//����� ����� ��� win32 ����������
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static char szAppName[] = "Tetris emulator"; //��������� ����


	HWND hwnd; //���������� ����
	MSG msg; //��������� ��� ��������� windows
	WNDCLASSEX wndclass; //����� ����

	//�������� ������ �������� ����
	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = CreateSolidBrush(font_color);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&wndclass); //������������ ��� � ��

	//�������� �������� ����, ����������� ��� � �������, � hwnd ����������� ��������� ��� ����������� ��������� � ����
	hwnd = CreateWindow(
		szAppName, 
		"Tetris emulator V - 0.5", // ���������
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |  WS_MINIMIZEBOX, // �����, ���������� ������ ��������� �� ���� ����� � ����������� �������� ������ ����
		CW_USEDEFAULT, // ������� �� x �� ���������
		CW_USEDEFAULT, // ������� �� ���������
		580, // ������ ����
		475, // ������ ����
		NULL, // ��������� �� ������������ ����, � ��� ��� �.�. ��� �������
		NULL, // ��������� �� ������ ����, � ��� ��� ��� ��� ������� ���������
		hInstance, //��������� ������ ���������
		NULL
	); 

	ShowWindow(hwnd, iCmdShow); //���������� ���� 
	UpdateWindow(hwnd); //�������������� ����

	while (GetMessage(&msg, NULL, 0, 0)) //��������� ���� ��������� ��������� �� windows
	{
		TranslateMessage(&msg); //�������� ��������� � WndProc
		DispatchMessage(&msg); //�������� ��������� � windows
	}
	return msg.wParam;
}


// ���������� �������� ������
class ETetris {

public:

	ETetris() { //� ������������ ��������� ����� ������ ������

		for (int i = 0; i <= 19; i++) {
			for (int j = 0; j<= 19; j++) {
				screen_buffer[i][j] = 0;				
			}
		}
		

		TPen_gray = CreatePen(PS_SOLID, 1, gray_color); //������� ����� � ����� GDI (Graphical Device Interface)

		TBrush_gray = CreateSolidBrush(gray_color);

		TPen_black = CreatePen(PS_SOLID, 1, black_color);

		TBrush_black = CreateSolidBrush(black_color);

		TBrush_font = CreateSolidBrush(font_color);
	}

	~ETetris() { //� ����������� �������� GDI ������� �� ������
		DeleteObject(TPen_gray);
		DeleteObject(TBrush_gray);
		DeleteObject(TPen_black);
		DeleteObject(TBrush_black);
		DeleteObject(TBrush_font);
	}
	

	void CreateScreen(HDC Thdc) { // ������� ����������� ����, ��������� �������� �������� HDC ���� ������
		x_disp = 20;
		y_disp = 20;

		

		ETetris::DrawFrame(Thdc, 10, 10, 415, 415); // ������ �����


		for (int j = 0; j <= 19; j++) { // ������ ����
			x_disp = 20;
			for (int i = 0; i <= 19; i++) {
				if (screen_buffer[i][j] == 0) {
					DrawPixel(Thdc,Gray, x_disp, y_disp, 15, 15); // ������� ��������� ���������� ���������� �������
				}
				else
				{

					DrawPixel(Thdc, Black, x_disp, y_disp, 15, 15);
				}
				x_disp += 20;
			}
			y_disp += 20;
		}

}
	void SetPixelState(int x, int y) { // ������� ��� �������������� ������ ������
		
		if (screen_buffer[x-1][y-1] == 0) { // �������� ��������� �������
			screen_buffer[x-1][y-1] = 1;
		}
		else
		{
			screen_buffer[x-1][y-1] = 0;
		}
	}

	void Fill(int x, int y) { // ����������� ������� �������, �������� ���������� �����

		if (x >= 0 && x < 20 && y >= 0 && y < 20 && screen_buffer[x][y] == 0 && screen_buffer[x][y] != 1)
		{
			screen_buffer[x][y] = 1;

			Fill(x + 1, y);
			Fill(x - 1, y);
			Fill(x, y + 1);
			Fill(x, y - 1);
		}
	}

	void Erase() { // ������� ������� ����
		for (int i = 0; i <= 19; i++) {
			for (int j = 0; j <= 19; j++) {
				screen_buffer[i][j] = 0;
			}
		}
	}

	void SaveScreen() { // ������� ���������� ����������� ������ � ����

		std::ofstream ScreenFile("ScreenBuffer.txt");
		for (int i = 0; i <= 19; i++) {
			for (int j = 0; j <= 19; j++) {
				ScreenFile << screen_buffer[j][i];
			}
			if(i != 19)  ScreenFile << std::endl;
		}

		ScreenFile.close();
	}

	void OpenScreen(HWND hwnd) { // ������� ������ �� ����� ������ ������
		char buff[21];
		std::ifstream Screen("ScreenBuffer.txt");

		if (!Screen.is_open()) {
			MessageBox(hwnd, "���� �� ��������� ��� ���� ��� ���������!", "��", NULL);
		}
		else {
			int j = 0;
			while (!Screen.eof()) {
				Screen.getline(buff, 21);

				/*wsprintf(szbuff, "len - %d", strlen(buff));
				MessageBox(hwnd, buff, szbuff, NULL);*/

				for (int i = 0; i <= strlen(buff)-1; i++) {
					if (buff[i] == '0') {
						screen_buffer[i][j] = 0;
					}
					else {
						screen_buffer[i][j] = 1;
					}
				}
				j++;
			}
			
		}
		Screen.close();
	}

private:
	enum colors {Gray, Black};  // ��������� �������
	int screen_buffer[20][20]; // ����� ������
	int x_disp, y_disp;

	 HBRUSH TBrush_gray; // ����� � �����
	 HPEN TPen_gray;
     HBRUSH TBrush_black;
	 HPEN TPen_black;
	 HBRUSH TBrush_font;


	void DrawPixel(HDC hdc, colors color, int x, int y, int width, int height) { //������� ��������� �������
	   
		if (color == Gray) {

			SelectObject(hdc, TPen_gray); // ����� ���� � �������� ���������
		}
		else {

			SelectObject(hdc, TPen_black); // ����� ���� � �������� ���������
		}


		SelectObject(hdc, TBrush_font); // ����� ����� � �������� ���������, ��� �������
		 

		Rectangle(hdc, x, y, x + width, y + height); // ������ �������������

		if (color == Gray) { 
			
			SelectObject(hdc, TBrush_gray);
		}
		else {
			SelectObject(hdc, TBrush_black);
		}
		Rectangle(hdc, x + int(width*0.2), y + int(height*0.2), x + int(width*0.8), y + int(height*0.8));  // ������ ���������� �������������

	}

	void DrawFrame(HDC hdc, int x, int y, int width, int height) {	// ����� �����
		SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));
		Rectangle(hdc, x, y, x + width, y + height);
	}
};

ETetris Tetris; // ������� ��������� ������ 


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) // ������� ��� ��������� ��������� windows
{
	//��������� ����������
	int x, y; //��� ��������� ����
	int x_pos, y_pos; // ��� ��������� ������� � ������(�������) 
	bool finde_x, finde_y;
	HDC hdc; // �������� ���������� GDI
	PAINTSTRUCT ps; // ��� ���������� �� GDI
	static RECT rect; // ��� �������������� ����������� ����� ����

	//��������� �� ������
	static HWND draw_button;
	static HWND Fill_button;
	static HWND Save_button;
	static HWND Load_button;
	static HWND Erase_button;

	static int state;
	

	static char szBuffer[50];


	switch (iMsg) //������ ��������� �� windows
	{
	case WM_CREATE: //������� �������� ����
		state = 1;
		//������� ������
		draw_button = CreateWindow("button", "��������", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 445, 12, 100, 20, hwnd, (HMENU)1, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Fill_button = CreateWindow("button", "���������", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 445, 55, 100, 20, hwnd, (HMENU)2, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Save_button = CreateWindow("button", "���������", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 445, 370, 100, 20, hwnd, (HMENU)3, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Load_button = CreateWindow("button", "���������", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 445, 400, 100, 20, hwnd, (HMENU)4, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Erase_button = CreateWindow("button", "��������", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 445, 150, 100, 20, hwnd, (HMENU)5, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		rect.left = 20;
		rect.top = 20;
		rect.right = 430;
		rect.bottom = 430;
		return 0;

	case WM_COMMAND: //������� ����� �� �������, � ����� ��������� �� ��������� ���������� � wParam � lParam ����������� ����������� � ���� ���������
		
		switch (LOWORD(wParam)) { //���������� �� ����� ������ ���� ������� �� � id ������� ����������� � wParam, � �������� � ������� CreateWindow, � ����������� ���� � (HMENU)
		case 1: 
			rect.left = 445;
			rect.top = 110;
			rect.right = 550;
			rect.bottom = 130;
			state = 1;
			InvalidateRect(hwnd, &rect, TRUE); //�������� ����� ������� � �������������� ������ ������������� � ���� �������� 
		break;

		case 2:
			rect.left = 445;
			rect.top = 110;
			rect.right = 550;
			rect.bottom = 130;
			state = 2;	
			InvalidateRect(hwnd, &rect, TRUE); //�������� ����� ������� � �������������� ������ ������������� � ���� �������� 
		break;

		case 3:
			Tetris.SaveScreen(); //�������� ������� ����������� ������ ������
		break;

		case 4:
			Tetris.OpenScreen(hwnd); //�������� ������� ������ ������ ������ �� �����
			rect.left = 20;
			rect.top = 20;
			rect.right = 430;
			rect.bottom = 430;
			InvalidateRect(hwnd, &rect, TRUE); //�������������� ����
		break;
		case 5:
			Tetris.Erase(); //�������� ������� ������� ������ 
			rect.left = 20;
			rect.top = 20;
			rect.right = 430;
			rect.bottom = 430;
			InvalidateRect(hwnd, &rect, TRUE); //�������������� ����
		break;
		}
		
		return 0;

	case WM_LBUTTONDOWN: //������� ������ �� ����� ������ ����
		x = LOWORD(lParam); // ���������� ����������
		y = HIWORD(lParam);
		x_pos = 0;
		y_pos = 0;
		finde_x = false;
		finde_y = false;
		if (x > 20 && x < 35) { //���������� ��� �� ���� �� ������� �������, ���� �� ��������� finde_x � finde_y � ������ � ����������� ���������� �������
			x_pos = 1;
			finde_x= true;
		}
		if (y > 20 && y < 35) {
			y_pos = 1;
			finde_y = true;
		}
		//���� ��� �� ������ ������ ���� �� ���������, ������������� ���������� � 5px ��� �� ������������ � ���� ���� ��� ����� ��������� ��� �� ������
			for (int i = 1; i <= 19; i++) {
				if (!finde_x) {
					if (x > 20 + (5 * i) + (15 * i) && x < 20 + (5 * i) + (15 * i) + 15) {
						x_pos = i + 1;
						finde_x = true;
					}
				}
				if (!finde_y) {
					if (y > 20 + (5 * i) + (15 * i) && y < 20 + (5 * i) + (15 * i) + 15) {
						y_pos = i + 1;

					}
				}
			}
			
			if (state == 1) { //���� ������� ������� ���������, ������ ����� ����������� ������� 
				if(x_pos != 0 && y_pos != 0) Tetris.SetPixelState(x_pos, y_pos);
			}
			else //���� ������� ������� ��������, �������� ��������� �������
			{
				if (x_pos != 0 && y_pos != 0) Tetris.Fill(x_pos, y_pos);
			}
			rect.left = 20;
			rect.top = 20;
			rect.right = 430;
			rect.bottom = 430;
		InvalidateRect(hwnd, &rect, TRUE); //�������������� ����


		/*wsprintf(szBuffer, "XN-%d, YN-%d, x-%d, y-%d", x_pos,y_pos, x, y);
		MessageBox(hwnd, szBuffer, "mouse", NULL);*/
	     return 0;

	case WM_PAINT: //��������� � ����������� ����

		hdc = BeginPaint(hwnd, &ps); //�������� �������� ��������� 

		Tetris.CreateScreen(hdc); //�������������� ����

		//�������������� �������
		SetBkMode(hdc, TRANSPARENT);
		TextOut(hdc, 445, 90, szBuffer, wsprintf(szBuffer, "������ �����:"));
		if (state == 1) {
			TextOut(hdc, 445, 110, szBuffer, wsprintf(szBuffer, "���������"));
		}
		if (state == 2) {
			TextOut(hdc, 445, 110, szBuffer, wsprintf(szBuffer, "��������"));
		}

		//����������� �������� ��������� 
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY: //������� �������� ����
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam); //�������� ��������� � ��
}