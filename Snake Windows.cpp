// Snake Windows.cpp : Define el punto de entrada de la aplicación.
//

/*
* ERICK OSWALDO GALLEGOS PÉREZ. CARRERA: INGENIERÍA EN COMPUTACIÓN. SEMESTRE: OCTAVO. UNIVERSIDAD DEL ISTMO CAMPUS TEHUANTEPEC.
    PARA CORRER
    1. Se ejecuta el programa en máquinas diferentes dentro de una misma red. De forma: Snake Windows.exe IP_DE_LA_MÁQUINA USUARIO o simplemente editar el IP y nombre de usuario (char szMiIP[17] = "192.168.0.4";
    char szUsuario[32] = "lap";) en sus respectivas variables desde código.
    2. Una vez iniciado, se puede elegir jugar solo, que es el clásico juego. Pero también se puede jugar con otra máquina.
    3. Para ser el servidor (máquina anfitriona) se debe ingresar en el cuadro de texto la IP de la otra máquina. De igual manera en la máquina dos.
    En la anfotriona de le debe dar Jugar acompañado, lo cuál inicializará las dos serpientes en la máquina local, modificará banderas para la correcta visualización, seteará el timer para que esté a la
    espera de ser activdo por el cliente, levantará la hebra del servidor y se visualizará todo.
    4. En la otra máquina, se tendrá que dar a Conectarse para jugar, lo cual hará lo mismo que en la máquina anfitriona,
    Pequeña explicación: con excepción de que la hebra creada se llamará Cliente y que después de levantar la conexión y ésta sea correcta
        se envía el primer páquete de datos, el cual es el resultado de un tratamiento de int a char con la función numtochar, el resultado de este tratemiento del paquete de datos está compuesto por el tipo de máquina o usuario 
        (cliente 1 o servidor 0), las direcciones de las dos respientes y sus tamaños, además que se mandará el identificador de qué es
        lo que está enviando esta información, en este caso es un ESINICIOTIMER, lo cual es fundamental para iniciar correctamente el timer en el servidor.
    Con lo anterior estaría visualizandose y corriendo correctamente en las dos máquinas.

    EXPLICACIÓN
    Como tal, el servidor está a la escucha constante de lo que el cliente envíe, entonces al momento de realizarse la conexión, éste recibe los primeros datos, como el tipo de máquina o usuario (cliente o servidor)
    las direcciones de las serpientes, los tamaños, y por último el identificador de en qué parte se está enviando. 
    
    Estos datos pasan por MsgEnviar, en donde se realiza el paso para el Cliente. Esta función recibe lo que se envíe desde las teclas, el timer o la primera conexión
    Lo que es importante aquí es hacer la traducción y envío del contenido a enviar, junto con el ip y el hWnd hacie el cliente para realizar la pertinente conexión con la computadora deseada.

    Dentro del CLIENTE se manda la información para su posterior procesamiento en el servidor y por ende, la visualización gráfica de los snakes, de forma que:
    C1. Se inicializa el winsock
    C2. Se limpia la memoria para configurar la estructura del manejo de los sockets
    C3. Se obtiene información del puerto
    C4. Se prueba a conectar a una dirección hasta que una tenga éxito 
        C4.1. Se crea un socker para conectarse al servidor
        C4.2. Se conecta y valida el servidor
    C5. Validación para la llamada de conexión al socket válido
    C6. Primeramenre se envía el ip de nuestra máquina (en este caso cliente) y el nombre del usuario tal cual en la práctica original
    C7. Se obtiene el echo lanzado por el servidor para ver si se conectó, en este caso no se hace nada con esa devolución
    C8. Se coloca el envío de la información, es decir de los datos tales como las direcciones para las serpientes, el tipo de usuario en turno, el tamaño de cada una de las serpientes y el esUn respectivo para saber qué activar en el servidor, aquí faltaría que los datos a enviar contuviera la posición actual de la comida
    C9. Se hace el envío de los datos y se espera recibir contestación por parte del servidor
    C10. Una vez recibida la información proveniente del socket, de que es correcta la conexión final:
        C10.1Se debe actualizar la vista gráfica para que todos los cambios que procese el servidor, aunque aquí sea el cliente, se visualicen. Es decir que en todo momento se debe actualizar la vista, para mantener sincronizado el juego.
    C11. Se limpia y cierra el socket

    Para el SERVIDOR como se mencionó antes, está a la escucha constante de lo que el cliente envíe, entonces al momento de realizarse la conexión, éste recibe todos datos, como el tipo de máquina o usuario (cliente o servidor)
    las direcciones de las serpientes, los tamaños, y por último el identificador de en qué parte se está enviando. La lógica con la que funciona es la siguiente:
    S1. Se inicializa el winsock
    S2. Inicialización de las estructuras para la conexión
    S3. Resuelve la dirección y el puerto del servidor
    S4. Crea un SOCKET para la conexión al server
    S5. Configura el escucha TCP del socket
    S6. Escucha el socket
    S7. En un ciclo infinito (se mantiene en escucha siempre)
        S7.1. Acepta el socket de conexión del cliente
        S7.2. Se recibe (y guarda para alguna otra implementación, NO EN ESTA) el IP y el usuario del cliente proveniente del socket, simplemente para corroborar la conexión y se devuelve un OK
        S7.3. Se reciben los datos empaquetados provenientes del cliente en turno
        S7.4. Se procesan en la función MsgRecibir los datos empaquetados provenientes del cliente para que los snakes se visualicen y funcionen correctamente
            S7.4.1. Dentro de la función MsgRecibir es donde está todo lo funcional para que la visualización sea correcta en las dos máquinas, es cómo ese bonche de datos, se traduce y se toman acciones dependiendo de su contenido:           
                S7.4.1.1 Primeramente se obtienen los valores almacenados que se han recibido por el socket
                S7.4.1.2 Después se traducen a enteros, para su compatibilidad con la función de MoverSerpiente, en donde al fin se hace uso del hWnd para obtener el Rect del cliente y que esta función MoverSerpiente se llamará de acuerdo a lo recibido y con los datos pertinentes.
                S7.4.1.3 Se verifica quién envió exactamente el paquete de datos con la información contenida en esUn, para así poder tomar la decisión (en el switch) de si:
                    -fue por una tecla entonces checar qué máquina o usuario es el que movió por tecla y mover (MoverSerpiente) la serpiente en cuestión con los datos que se recibieron en el socket.
                    -si fue por el timer entonces actualizar las dos serpientes, moviendolas (MoverSerpiente) con los datos que se recibieron en el socket.
                    -o si fue por la primera conexión exitosa entre las máquinas, lo cual sucitará que se cambie el valor de la variable startTimer para dar inicio al movimiento del timer y visualizarlo en pantalla con los movimientos de las serpientes este paso es fundamental.
                En todos los casos se llama al InvalidateRect para visualizar todo los cambios en la pantalla del juego
        S7.5 Se devuelve el estado actual del juego (todos los datos)
    S8. Se limpia y cierra el socket
*/

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

//variables, en su mayoría banderas para entender el flujo y mostrar/ocultar cosas
static bool atemptConect = true;
static int waitingConect = 0;
static int tipoUs = 0;
static int soloUs = 3;
static int tams = 5;
static int tams2 = 5;
static int cuenta = 0;
static int startTimer = 0;
RECT rect1;
//Declaración de serpientes
static PEDACITOS* serpiente = NULL;
static PEDACITOS* otraserpiente = NULL;

//Funciones para cada serpiente y su comportamiento
PEDACITOS* NuevaSerpiente(int, int, int);
void DibujarSerpiente(HDC, const PEDACITOS *);
int MoverSerpiente(PEDACITOS *, int, RECT, int);
PEDACITOS* AjustarSerpiente(PEDACITOS *, int *, int, RECT);
int Colisionar(const PEDACITOS *, int);
int Comer(const PEDACITOS *, int);
//Funciones vitales para recibir y enviar los datos por el socket
void MsgRecibir(HWND, char*);
void MsgEnviar(char* hEscribir, HWND hIP, HWND hWnd);
char* numtochar(int);

//Declaración de funciones necesarias para la comunicación y muestreo
int                 Cliente(char* szDirIP, PSTR pstrMensaje, HWND hWnd);
//Declaración de la función Servidor que será lanzada como hilo
DWORD WINAPI        Servidor(LPVOID argumento);

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
            hpenRed = CreatePen(PS_SOLID, 1, RGB(217, 88, 88));
            hpenGreen = CreatePen(PS_SOLID, 1, RGB(111, 210, 139));
            hpenGray = CreatePen(PS_SOLID, 1, RGB(205, 205, 205));
            hbrRed = CreateSolidBrush(RGB(217, 88, 88));
            hbrGreen = CreateSolidBrush(RGB(111, 210, 139));
            hbrGray = CreateSolidBrush(RGB(205, 205, 205));
            
            //Se crean previamente las serpientes con el tamaño definido y la ubicación, simplemente estético.
            serpiente = NuevaSerpiente(tams, 1, 1);
            otraserpiente = NuevaSerpiente(tams2, 10, 10);

            //Se declara el cuadro de texto que almacenará la dirección IP
            ipTXT = CreateWindowEx(0, L"Edit", L"", ES_LEFT |
                WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP, 200, 130, 100, 25,
                hWnd,
                (HMENU)ID_IPTXT,
                hInst,
                NULL);
            //timer comentado porque no quiero que corran por sí solos
            //cuando son dos, cuando es uno, se declara más adelante uwu
            //SetTimer(hWnd, IDT_TIMER1, 500, NULL);
        }
    break;

    case WM_TIMER: {
        switch (wParam)
        {
            case IDT_TIMER1: 
            {
                //Timer que se activa cuando sse seleciona un solo jugador
                GetClientRect(hWnd, &rect);
                //if que checa si el juego seleccionado es en solitario. Activa los movimientos
                //y otras características de una sola serpiente, como la comida, el ajuste, etc.
                if (soloUs == 1)
                {
                    if (!MoverSerpiente(serpiente, serpiente[tams - 1].dir, rect, tams)) {
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
                        com.pos.x = rand() % rect.right / TAMSERP;
                        com.pos.y = rand() % rect.bottom / TAMSERP;
                        cuenta = 0;
                    }
                    if (Comer(serpiente, tams)) {
                        serpiente = AjustarSerpiente(serpiente, &tams, com.tipo, rect);
                        com.tipo = NADA;
                    }
                    //actualiza solo en el caso de ser juego en solitario
                    InvalidateRect(hWnd, NULL, TRUE);
                }
                
                //Verifica si se dio el inicio del timer lanzado por la conexión del cliente, este parámetro se ve
                //alterado al momento de que el servidor recibe los datos del cliente
                if (startTimer == 1)
                {
                    if (!MoverSerpiente(serpiente, serpiente[tams - 1].dir, rect, tams)) {
                        KillTimer(hWnd, IDT_TIMER1);
                        MessageBox(hWnd, L"Ya se murió", L"Fin del juego", MB_OK | MB_ICONINFORMATION);
                    }

                    if (!MoverSerpiente(otraserpiente, otraserpiente[tams2 - 1].dir, rect, tams2)) {
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
                        com.pos.x = rand() % rect.right / TAMSERP;
                        com.pos.y = rand() % rect.bottom / TAMSERP;
                        cuenta = 0;
                    }
                    if (Comer(serpiente, tams)) {
                        serpiente = AjustarSerpiente(serpiente, &tams, com.tipo, rect);
                        com.tipo = NADA;
                    }
                    if (Comer(otraserpiente, tams)) {
                        otraserpiente = AjustarSerpiente(otraserpiente, &tams2, com.tipo, rect);
                        com.tipo = NADA;
                    }

                    //Empaquetado o wrap de los datos que se pasarán por el socker para cumplir con la sincronización y
                    //comunicación entre las dos máquinas.
                    strcpy(datosEnviar, "");
                    strcat(datosEnviar, numtochar(3));
                    strcat(datosEnviar, numtochar(serpiente[tams-1].dir));
                    strcat(datosEnviar, numtochar(tams));
                    strcat(datosEnviar, numtochar(otraserpiente[tams2 - 1].dir));
                    strcat(datosEnviar, numtochar(tams2));
                    strcat(datosEnviar, numtochar(ESTIMER));
                    MsgEnviar(datosEnviar, ipTXT, hWnd);
                }
                
            }
            break;
        }
    }
        break;
    case WM_KEYDOWN: {
        GetClientRect(hWnd, &rect);
        switch (wParam)
        {
            //En este switch se toman acciones por cada tecla que se oprima
            //de manera que, para la primera serpiente se toman acciones dentro
            //de un if que checa si el tipo de usuario es servidor o también entra si
            //es solo un usuario, para mostrar el comportamiento al apretar las teclas
            //la diferencia más notable es que en la serpiente dos, para mostrar acciones 
            //solo se entra cuando el tipo de usuario es cliente.
            //Para las dos, y si es que la primera no es del tipo juego solo,
            //se obtiene el tipo de usuario que realiza el movimiento, también las direcciones del movimiento para cada serpiente,
            //el tamaño actual de la serpiente en cuestión y el tipo de lanzador (esUn) que envía el mensaje, en este caso es ESTECLA. Todo eso se envuenve en
            //la variable datosEnviar, que sí, obviemante se envía por MsgEnviar, junto con el 
            //IP que se ingrese en ipTXT y el hWnd para uso posterior de visualización de movimientos.

            case VK_UP: {
                
                if (tipoUs == 0 || soloUs == 1) {
                    if (soloUs != 1)
                    {
                        //Empaquetado o wrap de los datos que se pasarán por el socker para cumplir con la sincronización y
                        //comunicación entre las dos máquinas.
                        strcpy(datosEnviar, "");
                        strcat(datosEnviar, numtochar(0));
                        strcat(datosEnviar, numtochar(ARRIBA));
                        strcat(datosEnviar, numtochar(tams));
                        strcat(datosEnviar, numtochar(otraserpiente[tams2 - 1].dir));
                        strcat(datosEnviar, numtochar(tams2));
                        strcat(datosEnviar, numtochar(ESTECLA));
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
                    //Empaquetado o wrap de los datos que se pasarán por el socker para cumplir con la sincronización y
                    //comunicación entre las dos máquinas.
                    strcpy(datosEnviar, "");
                    strcat(datosEnviar, numtochar(1));
                    strcat(datosEnviar, numtochar(serpiente[tams - 1].dir));
                    strcat(datosEnviar, numtochar(tams));
                    strcat(datosEnviar, numtochar(ARRIBA));
                    strcat(datosEnviar, numtochar(tams2));
                    strcat(datosEnviar, numtochar(ESTECLA));
                    MsgEnviar(datosEnviar, ipTXT, hWnd);

                    if (!MoverSerpiente(otraserpiente, ARRIBA, rect, tams2)) {
                        KillTimer(hWnd, IDT_TIMER1);
                        MessageBox(hWnd, L"Ya se murió", L"Fin del juego", MB_OK | MB_ICONINFORMATION);
                    }

                    if (Comer(otraserpiente, tams)) {
                        otraserpiente = AjustarSerpiente(otraserpiente, &tams2, com.tipo, rect);
                        com.tipo = NADA;
                    }

                   

                    InvalidateRect(hWnd, NULL, TRUE);
                }

                
                break;
            }
            case VK_DOWN: {
                

                if (tipoUs == 0 || soloUs == 1) {
                    if (soloUs != 1)
                    {
                        //Empaquetado o wrap de los datos que se pasarán por el socker para cumplir con la sincronización y
                        //comunicación entre las dos máquinas.
                        strcpy(datosEnviar, "");
                        strcat(datosEnviar, numtochar(0));
                        strcat(datosEnviar, numtochar(ABAJO));
                        strcat(datosEnviar, numtochar(tams));
                        strcat(datosEnviar, numtochar(otraserpiente[tams2 - 1].dir));
                        strcat(datosEnviar, numtochar(tams2));
                        strcat(datosEnviar, numtochar(ESTECLA));
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
                    //Empaquetado o wrap de los datos que se pasarán por el socker para cumplir con la sincronización y
                    //comunicación entre las dos máquinas.
                    strcpy(datosEnviar, "");
                    strcat(datosEnviar, numtochar(1));
                    strcat(datosEnviar, numtochar(serpiente[tams - 1].dir));
                    strcat(datosEnviar, numtochar(tams));
                    strcat(datosEnviar, numtochar(ABAJO));
                    strcat(datosEnviar, numtochar(tams2));
                    strcat(datosEnviar, numtochar(ESTECLA));
                    MsgEnviar(datosEnviar, ipTXT, hWnd);

                    if (!MoverSerpiente(otraserpiente, ABAJO, rect, tams2)) {
                        KillTimer(hWnd, IDT_TIMER1);
                        MessageBox(hWnd, L"Ya se murió", L"Fin del juego", MB_OK | MB_ICONINFORMATION);
                    }

                    if (Comer(otraserpiente, tams)) {
                        otraserpiente = AjustarSerpiente(otraserpiente, &tams2, com.tipo, rect);
                        com.tipo = NADA;
                    }

                    

                    InvalidateRect(hWnd, NULL, TRUE);
                }
                break;
            }
            case VK_LEFT: {
                

                if (tipoUs == 0 || soloUs == 1) {
                    if (soloUs != 1)
                    {
                        //Empaquetado o wrap de los datos que se pasarán por el socker para cumplir con la sincronización y
                        //comunicación entre las dos máquinas.
                        strcpy(datosEnviar, "");
                        strcat(datosEnviar, numtochar(0));
                        strcat(datosEnviar, numtochar(IZQ));
                        strcat(datosEnviar, numtochar(tams));
                        strcat(datosEnviar, numtochar(otraserpiente[tams2 - 1].dir));
                        strcat(datosEnviar, numtochar(tams2));
                        strcat(datosEnviar, numtochar(ESTECLA));
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
                    //Empaquetado o wrap de los datos que se pasarán por el socker para cumplir con la sincronización y
                    //comunicación entre las dos máquinas.
                    strcpy(datosEnviar, "");
                    strcat(datosEnviar, numtochar(1));
                    strcat(datosEnviar, numtochar(serpiente[tams - 1].dir));
                    strcat(datosEnviar, numtochar(tams));
                    strcat(datosEnviar, numtochar(IZQ));
                    strcat(datosEnviar, numtochar(tams2));
                    strcat(datosEnviar, numtochar(ESTECLA));
                    MsgEnviar(datosEnviar, ipTXT, hWnd);

                    if (!MoverSerpiente(otraserpiente, IZQ, rect, tams2)) {
                        KillTimer(hWnd, IDT_TIMER1);
                        MessageBox(hWnd, L"Ya se murió", L"Fin del juego", MB_OK | MB_ICONINFORMATION);
                    }

                    if (Comer(otraserpiente, tams)) {
                        otraserpiente = AjustarSerpiente(otraserpiente, &tams2, com.tipo, rect);
                        com.tipo = NADA;
                    }

                   

                    InvalidateRect(hWnd, NULL, TRUE);
                }

                break;
            }
            case VK_RIGHT: {
                

                if (tipoUs == 0 || soloUs == 1) {
                    if (soloUs != 1)
                    {
                        //Empaquetado o wrap de los datos que se pasarán por el socker para cumplir con la sincronización y
                        //comunicación entre las dos máquinas.
                        strcpy(datosEnviar, "");
                        strcat(datosEnviar, numtochar(0));
                        strcat(datosEnviar, numtochar(DER));
                        strcat(datosEnviar, numtochar(tams));
                        strcat(datosEnviar, numtochar(otraserpiente[tams2 - 1].dir));
                        strcat(datosEnviar, numtochar(tams2));
                        strcat(datosEnviar, numtochar(ESTECLA));
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
                    //Empaquetado o wrap de los datos que se pasarán por el socker para cumplir con la sincronización y
                    //comunicación entre las dos máquinas.
                    strcpy(datosEnviar, "");
                    strcat(datosEnviar, numtochar(1));
                    strcat(datosEnviar, numtochar(serpiente[tams - 1].dir));
                    strcat(datosEnviar, numtochar(tams));
                    strcat(datosEnviar, numtochar(DER));
                    strcat(datosEnviar, numtochar(tams2));
                    strcat(datosEnviar, numtochar(ESTECLA));
                    MsgEnviar(datosEnviar, ipTXT, hWnd);
                    if (!MoverSerpiente(otraserpiente, DER, rect, tams2)) {
                        KillTimer(hWnd, IDT_TIMER1);
                        MessageBox(hWnd, L"Ya se murió", L"Fin del juego", MB_OK | MB_ICONINFORMATION);
                    }

                    if (Comer(otraserpiente, tams)) {
                        otraserpiente = AjustarSerpiente(otraserpiente, &tams2, com.tipo, rect);
                        com.tipo = NADA;
                    }

                    

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
                ShowWindow(ipTXT, false);
                atemptConect = false;
                if (serpiente != NULL) {
                    KillTimer(hWnd, IDT_TIMER1);
                    free(serpiente);
                    tams = 5;
                    cuenta = 0;
                    tipoUs = 0;
                    serpiente = NuevaSerpiente(tams, 1, 1);
                    //otraserpiente = NuevaSerpiente(tams2, 10, 10);
                    SetTimer(hWnd, IDT_TIMER1, 500, NULL);
                    InvalidateRect(hWnd, NULL, TRUE);
                }
                else if (serpiente == NULL){
                    KillTimer(hWnd, IDT_TIMER1);
                    tams = 5;
                    cuenta = 0;
                    tipoUs = 0;
                    serpiente = NuevaSerpiente(tams, 1, 1);
                    //otraserpiente = NuevaSerpiente(tams2, 10, 10);
                    SetTimer(hWnd, IDT_TIMER1, 500, NULL);
                    InvalidateRect(hWnd, NULL, TRUE);
                }

                atemptConect = false;
                waitingConect = 0;
                break;
            }
            case IDM_ACOMPA: {
                
                //Se cierran los hilo
                CloseHandle(threadServer);
                CloseHandle(threadCliente);

                //Se libera la memoria que ocupan las serpientes
                free(serpiente);
                free(otraserpiente);

                //Se deja de visualizar el ingreso del IP
                atemptConect = false;
                ShowWindow(ipTXT, false);

                //Se mata el timer
                KillTimer(hWnd, IDT_TIMER1);

                //Se declaran nuevos valores para las banderas, surtiendo efecto en la visualización de elementos gráficos
                tipoUs = 0;
                soloUs = 0;
                waitingConect = 1;
                com = { {0,0}, NADA };
                cuenta = 0;
                //Se crean previamente las serpientes con el tamaño definido y la ubicación
                serpiente = NuevaSerpiente(tams, 1, 1);
                otraserpiente = NuevaSerpiente(tams2, 10, 10);
                SetTimer(hWnd, IDT_TIMER1, 500, NULL);
                SetFocus(hWnd);
                // Se crea el hilo pasandole el tipo de seguridad, el tamaño de la pila, la función que 
                //se levantará en el hilo, el argumento de la función y la devolución del 
                //identificador del hilo
                //Esto es fundamental para que la creación e interación con la otra máquina se la correcta
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
                //Se cierran los hilo
                CloseHandle(threadServer);
                CloseHandle(threadCliente);

                //Se libera la memoria que ocupan las serpientes
                free(serpiente);
                free(otraserpiente);
                //Banderas que se alteran de acuerdo al flujo de la aplicación
                atemptConect = false;
                waitingConect = 2;
                tipoUs = 1;
                soloUs = 0;
                //Se oculta la toma del IP
                ShowWindow(ipTXT, false);

                com = { {0,0}, NADA };
                cuenta = 0;
                //Se crean previamente las serpientes con el tamaño definido y la ubicación
                serpiente = NuevaSerpiente(tams, 1, 1);
                otraserpiente = NuevaSerpiente(tams2, 10, 10);
                SetTimer(hWnd, IDT_TIMER1, 500, NULL);
                SetFocus(hWnd);
                //serpiente = NuevaSerpiente(tams, 1, 1);
                //otraserpiente = NuevaSerpiente(tams2, 10, 10);
                //MessageBox(NULL, (LPCWSTR)ipTXT, L"XD", MB_OK | MB_ICONINFORMATION);
                //En este caso se declara una hebra para el Cliente, pero fungiendo como Servidor
                //Esto se hace con el fin de que, en todo momento, las dos máquinas puedan enviar y recibir mensajes
                //fungiendo como una conexión peer to peer o punto a punto, pero con la idea en esencia
                //de ser cliente servidor.
                threadCliente = CreateThread(NULL, 0, Servidor,
                    (LPVOID)hWnd, 0, &threadClienteID);
                //Validamos la creación del hilo para le servidor
                if (threadCliente == NULL) {
                    MessageBox(hWnd, L"Error al crear el hilo para el servidor", L"Error", MB_OK | MB_ICONERROR);
                }

                //Justo despúés de crear el hilo anterior se hace lo siguiente para notificar el inicio del timer con los
                //respectivos valores para cada serpiente.
                //Empaquetado o haciendo wrap de los datos que se pasarán por el socket para cumplir con la sincronización y
                //comunicación entre las dos máquinas.
                strcpy(datosEnviar, "");
                strcat(datosEnviar, numtochar(3));
                strcat(datosEnviar, numtochar(serpiente[tams - 1].dir));
                strcat(datosEnviar, numtochar(tams));
                strcat(datosEnviar, numtochar(otraserpiente[tams2 - 1].dir));
                strcat(datosEnviar, numtochar(tams2));
                strcat(datosEnviar, numtochar(ESINICIOTIMER));
                MsgEnviar(datosEnviar, ipTXT, hWnd);
                
               // hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
                //LoadLibrary(L"riched20.dll");
                            
                //Para visualizar ya los snakes
                InvalidateRect(hWnd, NULL, TRUE);
              
                
                break;
            }
            case IDM_ABOUT:
                MessageBox(NULL, L"Alumno: Erick Oswaldo Gallegos Pérez \n Semestre: Octavo. \n Carrera: Ingeniería en computación. \n Materia: Sistemas de Cómputo Paralelo y Distribuido.", L"Acerca de", MB_OK | MB_ICONINFORMATION);
                //DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_SALIR:


                //Se libera la memoria que ocupan las serpientes
                free(serpiente);
                    
                   
                free(otraserpiente);
                    
                //Se cierran los hilo
                CloseHandle(threadServer);
                CloseHandle(threadCliente);

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
            //Entra a esta sección cuando se juega en "multijugador"
            //Además se pintan las serpientes con colores diferentes
            hpenTemp = (HPEN)SelectObject(hdc, hpenRed);
            hbrTemp = (HBRUSH)SelectObject(hdc, hbrRed);
            DibujarSerpiente(hdc, serpiente);
            SelectObject(hdc, hpenTemp);
            SelectObject(hdc, hbrTemp);
            hpenTemp = (HPEN)SelectObject(hdc, hpenGreen);
            hbrTemp = (HBRUSH)SelectObject(hdc, hbrGreen);
            DibujarSerpiente(hdc, otraserpiente);
            SelectObject(hdc, hpenTemp);
            SelectObject(hdc, hbrTemp);
        }
        if (soloUs == 1)
        {
            //Entra a esta sección cuando solo se busca jugar en solitario
            //Por ende, solo pinta una serpiente
            hpenTemp = (HPEN)SelectObject(hdc, hpenRed);
            hbrTemp = (HBRUSH)SelectObject(hdc, hbrRed);
            DibujarSerpiente(hdc, serpiente);
            SelectObject(hdc, hpenTemp);
            SelectObject(hdc, hbrTemp);
        }
        
        //Para la comidaaaaaa
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

        //Se evalúan los estados de las respectivas banderas
        //para poder visualizar el elemento en cuestión en el 
        //momento indicado
        if (waitingConect == 1) {
            TextOut(hdc, 200, 80, L"CREANDO", sizeof("CREANDO"));
            TextOut(hdc, 200, 100, L"En espera!!!", sizeof("En espera!!!"));
        }
        if (waitingConect == 2) {
            TextOut(hdc, 200, 100, L"CONECTADO!!!", sizeof("CONECTADO!!!"));
        }

        if (atemptConect) {
            TextOut(hdc, 200, 80, L"CONECTARSE", sizeof("CONECTARSE"));
            TextOut(hdc, 200, 100, L"Introduce el IP:", sizeof("Introduce el IP:"));
        }
        
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        //Se cierran los manejadores de las hebras
        CloseHandle(threadServer);
        CloseHandle(threadCliente);
        //Se libren memorias ocupadas por las serpientes
        free(serpiente);
        free(otraserpiente);
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
    static HPEN hpenGray;
    static HBRUSH hbrGray;


    hpenGray = CreatePen(PS_SOLID, 1, RGB(205, 205, 205));

    hbrGray = CreateSolidBrush(RGB(205, 205, 205));

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
    //Para el manejo del socket
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

    //Variables para guardar las cadenas de mensajes para entrada y salida
    char szBuffer[256], szIP[16], szNN[32];


    // Se inicializa el winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        wsprintf(msgFalla, L"WSAStartup failed with error: %d\n", iResult);
        MessageBox(NULL, msgFalla, L"Error em servidore", MB_OK | MB_ICONERROR);

        return 1;
    }

    //Inicialización de las estructuras para la conexión
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resuelve la dirección y el puerto del servidor
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        wsprintf(msgFalla, L"getaddrinfo failed with error: %d", iResult);
        MessageBox(NULL, msgFalla, L"Error en servidor", MB_OK | MB_ICONERROR);
        WSACleanup();

        return 1;
    }

    // Crea un SOCKET para la conexión al server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        wsprintf(msgFalla, L"socket failed with error: %d", WSAGetLastError());
        MessageBox(NULL, msgFalla, L"Error en servidor", MB_OK | MB_ICONERROR);
        freeaddrinfo(result);
        WSACleanup();

        return 1;
    }
    
    //Configura el escucha TCP del socket
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

    //Escucha el socket
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        wsprintf(msgFalla, L"listen failed with error: %d", WSAGetLastError());
        MessageBox(NULL, msgFalla, L"Error en servidor", MB_OK | MB_ICONERROR);
        closesocket(ListenSocket);
        WSACleanup();

        return 1;
    }
    
    
    while (TRUE) {

        //Acepta el socket de conexión del cliente
        ClientSocket = accept(ListenSocket, NULL, NULL);        

        if (ClientSocket == INVALID_SOCKET) {
            wsprintf(msgFalla, L"acept failed with error: %d", WSAGetLastError());
            MessageBox(NULL, msgFalla, L"Error en servidor", MB_OK | MB_ICONERROR);
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }
        waitingConect = 0;

        
        //Se recibe (y guarda para alguna otra implementación) el IP y el usuario del cliente proveniente del socket, simplemente para corroborar la conexión y se devuelve un OK
        iResult = recv(ClientSocket, szBuffer, sizeof(char) * 256, 0);
        aux = sscanf(szBuffer, "%s %s ", szIP, szNN);
        sprintf_s(szBuffer, "Ok");

        iSendResult = send(ClientSocket, szBuffer, sizeof(char) * 256, 0);

        //Se reciben los datos empaquetados provenientes del cliente

        iResult = recv(ClientSocket, szBuffer, sizeof(char) * 256, 0);

        //Se procesan en la función MsgRecibir los datos empaquetados provenientes del cliente para que los snakes se visualicen
        MsgRecibir(hWnd, szBuffer);

        //se devuelve el estado actual del juego (todos los datos)
        iSendResult = send(ClientSocket, szBuffer, sizeof(char) * 256, 0);

        

        iResult = shutdown(ClientSocket, SD_SEND);
    }

    // Limpiar el socket

    closesocket(ClientSocket);
    WSACleanup();

    return 1;
}

void MsgEnviar(char* direct, HWND hIP, HWND hWnd) {
    //Esta función recibe lo que se envíe desde las teclas, el timer o la primera conexión
    //Lo que es importante aquí es hacer la traducción y envío del contenido a enviar, 
    //junto con el ip y el hWnd hacie el cliente para realizar la pertinente
    //conexión con la computadora deseada.
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

        //Aquí es donde se manda a llamar Cliente y recibe los datos antes mencionados
        Cliente(szDirIP, pstrBuffer, hWnd);

        free(pstrBuffer);
        free(ptchBuffer);
    }
}

int Cliente(char* szDirIP, PSTR pstrMensaje, HWND hWnd) {
    //Esta sección es importante para la conexión en el socket para las máquinas, en el caso particular cuando es
    //cliente se manda la información para su posterior procesamiento en el servidor y por ende, la visualización
    //gráfica de los snakes
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL, * ptr = NULL, hints;
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;
    char szMsg[256];
    char localhost[] = "localhost";
    char chat[] = "chat";
    TCHAR msgFalla[256];

    // SE inicializa el Winsock
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

    //Se obtiene información del puerto
    iResult = getaddrinfo(szDirIP, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        wsprintf(msgFalla, L"getaddrinfo failed with error: %d\n", iResult);
        MessageBox(NULL, msgFalla, L"Error en cliente", MB_OK | MB_ICONERROR);
        WSACleanup();
        return 1;
    }

    // Se prueba a conectar a una dirección hasta que una tenga éxito  
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Se crea un socker para conectarse al servidor
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

        if (ConnectSocket == INVALID_SOCKET) {
            wsprintf(msgFalla, L"socket failed with error: %d\n", WSAGetLastError());
            MessageBox(NULL, msgFalla, L"Error en cliente", MB_OK | MB_ICONERROR);
            WSACleanup();
            return 1;
        }

        // Se conecta y valida el servidor
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
    //Primeramenre se envía el ip de nuestra máquina y el nombre del usuario
    //tal cual en la práctica original
    sprintf_s(szMsg, "%s %s ", szMiIP, szUsuario);
    iResult = send(ConnectSocket, szMsg, sizeof(char) * 256, 0);
    iResult = recv(ConnectSocket, szMsg, sizeof(char) * 256, 0);
    //Se obtiene el echo lanzado por el servidor para ver si se conectó y se lanza una MessageBox
    /*
    if (strcmp(szMsg, "Ok") == 0)
    {      
        MessageBox(hWnd, L"Se ha conectado a una sesión. lml", L"CONECTADO", MB_OK | MB_ICONINFORMATION);
    }
    */
    //se coloca el envío de la información, es decir de los datos
    //tales como las direcciones para las serpientes, el tipo de usuario en turno, el tamaño de cada una de las serpientes y
    //el esUn respectivo para saber qué activar en el servidor
    //aquí faltaría que los datos a enviar contuviera la posición actual de la comida
    
    strcpy_s(szMsg, pstrMensaje);

    //Se hace el envío de los datos y se espera recibir contestación por parte del servidor
    iResult = send(ConnectSocket, szMsg, sizeof(char) * 256, 0);
    iResult = shutdown(ConnectSocket, SD_SEND);
    iResult = recv(ConnectSocket, szMsg, sizeof(char) * 256, 0);


    //Una vez recibida la información proveniente del socket, de que es correcta la conexión final. 
    //Se debe actualizar la vista gráfica para que todos los cambios que procese el servidor, aunque aquí sea el cliente, se visualicen.
    //Es decir que en todo momento se debe actualizar la vista, para mantener sincronizado el juego.
    InvalidateRect(hWnd, NULL, TRUE);
    //MsgRecibir(hWnd, szMsg);

    closesocket(ConnectSocket);
    WSACleanup();

    return 1;
}


char* numtochar(int num) {
    //lo que hace esta función es pasar de número a char para el empaquetado de la información
    //y que sea legible en el paso del socket
    char val[3] = { num + '0' };
    strcat(val, " ");
    return val;
}

void MsgRecibir(HWND hWnd, char* szMsg) {
    //Aquí es donde sucede la magia por así llamarlo.
    //Primeramente se obtienen los valores almacenados que se han recibido por el socket
    //después se traducen a enteros, para su compatibilidad con la función de
    //MoverSerpiente, en donde al fin se hace uso del hWnd para obtener el Rect del cliente
    //Y poder así cumplir con los parámetros.
    //Como he mencionado antes, en el paquete de datos se presenta el tipo de usuario
    //el cual es, al menos, la identificación de servidor o cliente, para poder
    //dibujar la serpiente en turno.
    //Además de loss tamaños de las dos serpientes, las direcciones de las doss serpiente y el esUn, que viene siendo, quién envió exactamente el mensaje
    //para así poder tomass la decisión (en el switch) de si fue por una tecla, si fue por el timer o si fue por la primera conexión exitosa entre las máquinas.
    GetClientRect(hWnd, &rect1);
    char tipoS[2], direcS[2], tamS[2], direcOS[2], tamOS[2], esUn[2];
    int itipoS = 0, idirecS = 0, itamS = 0, idirecOS = 0, itamOS = 0, iesUn = 0;
    sscanf(szMsg, "%s %s %s %s %s %s", tipoS, direcS, tamS, direcOS, tamOS, esUn);

    itipoS = atoi(tipoS); idirecS = atoi(direcS);
    itamS = atoi(tamS); idirecOS = atoi(direcOS);
    itamOS = atoi(tamOS); iesUn = atoi(esUn);

    switch (iesUn)
    {
    case ESTECLA:
        if (itipoS == 0) {

            MoverSerpiente(serpiente, idirecS, rect1, itamS);
            //Para visualizar todo los cambios en la pantalla del juego
            InvalidateRect(hWnd, NULL, TRUE);
        }
        if (itipoS == 1) {

            MoverSerpiente(otraserpiente, idirecOS, rect1, itamOS);
            //Para visualizar todo los cambios en la pantalla del juego
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;
    case ESTIMER:
        
        MoverSerpiente(serpiente, idirecS, rect1, itamS);
        MoverSerpiente(otraserpiente, idirecOS, rect1, itamOS);
        //Para visualizar todo los cambios en la pantalla del juego
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    case ESINICIOTIMER:
        startTimer = 1;

        //Para visualizar todo los cambios en la pantalla del juego
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    default:
        break;
    }

    
 
}