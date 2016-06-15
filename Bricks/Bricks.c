
#include "resource.h"
#include "initial.h"
#define MAX_LOADSTRING 100

TCHAR szAppName[MAX_LOADSTRING];
TCHAR szClsName[MAX_LOADSTRING];
HINSTANCE hInst;

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpszCmdLine, int iCmdShow) {
	hInst = hInstance;
	LoadString(hInstance, IDS_CLASS_NAME, szClsName, MAX_LOADSTRING);
	LoadString(hInstance, IDS_APP_TITLE, szAppName, MAX_LOADSTRING);
	srand((unsigned)time(NULL));
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hIcon = LoadIconA(hInstance, MAKEINTRESOURCE(IDI_ICON3));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hInstance = hInstance;
	wndclass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wndclass.lpszClassName = szClsName;
	

	if (!RegisterClass(&wndclass)) {
		MessageBox(NULL, TEXT("The program requires Windows NT!"), szAppName, MB_ICONERROR);
		return 0;
	}
	hwnd = CreateWindow(
		szClsName,
		szAppName,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU  | WS_MINIMIZEBOX ,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		(WI_CXPADDING << 2) + BLOCK_XNUM*UNIT_LEN ,
		WI_CYPADDING + BLOCK_YNUM*UNIT_LEN + (WI_CBOTTOMPADDING << 3) + 3,
		NULL,
		NULL,
		hInstance,
		NULL
		);
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

void LandMines(char (*map)[BLOCK_XNUM], unsigned short * pfirstNotMine,
				unsigned short * pMineLoc) {
	unsigned short mark[BLOCK_XNUM * BLOCK_YNUM];
	int i, t, x, y;
	BOOL got = FALSE; 
	for (i = 0; i < BLOCK_XNUM * BLOCK_YNUM; i++) {
		mark[i] = i;
	}
	//�������
	for (i = 1; i <= MINE_NUM; i++) {
		t = rand() % (BLOCK_XNUM * BLOCK_YNUM - i + 1);
	
		map[mark[t] / BLOCK_XNUM][mark[t] % BLOCK_XNUM] = LID(BLOCK_MINE);
		pMineLoc[i - 1] = mark[t];
		mark[t] = mark[BLOCK_XNUM * BLOCK_YNUM - i];
	}
	//���������ͼ���������׷������Ϣ
	for (y = 0; y < BLOCK_YNUM; y++) {
		for (x = 0; x < BLOCK_XNUM; x++) {
			int cMine = 0;
			for (i = 0; map[y][x] != LID(BLOCK_MINE) && i < DIR_NUM; i++) {
				if (INBOND(0, y + dir[i][0], BLOCK_YNUM) &&
					INBOND(0, x + dir[i][1], BLOCK_XNUM) &&
					map[y + dir[i][0]][x + dir[i][1]] == LID(BLOCK_MINE)) {
					cMine++;
				}
			}
			//�ϴ�forѭ����iֵΪ�㣬����map[y][x]ֵ��ΪBLOCK_MINE,
			//����λ��λͼ�·��� cMineֵΪ0����ʾλͼ���·��Ŀշ���
			if (i) {
				map[y][x] = LID(BMP_NUM - cMine - 1); 
				if (!got) {
					*pfirstNotMine = (y << (1 << 3)) | x;
					got = TRUE;
				}
			}
		}
		
	}

}

void UnridVacant(char(*map)[BLOCK_XNUM], int nx, int ny, BOOL (*visited)[BLOCK_XNUM]) {
	int i;
	map[ny][nx] = UNLID(map[ny][nx]);
	visited[ny][nx] = TRUE;
	for (i = 0; i < DIR_NUM; i++) {
		int newX = nx + dir[i][1];
		int newY = ny + dir[i][0];
		if (INBOND(0, newX, BLOCK_XNUM) &&
			INBOND(0, newY, BLOCK_YNUM) &&
			!visited[newY][newX] && ISLIDDED(map[newY][newX]) ) {
			if (map[newY][newX] == LID(BLOCK_NONE)) {
				UnridVacant(map, newX, newY, visited);
			}
			else if (map[newY][newX] != LID(BLOCK_MINE)) {
				map[newY][newX] = UNLID(map[newY][newX]);
			}
		}
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	//��map�ķ���λ����ʾ��ʾ��������: �Ǹ�����ʾ��ʾ�� ������ʾ����
	static TCHAR map[BLOCK_YNUM][BLOCK_XNUM] = { 0 };
	static HBITMAP hBitmapBrick, hBitmapNum;
	static int cxSource, cySource;
	static BOOL isFstLBtnDw = TRUE, GameOver = FALSE;
	static unsigned short firstNotMine;
	static unsigned short MineLoc[MINE_NUM];
	BITMAP bitmap;
	HDC hdc, hdcMem;
	PAINTSTRUCT ps;
	int x, y;
	int mx, my, nx, ny;
	int i;
	
	switch (message)
	{
	case WM_CREATE:
		hBitmapBrick = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
		GetObject(hBitmapBrick, sizeof(BITMAP), &bitmap);
		cxSource = bitmap.bmWidth;
		cySource = bitmap.bmHeight;
		LandMines(map, &firstNotMine, MineLoc);
		return 0;
	case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			switch (wmId)
			{
			case IDO_CUSTOM:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, About);

			default:
				break;
			}
		}

	case WM_LBUTTONUP:
		mx = LOWORD(lParam);
		my = HIWORD(lParam);
		if (!GameOver && 
			INBOND(WI_CXPADDING, mx, WI_CXPADDING + BLOCK_XNUM*UNIT_LEN) &&
			INBOND(WI_CYPADDING, my, WI_CYPADDING + BLOCK_YNUM*UNIT_LEN)) {
			nx = (mx - WI_CXPADDING) / UNIT_LEN;
			ny = (my - WI_CYPADDING) / UNIT_LEN;
			
			//���״ε㿪�ǵ��ף������������������ͼ���׸������׵ķ��齻��
			//�������������������Χ��Χ�˸��������Ϣ
			if (map[ny][nx] == LID(BLOCK_MINE)) {
					if (isFstLBtnDw) {
						int fnmx = firstNotMine & 0x00ff;
						int fnmy = (firstNotMine >> 8) & 0x00ff;
						map[fnmy][fnmx] = LID(BLOCK_MINE);
						map[ny][nx] = BLOCK_NONE;   //��ʼ��Ϊ��ש
						for (i = 0; i < DIR_NUM; i++) {
							int newX = nx + dir[i][1];
							int newY = ny + dir[i][0];
							if (INBOND(0, newY, BLOCK_YNUM) && 
								INBOND(0, newX, BLOCK_XNUM) ) {
								//����Χ�����֣�����Χ������ʾ�����ּ�һ
								if (map[newY][newX] != LID(BLOCK_MINE)) {
									map[newY][newX] = NUM_DEC(map[newY][newX]);
								}
								//����Χ�е��ף��÷�����ʾ���ּ�һ
								else {
									map[ny][nx] = NUM_ASC(map[ny][nx]);				
								}
							}
							//���±������ķ������Χ�����������ʾ
							newX = fnmx + dir[i][1];
							newY = fnmy + dir[i][0];
							if (INBOND(0, newY, BLOCK_YNUM) && 
								INBOND(0, newX, BLOCK_XNUM) && 
								map[newY][newX] != LID(BLOCK_MINE)) {
								map[newY][newX] = NUM_ASC(map[newY][newX]);		
							}
						}
					}
					else {
						GameOver = TRUE;
						for (y = 0; y < BLOCK_YNUM; y++) {
							for (x = 0; x < BLOCK_XNUM; x++) {
								if (map[y][x] == LID(BLOCK_MINE)) {
									map[y][x] = UNLID(BLOCK_MINE);
								}
							}
						}
						map[ny][nx] = UNLID(BLOCK_REDMINE);
					}
				}
			else if (map[ny][nx] == LID(BLOCK_NONE)) {
				BOOL visited[BLOCK_YNUM][BLOCK_XNUM] = { 0 };
				UnridVacant(map, nx, ny, visited);
			}
			else {
				map[ny][nx] = UNLID(map[ny][nx]);
			}
			isFstLBtnDw = FALSE;
			InvalidateRect(hwnd, NULL, FALSE);
		}
		return 0;

	case WM_RBUTTONUP:
		mx = LOWORD(lParam);
		my = HIWORD(lParam);
		if (!GameOver &&
			INBOND(WI_CXPADDING, mx, WI_CXPADDING + BLOCK_XNUM*UNIT_LEN) &&
			INBOND(WI_CYPADDING, my, WI_CYPADDING + BLOCK_YNUM*UNIT_LEN)) {
			nx = (mx - WI_CXPADDING) / UNIT_LEN;
			ny = (my - WI_CYPADDING) / UNIT_LEN;
			if (ISLIDDED(map[ny][nx])) {
				map[ny][nx] = (((LIDWORD(map[ny][nx])+1) % LID_STATE) << 4) | (map[ny][nx] & 0x8f);
			}
		}
		InvalidateRect(hwnd, NULL, FALSE);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem, hBitmapBrick);

		for (y = 0; y < BLOCK_YNUM; y++) {
			for (x = 0; x < BLOCK_XNUM; x++) {
				BitBlt(	hdc,
						WI_CXPADDING + x*UNIT_LEN, WI_CYPADDING + y*UNIT_LEN,
						UNIT_LEN, UNIT_LEN, 
						hdcMem,
						0, UNIT_LEN*(ISLIDDED(map[y][x]) ? LIDWORD(map[y][x]) : UNLIDWORD(map[y][x])),
						SRCCOPY);
			}
		}
		DeleteDC(hdcMem);
		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		DeleteObject(hBitmapBrick);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

INT_PTR CALLBACK  About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}