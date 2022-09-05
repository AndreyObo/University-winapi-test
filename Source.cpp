#include <windows.h>
#include <fstream>
//При перерисовке поля происходит мерцание из-за отсутствия двойной буферизации


//Включаем стиль windows 10 для элементов управления
#pragma comment(linker,"\"/manifestdependency:type='win32' \ name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \ processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

//Определяем цвета
#define gray_color RGB(135, 146, 112)
#define black_color RGB(0,0,0)
#define font_color RGB(154, 166, 128)

//Определение функции обработки сообщений
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Точка входа для win32 приложений
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static char szAppName[] = "Tetris emulator"; //Заголовок окна


	HWND hwnd; //Дескриптор окна
	MSG msg; //Структура для сообщений windows
	WNDCLASSEX wndclass; //Класс окна

	//Описание класса главного окна
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
	RegisterClassEx(&wndclass); //Регистрируем его в ОС

	//Создание главного окна, регистрация его в системе, в hwnd помещаеться указатель для дальнейшего обращения к нему
	hwnd = CreateWindow(
		szAppName, 
		"Tetris emulator V - 0.5", // заголовок
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |  WS_MINIMIZEBOX, // Стиль, октключаем кнопку раскритыя на весь экран и возможность изменять размер окна
		CW_USEDEFAULT, // Позиция по x по умалчанию
		CW_USEDEFAULT, // позиция по умолчания
		580, // ширина окна
		475, // длинна окна
		NULL, // указатель на родительское окно, у нас нет т.к. оно главное
		NULL, // указатель на строку меню, у нас нет так что нулевой укащатель
		hInstance, //описатель данной программы
		NULL
	); 

	ShowWindow(hwnd, iCmdShow); //Отображаем окно 
	UpdateWindow(hwnd); //Перерисовываем окно

	while (GetMessage(&msg, NULL, 0, 0)) //Запускаем цикл обработки сообщений от windows
	{
		TranslateMessage(&msg); //Передаем сообщение в WndProc
		DispatchMessage(&msg); //Передаем сообщение в windows
	}
	return msg.wParam;
}


// Реализация главного класса
class ETetris {

public:

	ETetris() { //В конструкторе заполняем буфер экрана нулями

		for (int i = 0; i <= 19; i++) {
			for (int j = 0; j<= 19; j++) {
				screen_buffer[i][j] = 0;				
			}
		}
		

		TPen_gray = CreatePen(PS_SOLID, 1, gray_color); //Создаем кисти и перья GDI (Graphical Device Interface)

		TBrush_gray = CreateSolidBrush(gray_color);

		TPen_black = CreatePen(PS_SOLID, 1, black_color);

		TBrush_black = CreateSolidBrush(black_color);

		TBrush_font = CreateSolidBrush(font_color);
	}

	~ETetris() { //В деструкторе удалаяем GDI обьекты из памяти
		DeleteObject(TPen_gray);
		DeleteObject(TBrush_gray);
		DeleteObject(TPen_black);
		DeleteObject(TBrush_black);
		DeleteObject(TBrush_font);
	}
	

	void CreateScreen(HDC Thdc) { // Функция перерисовки поля, принимает контекст утройтва HDC куда рисуем
		x_disp = 20;
		y_disp = 20;

		

		ETetris::DrawFrame(Thdc, 10, 10, 415, 415); // Рисуем рамку


		for (int j = 0; j <= 19; j++) { // Рисуем поле
			x_disp = 20;
			for (int i = 0; i <= 19; i++) {
				if (screen_buffer[i][j] == 0) {
					DrawPixel(Thdc,Gray, x_disp, y_disp, 15, 15); // Функция рисования отдельного квадратика Пикселя
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
	void SetPixelState(int x, int y) { // Функция для редактирования буфера экрана
		
		if (screen_buffer[x-1][y-1] == 0) { // Инверсия состояния пикселя
			screen_buffer[x-1][y-1] = 1;
		}
		else
		{
			screen_buffer[x-1][y-1] = 0;
		}
	}

	void Fill(int x, int y) { // Рекурсивная функция заливки, принмает координаты точки

		if (x >= 0 && x < 20 && y >= 0 && y < 20 && screen_buffer[x][y] == 0 && screen_buffer[x][y] != 1)
		{
			screen_buffer[x][y] = 1;

			Fill(x + 1, y);
			Fill(x - 1, y);
			Fill(x, y + 1);
			Fill(x, y - 1);
		}
	}

	void Erase() { // Функция очистки поля
		for (int i = 0; i <= 19; i++) {
			for (int j = 0; j <= 19; j++) {
				screen_buffer[i][j] = 0;
			}
		}
	}

	void SaveScreen() { // Функция сохранения содержимого буфера в файл

		std::ofstream ScreenFile("ScreenBuffer.txt");
		for (int i = 0; i <= 19; i++) {
			for (int j = 0; j <= 19; j++) {
				ScreenFile << screen_buffer[j][i];
			}
			if(i != 19)  ScreenFile << std::endl;
		}

		ScreenFile.close();
	}

	void OpenScreen(HWND hwnd) { // Функция чтения из файла буфера экрана
		char buff[21];
		std::ifstream Screen("ScreenBuffer.txt");

		if (!Screen.is_open()) {
			MessageBox(hwnd, "Поле не сохранено или файл был поврежден!", "Ой", NULL);
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
	enum colors {Gray, Black};  // Состояния пикселя
	int screen_buffer[20][20]; // Буфер экрана
	int x_disp, y_disp;

	 HBRUSH TBrush_gray; // Кисти и перья
	 HPEN TPen_gray;
     HBRUSH TBrush_black;
	 HPEN TPen_black;
	 HBRUSH TBrush_font;


	void DrawPixel(HDC hdc, colors color, int x, int y, int width, int height) { //Функция рисования пикселя
	   
		if (color == Gray) {

			SelectObject(hdc, TPen_gray); // Выбор пера в контескт рисования
		}
		else {

			SelectObject(hdc, TPen_black); // Выбор пера в контескт рисования
		}


		SelectObject(hdc, TBrush_font); // Выбор кисти в контескт рисования, фон пикселя
		 

		Rectangle(hdc, x, y, x + width, y + height); // Рисуем прямоугольник

		if (color == Gray) { 
			
			SelectObject(hdc, TBrush_gray);
		}
		else {
			SelectObject(hdc, TBrush_black);
		}
		Rectangle(hdc, x + int(width*0.2), y + int(height*0.2), x + int(width*0.8), y + int(height*0.8));  // Рисуем внутренний прямоугольник

	}

	void DrawFrame(HDC hdc, int x, int y, int width, int height) {	// Рисуе рамку
		SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));
		Rectangle(hdc, x, y, x + width, y + height);
	}
};

ETetris Tetris; // Создаем экземпляр оьекта 


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) // Функция для обработки сообщений windows
{
	//Обьявляем переменные
	int x, y; //для координат мыши
	int x_pos, y_pos; // для координат пикселя в буфере(массиве) 
	bool finde_x, finde_y;
	HDC hdc; // Контекст устройства GDI
	PAINTSTRUCT ps; // для инофрмации от GDI
	static RECT rect; // для прямоугольника перерисовки части окна

	//Указатели на кнопки
	static HWND draw_button;
	static HWND Fill_button;
	static HWND Save_button;
	static HWND Load_button;
	static HWND Erase_button;

	static int state;
	

	static char szBuffer[50];


	switch (iMsg) //Разбор сообщения от windows
	{
	case WM_CREATE: //Событие создания окна
		state = 1;
		//Создаем кнопки
		draw_button = CreateWindow("button", "Рисовать", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 445, 12, 100, 20, hwnd, (HMENU)1, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Fill_button = CreateWindow("button", "Закрасить", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 445, 55, 100, 20, hwnd, (HMENU)2, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Save_button = CreateWindow("button", "Сохранить", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 445, 370, 100, 20, hwnd, (HMENU)3, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Load_button = CreateWindow("button", "Загрузить", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 445, 400, 100, 20, hwnd, (HMENU)4, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Erase_button = CreateWindow("button", "Очистить", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 445, 150, 100, 20, hwnd, (HMENU)5, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		rect.left = 20;
		rect.top = 20;
		rect.right = 430;
		rect.bottom = 430;
		return 0;

	case WM_COMMAND: //Событие клика по кнопкам, в целом сообщения от элементов управления в wParam и lParam передаються подробности о типе сообщения
		
		switch (LOWORD(wParam)) { //Определяем на какую кнопку было нажатие по её id которое передаеться в wParam, а задеться в функции CreateWindow, с приведением типа к (HMENU)
		case 1: 
			rect.left = 445;
			rect.top = 110;
			rect.right = 550;
			rect.bottom = 130;
			state = 1;
			InvalidateRect(hwnd, &rect, TRUE); //Изменяем текст надписи и перерисовываем только прямоугольник с этой надписью 
		break;

		case 2:
			rect.left = 445;
			rect.top = 110;
			rect.right = 550;
			rect.bottom = 130;
			state = 2;	
			InvalidateRect(hwnd, &rect, TRUE); //Изменяем текст надписи и перерисовываем только прямоугольник с этой надписью 
		break;

		case 3:
			Tetris.SaveScreen(); //Вызываем функция сохраниения буфера экрана
		break;

		case 4:
			Tetris.OpenScreen(hwnd); //Вызываем функцию чтения буфера экрана из файла
			rect.left = 20;
			rect.top = 20;
			rect.right = 430;
			rect.bottom = 430;
			InvalidateRect(hwnd, &rect, TRUE); //Перерисовываем поле
		break;
		case 5:
			Tetris.Erase(); //Вызываем функцию очистки экрана 
			rect.left = 20;
			rect.top = 20;
			rect.right = 430;
			rect.bottom = 430;
			InvalidateRect(hwnd, &rect, TRUE); //Перерисовываем поле
		break;
		}
		
		return 0;

	case WM_LBUTTONDOWN: //События щелчка по левой кнопки мыши
		x = LOWORD(lParam); // Определяем координаты
		y = HIWORD(lParam);
		x_pos = 0;
		y_pos = 0;
		finde_x = false;
		finde_y = false;
		if (x > 20 && x < 35) { //Определяем был ли клик по первому пикселю, если да переводим finde_x и finde_y в истину и невыполняем дальнейший перебор
			x_pos = 1;
			finde_x= true;
		}
		if (y > 20 && y < 35) {
			y_pos = 1;
			finde_y = true;
		}
		//Если это не первый пиксел ищем по остальным, межпикселяное рассотяние в 5px так же учитываеться и если клик был между пикселями это не взачет
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
			
			if (state == 1) { //Если активна функция рисования, меняем режим отображения пикселя 
				if(x_pos != 0 && y_pos != 0) Tetris.SetPixelState(x_pos, y_pos);
			}
			else //Если активна функция закраски, заливаем выбранную область
			{
				if (x_pos != 0 && y_pos != 0) Tetris.Fill(x_pos, y_pos);
			}
			rect.left = 20;
			rect.top = 20;
			rect.right = 430;
			rect.bottom = 430;
		InvalidateRect(hwnd, &rect, TRUE); //Перерисовываем поле


		/*wsprintf(szBuffer, "XN-%d, YN-%d, x-%d, y-%d", x_pos,y_pos, x, y);
		MessageBox(hwnd, szBuffer, "mouse", NULL);*/
	     return 0;

	case WM_PAINT: //Сообщение о перерисовке окна

		hdc = BeginPaint(hwnd, &ps); //Получаем контекст утройства 

		Tetris.CreateScreen(hdc); //Перерисовываем поле

		//Перерисовываем надписи
		SetBkMode(hdc, TRANSPARENT);
		TextOut(hdc, 445, 90, szBuffer, wsprintf(szBuffer, "Выбран режим:"));
		if (state == 1) {
			TextOut(hdc, 445, 110, szBuffer, wsprintf(szBuffer, "Рисование"));
		}
		if (state == 2) {
			TextOut(hdc, 445, 110, szBuffer, wsprintf(szBuffer, "Закраска"));
		}

		//Освобождаем контекст утройства 
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY: //События закрытия окна
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam); //Передача сообщения в ОС
}