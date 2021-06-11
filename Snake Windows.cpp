// Snake Windows.cpp : Define el punto de entrada de la aplicación.
//

#include "framework.h"
#include "Serpiente.h"
#include "Snake Windows.h"
#include "shellapi.h"

#pragma comment(lib, "comctl32.lib")

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")

#define MAX_LOADSTRING 100

//Tamaño máximo de buffer y el puerto de conexión.
#define DEFAULT_BUFLEN 		512
#define DEFAULT_PORT		"4200"



//PARA LA SERPIENTE
struct pos {
    int x;
    int y;
}; typedef struct pos POS;

struct PedacitoS {
    POS pos;
    int tipo;
    int dir;
}; typedef struct PedacitoS PEDACITOS;

struct Comida {
    POS pos;
    int tipo;
}; typedef struct Comida COMIDA;

COMIDA com = { {0,0}, NADA };
static bool atemptConect = false;
static int waitingConect = 0;



static int tipoUs = 0;
static int soloUs = 3;
static int tams = 5;
static int tams2 = 5;
static bool band = true;
static int cuenta = 0;
RECT rect1;
static PEDACITOS* serpiente = NULL;
static PEDACITOS* serpiente2 = NULL;

PEDACITOS* NuevaSerpiente(int, int, int);
void DibujarSerpiente(HDC, const PEDACITOS *);
int MoverSerpiente(PEDACITOS *, int, RECT, int);
PEDACITOS* AjustarSerpiente(PEDACITOS *, int *, int, RECT);
int Colisionar(const PEDACITOS *, int);
int Comer(const PEDACITOS *, int);
void MsgRecibir(HWND, char*);
void MsgEnviar(char* hEscribir, HWND hIP, HWND hWnd);

//Declaración de funciones necesarias para la comunicación y muestreo
int                 Cliente(char* szDirIP, PSTR pstrMensaje, HWND hWnd);
//Declaración de la función Servidor que será lanzada como hilo
DWORD WINAPI        Servidor(LPVOID argumento);


char* NumChar(int);

//Variables globales para la ip de la máquina por default y el nombre de la máquina
char szMiIP[17] = "192.168.0.4";
char szUsuario[32] = "lap";





// Variables globales:
HINSTANCE hInst;                                // instancia actual
WCHAR szTitle[MAX_LOADSTRING];                  // Texto de la barra de título
WCHAR szWindowClass[MAX_LOADSTRING];            // nombre de clase de la ventana principal

// Declaraciones de funciones adelantadas incluidas en este módulo de código:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Colocar código aquí.
    TCHAR tchIP[17], tchszUsuario[32];
    if (_tcslen(lpCmdLine) > 10) {
        swscanf(lpCmdLine, L"%s %s", tchIP, tchszUsuario);
        wcstombs(szMiIP, tchIP, 17);
        wcstombs(szUsuario, tchszUsuario, 32);
    }

    // Inicializar cadenas globales
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SNAKEWINDOWS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Realizar la inicialización de la aplicación:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SNAKEWINDOWS));

    MSG msg;

    // Bucle principal de mensajes:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCIÓN: MyRegisterClass()
//
//  PROPÓSITO: Registra la clase de ventana.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SNAKEWINDOWS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SNAKEWINDOWS);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCIÓN: InitInstance(HINSTANCE, int)
//
//   PROPÓSITO: Guarda el identificador de instancia y crea la ventana principal
//
//   COMENTARIOS:
//
//        En esta función, se guarda el identificador de instancia en una variable común y
//        se crea y muestra la ventana principal del programa.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Almacenar identificador de instancia en una variable global

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
       CW_USEDEFAULT, 0, 600, 400, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCIÓN: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PROPÓSITO: Procesa mensajes de la ventana principal.
//
//  WM_COMMAND  - procesar el menú de aplicaciones
//  WM_PAINT    - Pintar la ventana principal
//  WM_DESTROY  - publicar un mensaje de salida y volver
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    RECT rect;
    char datosEnviar[20] = "";

    HFONT fuenteC;
    static HINSTANCE hInstance;
    static HWND mostrarTXT, escribirTXT, ipTXT, enviarBTN;
    static HANDLE threadServer, threadCliente;
    static DWORD threadServerID, threadClienteID;
    static HPEN hpenRed, hpenGreen, hpenGray;
    static HBRUSH hbrRed, hbrGreen, hbrGray;

    switch (message)
    {
    case WM_CREATE:
        {
            hpenRed = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
            hpenGreen = CreatePen(PS_SOLID, 1, RGB(45, 255, 45));
            hpenGray = CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
            hbrRed = CreateSolidBrush(RGB(255, 0, 0));
            hbrGreen = CreateSolidBrush(RGB(45, 255, 45));
            hbrGray = CreateSolidBrush(RGB(128, 128, 128));
            
            serpiente = NuevaSerpiente(tams, 1, 1);
            serpiente2 = NuevaSerpiente(tams2, 10, 10);

            ipTXT = CreateWindowEx(0, L"Edit", L"", ES_LEFT |
                WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP, 200, 130, 100, 25,
                hWnd,
                (HMENU)ID_IPTXT,
                hInst,
                NULL);
            //SetTimer(hWnd, IDT_TIMER1, 500, NULL);
        }
    break;

    case WM_TIMER: {
        switch (wParam)
        {
            case IDT_TIMER1: 
            {
                GetClientRect(hWnd, &rect);
                
                if (!MoverSerpiente(serpiente, serpiente[tams - 1].dir, rect, tams)) {
                    KillTimer(hWnd, IDT_TIMER1);
                    MessageBox(hWnd, L"Ya se murió", L"Fin del juego", MB_OK | MB_ICONINFORMATION);
                }

                if (!MoverSerpiente(serpiente2, serpiente2[tams2 - 1].dir, rect, tams2)) {
                    KillTimer(hWnd, IDT_TIMER1);
                    MessageBox(hWnd, L"Ya se murió", L"Fin del juego", MB_OK | MB_ICONINFORMATION);
                }
                
                cuenta++;
                if (cuenta == 15) {
                    if (rand() % 100 < 80) {
                        com.tipo = CRECE;
                    }
                    else {
                        com.tipo = ACHICA;
                    }
                    com.pos.x = rand() % rect.right/TAMSERP;
                    com.pos.y = rand() % rect.bottom/TAMSERP;
                    cuenta = 0;
                }
                if (Comer(serpiente, tams)) {
                    serpiente = AjustarSerpiente(serpiente, &tams, com.tipo, rect);
                    com.tipo = NADA;
                }
                if (Comer(serpiente2, tams)) {
                    serpiente2 = AjustarSerpiente(serpiente2, &tams2, com.tipo, rect);
                    com.tipo = NADA;
                }
                InvalidateRect(hWnd, NULL, TRUE);
                
            }
            break;
        }
    }
        break;
    case WM_KEYDOWN: {
        GetClientRect(hWnd, &rect);
        switch (wParam)
        {
            case VK_UP: {
                
                if (tipoUs == 0 || soloUs == 1) {
                    if (soloUs != 1)
                    {
                        strcpy(datosEnviar, "");
                        strcat(datosEnviar, NumChar(0));
                        strcat(datosEnviar, NumChar(ARRIBA));
                        strcat(datosEnviar, NumChar(tams));
                        MsgEnviar(datosEnviar, ipTXT, hWnd);
                    }
                    

                    if (!MoverSerpiente(serpiente, ARRIBA, rect, tams)) {
                        KillTimer(hWnd, IDT_TIMER1);
                        MessageBox(hWnd, L"Ya se murió", L"Fin del juego", MB_OK | MB_ICONINFORMATION);
                    }

                    if (Comer(serpiente, tams)) {
                        serpiente = AjustarSerpiente(serpiente, &tams, com.tipo, rect);
                        com.tipo = NADA;
                    }

                    
                    InvalidateRect(hWnd, NULL, TRUE);
                }
                if (tipoUs == 1) {
                    strcpy(datosEnviar, "");
                    strcat(datosEnviar, NumChar(1));
                    strcat(datosEnviar, NumChar(ARRIBA));
                    strcat(datosEnviar, NumChar(tams2));

                    if (!MoverSerpiente(serpiente2, ARRIBA, rect, tams2)) {
                        KillTimer(hWnd, IDT_TIMER1);
                        MessageBox(hWnd, L"Ya se murió", L"Fin del juego", MB_OK | MB_ICONINFORMATION);
                    }

                    if (Comer(serpiente2, tams)) {
                        serpiente2 = AjustarSerpiente(serpiente2, &tams2, com.tipo, rect);
                        com.tipo = NADA;
                    }

                    MsgEnviar(datosEnviar, ipTXT, hWnd);

                    InvalidateRect(hWnd, NULL, TRUE);
                }

                
                break;
            }
            case VK_DOWN: {
                

                if (tipoUs == 0 || soloUs == 1) {
                    if (soloUs != 1)
                    {
                        strcpy(datosEnviar, "");
                        strcat(datosEnviar, NumChar(0));
                        strcat(datosEnviar, NumChar(ABAJO));
                        strcat(datosEnviar, NumChar(tams));
                        MsgEnviar(datosEnviar, ipTXT, hWnd);
                    }
                    if (!MoverSerpiente(serpiente, ABAJO, rect, tams)) {
                        KillTimer(hWnd, IDT_TIMER1);
                        MessageBox(hWnd, L"Ya se murió", L"Fin del juego", MB_OK | MB_ICONINFORMATION);
                    }

                    if (Comer(serpiente, tams)) {
                        serpiente = AjustarSerpiente(serpiente, &tams, com.tipo, rect);
                        com.tipo = NADA;
                    }

                    
                    InvalidateRect(hWnd, NULL, TRUE);
                }
                if (tipoUs == 1) {
                    strcpy(datosEnviar, "");
                    strcat(datosEnviar, NumChar(1));
                    strcat(datosEnviar, NumChar(ABAJO));
                    strcat(datosEnviar, NumChar(tams2));
                    if (!MoverSerpiente(serpiente2, ABAJO, rect, tams2)) {
                        KillTimer(hWnd, IDT_TIMER1);
                        MessageBox(hWnd, L"Ya se murió", L"Fin del juego", MB_OK | MB_ICONINFORMATION);
                    }

                    if (Comer(serpiente2, tams)) {
                        serpiente2 = AjustarSerpiente(serpiente2, &tams2, com.tipo, rect);
                        com.tipo = NADA;
                    }

                    MsgEnviar(datosEnviar, ipTXT, hWnd);

                    InvalidateRect(hWnd, NULL, TRUE);
                }
                break;
            }
            case VK_LEFT: {
                

                if (tipoUs == 0 || soloUs == 1) {
                    if (soloUs != 1)
                    {
                        strcpy(datosEnviar, "");
                        strcat(datosEnviar, NumChar(0));
                        strcat(datosEnviar, NumChar(IZQ));
                        strcat(datosEnviar, NumChar(tams));
                        MsgEnviar(datosEnviar, ipTXT, hWnd);
                    }
                    
                    if (!MoverSerpiente(serpiente, IZQ, rect, tams)) {
                        KillTimer(hWnd, IDT_TIMER1);
                        MessageBox(hWnd, L"Ya se murió", L"Fin del juego", MB_OK | MB_ICONINFORMATION);
                    }

                    if (Comer(serpiente, tams)) {
                        serpiente = AjustarSerpiente(serpiente, &tams, com.tipo, rect);
                        com.tipo = NADA;
                    }

                    
                    InvalidateRect(hWnd, NULL, TRUE);
                }
                if (tipoUs == 1) {
                    strcpy(datosEnviar, "");
                    strcat(datosEnviar, NumChar(1));
                    strcat(datosEnviar, NumChar(IZQ));
                    strcat(datosEnviar, NumChar(tams2));
                    if (!MoverSerpiente(serpiente2, IZQ, rect, tams2)) {
                        KillTimer(hWnd, IDT_TIMER1);
                        MessageBox(hWnd, L"Ya se murió", L"Fin del juego", MB_OK | MB_ICONINFORMATION);
                    }

                    if (Comer(serpiente2, tams)) {
                        serpiente2 = AjustarSerpiente(serpiente2, &tams2, com.tipo, rect);
                        com.tipo = NADA;
                    }

                    MsgEnviar(datosEnviar, ipTXT, hWnd);

                    InvalidateRect(hWnd, NULL, TRUE);
                }

                break;
            }
            case VK_RIGHT: {
                

                if (tipoUs == 0 || soloUs == 1) {
                    if (soloUs != 1)
                    {
                        strcpy(datosEnviar, "");
                        strcat(datosEnviar, NumChar(0));
                        strcat(datosEnviar, NumChar(DER));
                        strcat(datosEnviar, NumChar(tams));
                        MsgEnviar(datosEnviar, ipTXT, hWnd);
                    }
                    
                    if (!MoverSerpiente(serpiente, DER, rect, tams)) {
                        KillTimer(hWnd, IDT_TIMER1);
                        MessageBox(hWnd, L"Ya se murió", L"Fin del juego", MB_OK | MB_ICONINFORMATION);
                    }

                    if (Comer(serpiente, tams)) {
                        serpiente = AjustarSerpiente(serpiente, &tams, com.tipo, rect);
                        com.tipo = NADA;
                    }

                    
                    InvalidateRect(hWnd, NULL, TRUE);
                }
                if (tipoUs == 1) {
                    strcpy(datosEnviar, "");
                    strcat(datosEnviar, NumChar(1));
                    strcat(datosEnviar, NumChar(DER));
                    strcat(datosEnviar, NumChar(tams2));
                    if (!MoverSerpiente(serpiente2, DER, rect, tams2)) {
                        KillTimer(hWnd, IDT_TIMER1);
                        MessageBox(hWnd, L"Ya se murió", L"Fin del juego", MB_OK | MB_ICONINFORMATION);
                    }

                    if (Comer(serpiente2, tams)) {
                        serpiente2 = AjustarSerpiente(serpiente2, &tams2, com.tipo, rect);
                        com.tipo = NADA;
                    }

                    MsgEnviar(datosEnviar, ipTXT, hWnd);

                    InvalidateRect(hWnd, NULL, TRUE);
                }

                
                break;
            }
        }
    }
    break;

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
            case IDM_NUEVO: {
                soloUs = 1;
                if (serpiente != NULL) {
                    KillTimer(hWnd, IDT_TIMER1);
                    free(serpiente);
                    tams = 5;
                    cuenta = 0;
                    tipoUs = 0;
                    serpiente = NuevaSerpiente(tams, 1, 1);
                    //serpiente2 = NuevaSerpiente(tams2, 10, 10);
                    SetTimer(hWnd, IDT_TIMER1, 500, NULL);
                    InvalidateRect(hWnd, NULL, TRUE);
                }
                else if (serpiente == NULL){
                    KillTimer(hWnd, IDT_TIMER1);
                    tams = 5;
                    cuenta = 0;
                    tipoUs = 0;
                    serpiente = NuevaSerpiente(tams, 1, 1);
                    //serpiente2 = NuevaSerpiente(tams2, 10, 10);
                    SetTimer(hWnd, IDT_TIMER1, 500, NULL);
                    InvalidateRect(hWnd, NULL, TRUE);
                }

                atemptConect = false;
                waitingConect = 0;
                break;
            }
            case IDM_ACOMPA: {
                MessageBox(NULL, L"BBBBBBbbbbbbbbbbb", L"Acompañado", MB_OK | MB_ICONINFORMATION);
                //serpiente = NuevaSerpiente(tams, 1, 1);
                //serpiente2 = NuevaSerpiente(tams2, 10, 10);
                // Se crea el hilo pasandole el tipo de seguridas, el tamaño de la pila, la función que 
                //se levantará en el hilo, el argumento de la función y la devolución del 
                //identificador del hilo
                tipoUs = 0;
                soloUs = 0;
                waitingConect = 1;
                threadServer = CreateThread(NULL, 0, Servidor,
                    (LPVOID)hWnd, 0, &threadServerID);
                //Validamos la creación del hilo para le servidor
                if (threadServer == NULL) {
                    MessageBox(hWnd, L"Error al crear el hilo para el servidor", L"Error", MB_OK | MB_ICONERROR);
                }
                InvalidateRect(hWnd, NULL, TRUE);
                break;
            }
            case IDM_CONECT: {
                atemptConect = true;
                waitingConect = 2;
                tipoUs = 1;
                soloUs = 0;

                //serpiente = NuevaSerpiente(tams, 1, 1);
                //serpiente2 = NuevaSerpiente(tams2, 10, 10);
                //MessageBox(NULL, (LPCWSTR)ipTXT, L"XD", MB_OK | MB_ICONINFORMATION);
                threadCliente = CreateThread(NULL, 0, Servidor,
                    (LPVOID)hWnd, 0, &threadClienteID);
                //Validamos la creación del hilo para le servidor
                if (threadCliente == NULL) {
                    MessageBox(hWnd, L"Error al crear el hilo para el servidor", L"Error", MB_OK | MB_ICONERROR);
                }
                
               // hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
                //LoadLibrary(L"riched20.dll");
                            
                
                InvalidateRect(hWnd, NULL, TRUE);
              
                
                break;
            }
            case IDM_ABOUT:
                MessageBox(NULL, L"Alumno: Erick Oswaldo Gallegos Pérez \n Semestre: Octavo. \n Carrera: Ingeniería en computación. \n Materia: Sistemas de Cómputo Paralelo y Distribuido.", L"Acerca de", MB_OK | MB_ICONINFORMATION);
                //DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_SALIR:
                if (soloUs == 1)
                {
                    free(serpiente);
                }
                if (soloUs != 1)
                {
                    if (serpiente != NULL) {
                        free(serpiente);
                    }
                    if (serpiente2 != NULL) {
                        free(serpiente2);
                    }
                }
                
                
                DestroyWindow(hWnd);
                break;
            case ID_ENVIARBTN:
                atemptConect = false;
                MsgEnviar(datosEnviar, ipTXT, hWnd);
                cuenta = 0;
                InvalidateRect(hWnd, NULL, TRUE);
                //MsgEnviar(mostrarTXT, escribirTXT, ipTXT);
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        hdc = BeginPaint(hWnd, &ps);

        HPEN hpenTemp;
        HBRUSH hbrTemp;

        
        // TODO: Agregar cualquier código de dibujo que use hDC aquí...
        if (soloUs != 1) {
            hpenTemp = (HPEN)SelectObject(hdc, hpenRed);
            hbrTemp = (HBRUSH)SelectObject(hdc, hbrRed);
            DibujarSerpiente(hdc, serpiente);
            SelectObject(hdc, hpenTemp);
            SelectObject(hdc, hbrTemp);
            hpenTemp = (HPEN)SelectObject(hdc, hpenGreen);
            hbrTemp = (HBRUSH)SelectObject(hdc, hbrGreen);
            DibujarSerpiente(hdc, serpiente2);
            SelectObject(hdc, hpenTemp);
            SelectObject(hdc, hbrTemp);
        }
        if (soloUs == 1)
        {
            hpenTemp = (HPEN)SelectObject(hdc, hpenRed);
            hbrTemp = (HBRUSH)SelectObject(hdc, hbrRed);
            DibujarSerpiente(hdc, serpiente);
            SelectObject(hdc, hpenTemp);
            SelectObject(hdc, hbrTemp);
        }
        
        if (com.tipo == CRECE) {
            RoundRect(hdc, com.pos.x * TAMSERP,
                com.pos.y * TAMSERP,
                com.pos.x * TAMSERP + TAMSERP,
                com.pos.y * TAMSERP + TAMSERP,
                7, 7);
        }
        else if (com.tipo == ACHICA) {
            Ellipse(hdc, com.pos.x * TAMSERP,
                com.pos.y * TAMSERP,
                com.pos.x * TAMSERP + TAMSERP,
                com.pos.y * TAMSERP + TAMSERP);
        }

        if (waitingConect == 1) {
            TextOut(hdc, 400, 200, L"CREANDO", sizeof("CREANDO"));
            TextOut(hdc, 200, 200, L"En espera!!!", sizeof("En espera!!!"));
        }
        if (waitingConect == 2) {
            TextOut(hdc, 400, 200, L"CONECTADO!!!", sizeof("CONECTADO!!!"));
        }

        if (atemptConect) {
            TextOut(hdc, 200, 100, L"CONECTARSE", sizeof("CONECTARSE"));
            TextOut(hdc, 200, 100, L"Introduce el IP:", sizeof("Introduce el IP:"));
        }
        
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        free(serpiente);
        DeleteObject(hpenRed);
        DeleteObject(hpenGreen);
        DeleteObject(hpenGray);
        DeleteObject(hbrRed);
        DeleteObject(hbrGreen);
        DeleteObject(hbrGray);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Controlador de mensajes del cuadro Acerca de.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


PEDACITOS * NuevaSerpiente(int tams, int enX, int enY) {
    PEDACITOS * serpiente = NULL;
    int i;

    if (tams < 2)
        tams = 2;
    serpiente = (PEDACITOS *)malloc(sizeof(PEDACITOS) * tams);
    if (serpiente == NULL) {
        MessageBox(NULL, L"Sin memoria", L"Error", MB_OK | MB_ICONERROR);
        exit(0);
    }
    serpiente[0].tipo = COLA;
    serpiente[0].pos.x = enX;
    serpiente[0].pos.y = enY;
    serpiente[0].dir = DER;
    for (i = 1; i < tams - 1; i++) {
        serpiente[i].tipo = CUERPO;
        serpiente[i].pos.x = i + enX;
        serpiente[i].pos.y = enY;
        serpiente[i].dir = DER;
    }
    serpiente[i].tipo = CABEZA;
    serpiente[i].pos.x = tams + (enX-1);
    serpiente[i].pos.y = enY;
    serpiente[i].dir = DER;

    return serpiente;
}

void DibujarSerpiente(HDC hdc, const PEDACITOS *serpiente) {
    static HPEN hpenRed, hpenGreen, hpenGray;
    static HBRUSH hbrRed, hbrGreen, hbrGray;

    hpenRed = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
    hpenGreen = CreatePen(PS_SOLID, 1, RGB(45, 255, 45));
    hpenGray = CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
    hbrRed = CreateSolidBrush(RGB(255, 0, 0));
    hbrGreen = CreateSolidBrush(RGB(45, 255, 45));
    hbrGray = CreateSolidBrush(RGB(128, 128, 128));

    int i = 1;
    switch (serpiente[0].dir)
    {
        case DER:
            MoveToEx(hdc, serpiente[0].pos.x * TAMSERP + TAMSERP,
                        serpiente[0].pos.y * TAMSERP, NULL);
            LineTo(hdc, serpiente[0].pos.x * TAMSERP,
                        serpiente[0].pos.y * TAMSERP + TAMSERP/2);
            LineTo(hdc, serpiente[0].pos.x * TAMSERP + TAMSERP,
                        serpiente[0].pos.y * TAMSERP + TAMSERP);
            LineTo(hdc, serpiente[0].pos.x * TAMSERP + TAMSERP,
                        serpiente[0].pos.y * TAMSERP);
            break;
        case IZQ:
            MoveToEx(hdc, serpiente[0].pos.x * TAMSERP,
                        serpiente[0].pos.y * TAMSERP, NULL);
            LineTo(hdc, serpiente[0].pos.x * TAMSERP + TAMSERP,
                        serpiente[0].pos.y * TAMSERP + TAMSERP/2);
            LineTo(hdc, serpiente[0].pos.x * TAMSERP,
                        serpiente[0].pos.y * TAMSERP + TAMSERP);
            LineTo(hdc, serpiente[0].pos.x * TAMSERP,
                        serpiente[0].pos.y * TAMSERP);
            break;
        case ARRIBA:
            MoveToEx(hdc, serpiente[0].pos.x * TAMSERP,
                        serpiente[0].pos.y * TAMSERP, NULL);
            LineTo(hdc, serpiente[0].pos.x * TAMSERP + TAMSERP/2,
                        serpiente[0].pos.y * TAMSERP + TAMSERP);
            LineTo(hdc, serpiente[0].pos.x * TAMSERP + TAMSERP,
                        serpiente[0].pos.y * TAMSERP);
            LineTo(hdc, serpiente[0].pos.x * TAMSERP,
                        serpiente[0].pos.y * TAMSERP);
            break;
        case ABAJO:
            MoveToEx(hdc, serpiente[0].pos.x * TAMSERP,
                        serpiente[0].pos.y * TAMSERP + TAMSERP, NULL);
            LineTo(hdc, serpiente[0].pos.x * TAMSERP + TAMSERP/2,
                        serpiente[0].pos.y * TAMSERP);
            LineTo(hdc, serpiente[0].pos.x * TAMSERP + TAMSERP,
                        serpiente[0].pos.y * TAMSERP + TAMSERP);
            LineTo(hdc, serpiente[0].pos.x * TAMSERP,
                        serpiente[0].pos.y * TAMSERP + TAMSERP);
            break;
        default:
            break;
    }
    while (serpiente[i].tipo != CABEZA) {
        RoundRect(hdc, serpiente[i].pos.x * TAMSERP,
                    serpiente[i].pos.y * TAMSERP,
                    serpiente[i].pos.x * TAMSERP + TAMSERP,
                    serpiente[i].pos.y * TAMSERP + TAMSERP,
                    5, 5);
        i++;
    }
    RoundRect(hdc, serpiente[i].pos.x * TAMSERP,
                serpiente[i].pos.y * TAMSERP,
                serpiente[i].pos.x * TAMSERP + TAMSERP,
                serpiente[i].pos.y * TAMSERP + TAMSERP,
                5, 5);

    HPEN hpenTemp;
    HBRUSH hbrTemp;

    hpenTemp = (HPEN)SelectObject(hdc, hpenGray);
    hbrTemp = (HBRUSH)SelectObject(hdc, hbrGray);
    switch (serpiente[i].dir)
    {
        case DER:
            Ellipse(hdc, serpiente[i].pos.x * TAMSERP,
                        serpiente[i].pos.y * TAMSERP,
                        serpiente[i].pos.x * TAMSERP + TAMSERP/2,
                        serpiente[i].pos.y * TAMSERP + TAMSERP/2);
            Ellipse(hdc, serpiente[i].pos.x * TAMSERP,
                        serpiente[i].pos.y * TAMSERP + TAMSERP/2,
                        serpiente[i].pos.x * TAMSERP + TAMSERP/2,
                        serpiente[i].pos.y * TAMSERP + TAMSERP);
            break;
        case IZQ:
            Ellipse(hdc, serpiente[i].pos.x * TAMSERP + TAMSERP/2,
                        serpiente[i].pos.y * TAMSERP,
                        serpiente[i].pos.x * TAMSERP + TAMSERP,
                        serpiente[i].pos.y * TAMSERP + TAMSERP/2);
            Ellipse(hdc, serpiente[i].pos.x * TAMSERP + TAMSERP/2,
                        serpiente[i].pos.y * TAMSERP + TAMSERP/2,
                        serpiente[i].pos.x * TAMSERP + TAMSERP,
                        serpiente[i].pos.y * TAMSERP + TAMSERP);
            break;
        case ARRIBA:
            Ellipse(hdc, serpiente[i].pos.x * TAMSERP,
                        serpiente[i].pos.y * TAMSERP + TAMSERP/2,
                        serpiente[i].pos.x * TAMSERP + TAMSERP/2,
                        serpiente[i].pos.y * TAMSERP + TAMSERP);
            Ellipse(hdc, serpiente[i].pos.x * TAMSERP + TAMSERP/2,
                        serpiente[i].pos.y * TAMSERP + TAMSERP/2,
                        serpiente[i].pos.x * TAMSERP + TAMSERP,
                        serpiente[i].pos.y * TAMSERP + TAMSERP);
            break;
        case ABAJO:
            Ellipse(hdc, serpiente[i].pos.x * TAMSERP,
                        serpiente[i].pos.y * TAMSERP,
                        serpiente[i].pos.x * TAMSERP + TAMSERP/2,
                        serpiente[i].pos.y * TAMSERP + TAMSERP/2);
            Ellipse(hdc, serpiente[i].pos.x * TAMSERP + TAMSERP/2,
                        serpiente[i].pos.y * TAMSERP,
                        serpiente[i].pos.x * TAMSERP + TAMSERP,
                        serpiente[i].pos.y * TAMSERP + TAMSERP/2);
            break;
        default:
            break;
    }
    SelectObject(hdc, hpenTemp);
    SelectObject(hdc, hbrTemp);
}


int MoverSerpiente(PEDACITOS *serpiente, int dir, RECT rect, int tams) {
    int i = 0;
    while (serpiente[i].tipo != CABEZA) {
        serpiente[i].dir = serpiente[i+1].dir;
        serpiente[i].pos = serpiente[i+1].pos;
        i++;
    }

    switch (serpiente[i].dir)
    {
        case DER:
            if (dir != IZQ)
                serpiente[i].dir = dir;
            break;
        case IZQ:
            if (dir != DER)
                serpiente[i].dir = dir;
            break;
        case ARRIBA:
            if (dir != ABAJO)
                serpiente[i].dir = dir;
            break;
        case ABAJO:
            if (dir != ARRIBA)
                serpiente[i].dir = dir;
            break;
        default:
            break;
    }

    switch (serpiente[i].dir)
    {
        case DER:
            serpiente[i].pos.x = serpiente[i].pos.x + 1;
            if (serpiente[i].pos.x >= rect.right/TAMSERP)
                serpiente[i].pos.x = 0;
            break;
        case IZQ:
            serpiente[i].pos.x = serpiente[i].pos.x - 1;
            if (serpiente[i].pos.x < 0)
                serpiente[i].pos.x = rect.right/TAMSERP;
            break;
        case ARRIBA:
            serpiente[i].pos.y = serpiente[i].pos.y - 1;
            if (serpiente[i].pos.y < 0)
                serpiente[i].pos.y = rect.bottom/TAMSERP;
            break;
        case ABAJO:
            serpiente[i].pos.y = serpiente[i].pos.y + 1;
            if (serpiente[i].pos.y >= rect.bottom/TAMSERP)
                serpiente[i].pos.y = 0;
            break;
        default:
            break;
    }
    return !Colisionar(serpiente, tams);
}

int Colisionar(const PEDACITOS *serpiente, int tams) {
    int i = 0;
    while (serpiente[i].tipo != CABEZA) {
        if (serpiente[i].pos.x == serpiente[tams-1].pos.x &&
            serpiente[i].pos.y == serpiente[tams-1].pos.y) {
            return 1;
        }
        i++;
    }
    return 0;
}


PEDACITOS * AjustarSerpiente(PEDACITOS *serpiente, int *tams, int comida, RECT rect) {
    int i;
    PEDACITOS cabeza = serpiente[*tams-1];
    switch (comida)
    {
        case CRECE: {
            (*tams)++;
            serpiente = (PEDACITOS *)realloc(serpiente, sizeof(PEDACITOS)*(*tams));
            serpiente[*tams-2].tipo = CUERPO;
            serpiente[*tams-1] = cabeza;
            i = *tams-1;
            switch (serpiente[i].dir)
            {
                case DER:
                    serpiente[i].pos.x = serpiente[i].pos.x + 1;
                    if (serpiente[i].pos.x >= rect.right / TAMSERP)
                        serpiente[i].pos.x = 0;
                    break;
                case IZQ:
                    serpiente[i].pos.x = serpiente[i].pos.x - 1;
                    if (serpiente[i].pos.x < 0)
                        serpiente[i].pos.x = rect.right / TAMSERP;
                    break;
                case ARRIBA:
                    serpiente[i].pos.y = serpiente[i].pos.y - 1;
                    if (serpiente[i].pos.y < 0)
                        serpiente[i].pos.y = rect.bottom / TAMSERP;
                    break;
                case ABAJO:
                    serpiente[i].pos.y = serpiente[i].pos.y + 1;
                    if (serpiente[i].pos.y >= rect.bottom / TAMSERP)
                        serpiente[i].pos.y = 0;
                    break;
                default:
                    break;
            }
            break;
        }
        case ACHICA: {
            if (*tams > 2) {
                i = 0;
                while (serpiente[i].tipo != CABEZA) {
                    serpiente[i] = serpiente[i+1];
                    i++;
                }
                (*tams)--;
                serpiente = (PEDACITOS *) realloc(serpiente, sizeof(PEDACITOS)*(*tams));
                serpiente[*tams-1] = cabeza;
            }
            break;
        }
        default:
            break;
    }
    return serpiente;
}

int Comer(const PEDACITOS *serpiente, int tams) {
    if (serpiente[tams - 1].pos.x == com.pos.x &&
        serpiente[tams - 1].pos.y == com.pos.y) {
        return 1;
    }
    return 0;
}

DWORD WINAPI Servidor(LPVOID argumento) {
    HWND hWnd = (HWND)argumento;
    WSADATA wsaData;
    int iResult;
    int aux;
    TCHAR msgFalla[256];

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    int iSendResult;
    int recvbuflen = DEFAULT_BUFLEN;
    char szBuffer[256], szIP[16], szNN[32];



    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        wsprintf(msgFalla, L"WSAStartup failed with error: %d\n", iResult);
        MessageBox(NULL, msgFalla, L"Error em servidore", MB_OK | MB_ICONERROR);

        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        wsprintf(msgFalla, L"getaddrinfo failed with error: %d", iResult);
        MessageBox(NULL, msgFalla, L"Error en servidor", MB_OK | MB_ICONERROR);
        WSACleanup();

        return 1;
    }

    // Crear SOCKET para la coneccion al server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        wsprintf(msgFalla, L"socket failed with error: %d", WSAGetLastError());
        MessageBox(NULL, msgFalla, L"Error en servidor", MB_OK | MB_ICONERROR);
        freeaddrinfo(result);
        WSACleanup();

        return 1;
    }
    

    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        wsprintf(msgFalla, L"bind failed with error: %d\n", WSAGetLastError());
        MessageBox(NULL, msgFalla, L"Error en servidor", MB_OK | MB_ICONERROR);
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        wsprintf(msgFalla, L"listen failed with error: %d", WSAGetLastError());
        MessageBox(NULL, msgFalla, L"Error en servidor", MB_OK | MB_ICONERROR);
        closesocket(ListenSocket);
        WSACleanup();

        return 1;
    }
    
    
    while (TRUE) {

        ClientSocket = accept(ListenSocket, NULL, NULL);        

        if (ClientSocket == INVALID_SOCKET) {
            wsprintf(msgFalla, L"acept failed with error: %d", WSAGetLastError());
            MessageBox(NULL, msgFalla, L"Error en servidor", MB_OK | MB_ICONERROR);
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }
        waitingConect = 0;

        // Recibir hasta que el par cierra la conexión

        iResult = recv(ClientSocket, szBuffer, sizeof(char) * 256, 0);
        aux = sscanf(szBuffer, "%s %s ", szIP, szNN);
        sprintf_s(szBuffer, "Ok");

        // Enviar el búfer al remitente

        iSendResult = send(ClientSocket, szBuffer, sizeof(char) * 256, 0);
        iResult = recv(ClientSocket, szBuffer, sizeof(char) * 256, 0);
        iSendResult = send(ClientSocket, szBuffer, sizeof(char) * 256, 0);

        MsgRecibir(hWnd, szBuffer);

        iResult = shutdown(ClientSocket, SD_SEND);
    }

    // Limpiar

    closesocket(ClientSocket);
    WSACleanup();

    return 1;
}

void MsgEnviar(char* direct, HWND hIP, HWND hWnd) {
    TCHAR tchDirIP[16];
    char szDirIP[16];
    int tam = 0;
    size_t i;

    GetWindowText(hIP, tchDirIP, 16);
    tam = GetWindowTextLength(hIP);
    wcstombs(szDirIP, tchDirIP, tam);
    szDirIP[tam] = '\0';

    long iLength;
    PSTR pstrBuffer;
    TCHAR* ptchBuffer;

    iLength = strlen(direct);
    if (NULL == (pstrBuffer = (PSTR)malloc(sizeof(char) * (iLength + 2))) ||
        NULL == (ptchBuffer = (TCHAR*)malloc(sizeof(TCHAR) * (iLength + 2))))
        MessageBox(NULL, L"Error al reservar memoria", L"Error", MB_OK | MB_ICONERROR);
    else
    {
        swprintf(ptchBuffer, 20, L"%hs", direct);
        wcstombs_s(&i, pstrBuffer, (iLength + 1), ptchBuffer, (iLength + 1));
        pstrBuffer[iLength + 1] = '\0';

        Cliente(szDirIP, pstrBuffer, hWnd);

        free(pstrBuffer);
        free(ptchBuffer);
    }
}

int Cliente(char* szDirIP, PSTR pstrMensaje, HWND hWnd) {
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL, * ptr = NULL, hints;
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;
    char szMsg[256];
    char localhost[] = "localhost";
    char chat[] = "chat";
    TCHAR msgFalla[256];

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        wsprintf(msgFalla, L"WSAStartup failed with error: %d\n", iResult);
        MessageBox(NULL, msgFalla, L"Error en cliente", MB_OK | MB_ICONERROR);
        return 1;
    }

    //Se limpia memoria para configurar la estructura del manejo de los sockets
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    //Se obtiene información
    iResult = getaddrinfo(szDirIP, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        wsprintf(msgFalla, L"getaddrinfo failed with error: %d\n", iResult);
        MessageBox(NULL, msgFalla, L"Error en cliente", MB_OK | MB_ICONERROR);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

        if (ConnectSocket == INVALID_SOCKET) {
            wsprintf(msgFalla, L"socket failed with error: %d\n", WSAGetLastError());
            MessageBox(NULL, msgFalla, L"Error en cliente", MB_OK | MB_ICONERROR);
            WSACleanup();
            return 1;
        }

        // Connect to server
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }

        break;
    }

    freeaddrinfo(result);

    //Validación para la llamada de conexión al socket válido
    if (ConnectSocket == INVALID_SOCKET) {
        MessageBox(NULL, L"Unable to connect to server!\n", L"Error en  cliente", MB_OK | MB_ICONERROR);
        sprintf_s(szMsg, "Error en la llamada a connect\nla dirección %s no es válida", szDirIP);
        
        WSACleanup();
        return 1;
    }
    waitingConect = 0;
    
    // Envío de mensajes al servidor
    sprintf_s(szMsg, "%s %s ", szMiIP, szUsuario);
    iResult = send(ConnectSocket, szMsg, sizeof(char) * 256, 0);
    iResult = recv(ConnectSocket, szMsg, sizeof(char) * 256, 0);

    strcpy_s(szMsg, pstrMensaje);

    iResult = send(ConnectSocket, szMsg, sizeof(char) * 256, 0);
    iResult = shutdown(ConnectSocket, SD_SEND);
    iResult = recv(ConnectSocket, szMsg, sizeof(char) * 256, 0);

    MsgRecibir(hWnd, szMsg);

    closesocket(ConnectSocket);
    WSACleanup();

    return 1;
}


char* NumChar(int num) {
    char NumChar[3] = { num + '0' };
    strcat(NumChar, " ");
    return NumChar;
}

void MsgRecibir(HWND hWnd, char* szMsg) {
    GetClientRect(hWnd, &rect1);
    char tipoS[2], direcS[2], tamS[2];
    int itipoS = 0, idirecS = 0, itamS = 0;
    sscanf(szMsg, "%s %s %s", tipoS, direcS, tamS);

    itipoS = atoi(tipoS); idirecS = atoi(direcS);
    itamS = atoi(tamS);

    if (itipoS == 0) {
        
        MoverSerpiente(serpiente, idirecS, rect1, itamS);
    }
    if (itipoS == 1) {
        
        MoverSerpiente(serpiente2, idirecS, rect1, itamS);
    }

    InvalidateRect(hWnd, NULL, TRUE);
 
}