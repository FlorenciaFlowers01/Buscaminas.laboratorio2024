#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define FILAS 8
#define COLUMNAS 8
#define MINAS 16
#define CASILLAS_SIN_MINAS 48
#define MAX_MARCADAS 16

//BUSCAMINAS.H (Cabezales)

char tablero[FILAS][COLUMNAS];
char tablero_oculto[FILAS][COLUMNAS];
int marcadas = 0;
int exploradas = 0; //Inicializamos las exploradas para que luego de la 1er exploracion se coloquen las minas
int primera_jugada = 1; // Variable para controlar si es la primera jugada
int game_over = 0; // Variable para indicar si el juego ha terminado

void inicializar_tablero();
void colocar_minas(int primera_fila, int primera_columna);
void mostrar_tablero();
void revelar_todas_las_bombas();
void explorar(int fila, int columna);
void marcar(int fila, int columna);
int es_casilla_explorada(int fila, int columna);
int es_casilla_adyacente_explorada(int fila, int columna);
void buscar(int fila, int columna) ;
void mostrar_tablero_completo();
void limpiar_buffer(); 

//Laboratorio parte 2

typedef struct{ //estructura que almacena una fecha
	int dia;
	int mes;
	int anio;
}date;


typedef struct{ //estructura que almacena una cadena de caracteres y su largo
	char letra[PALABRA];
	int largo;
}word;


typedef struct{ //almacena una cedula de identidad y su largo
	int cedula[CI];
	int largoCi;
}id;

typedef struct{ //estructura que almacena los jugadores registrados del juego
	word alias;
	word nombre;
	word apellido;
	id cedula;
	date fecha_nacimiento;
	char status; //A = Activo // I = Inactivo
}player;

//Prototipos de las funciones:

//Muestra las opciones del menú principal
int opciones_menu();

//Muestra las opciones del menú de gestión de usuarios
int opciones_usuarios();

//Muestra las opciones del menú de consultas
int opciones_consultas();

//Solicita al usuario el ingreso de una palabra de hasta 15 caracteres como máximo
word pedir_palabra();

//Solicita el año de nacimiento del jugador en formato(dd-mm-aaaa)
date pedir_fecha();

//Solicita una cedula de jugador de 8 digitos, sin puntos ni guiones
id pedir_cedula();

//Función que comprueba que se haya ingresado una fecha valida
int validar_fecha(date birthdate);

//Función que verifica si la cedula ingresada de un jugador es válida
int validar_ci(id document);

//busca si hay algun jugador con el mismo alias ingresado, falso retorna -1. De lo contrario retorna la pos del jugador
int buscar_jugador(player jugadores[], int pos_players, word alias_aux);

//retorna 0 si encuentra un alias igual al auxiliar o retorna 1 si no encuentra igualdad
int comparar_alias(word alias, word alias_aux);

//Solicita los datos de un jugador y los guarda en el arreglo de tipo player
void registrar_jugador(player players[], int *pos_players);

//Solicita el alias de un jugador y si existe, lo marca como inactivo
void baja_jugador(player players[], int pos_players);

//Solicita al usuario si desea guardar los cambios en la modificación de usuario
char save_confirm();

//Solicita al usuario que ingrese alias de un jugador, y si existe, permite modificar sus datos
void modificar_jugador(player players[], int pos_players);

//Imprime en pantalla una cedula recibida por párametro
void mostrar_cedula(id document);

//Imprime en pantalla una cadena de caracteres recibida por párametro
void mostrar_palabra(word cadena);

//Muestra una lista de los jugadores activos con sus respectivos datos
void player_list(player players[], int pos_players);

//Imprime en pantalla una fecha recibida por parámetro
void mostrar_fecha(date fecha);

//Muestra las victorias de cada jugador en una partida
void mostrar_resultado(int winsJ1, int winsJ2);


//MAIN.CPP(Ejecucion)

int main() {
	
	srand(time(NULL)); // Inicializa la semilla para n�meros aleatorios
	inicializar_tablero();
	printf("�Bienvenido al Buscaminas!\n");
	printf("Para jugar, elige una opci�n seguida de una fila (A-H) y una columna (A-H).\n");
	printf("Las opciones son: (E)xplorar, (M)arcar y (B)uscar.\n");
	mostrar_tablero();
	
	while (exploradas < CASILLAS_SIN_MINAS && !game_over) { // Mientras sea distinto a game_over el juego continua
		
		char opcion;
		char fila;
		char columna;
		printf("Elige una opci�n: ");
		scanf(" %c", &opcion);
		scanf(" %c", &fila);
		scanf(" %c", &columna);
		limpiar_buffer(); // Limpiar el b�fer de entrada despu�s de leer las entradas
		fila -= 'A'; // Convertir de letra A-H a rango 0-7
		columna -= 'A'; // Convertir de letra A-H a rango 0-7
		
		switch (opcion) {
		case 'E':
			explorar(fila, columna);
			break;
		case 'M':
			marcar(fila, columna);
			break;
		case 'B':
			buscar(fila, columna);
			break;
		default:
			printf("Opci�n inv�lida\n");
		}
		mostrar_tablero();
	}
	
	if (exploradas == CASILLAS_SIN_MINAS) {
		// El jugador gan�
		printf("�Pfff... fue pura suerte, la pr�xima no ser� tan f�cil!\n");
	} else if (game_over) {
		// El jugador perdi�
		printf("���� PERDISTE !!!! Suerte la pr�xima, se nota que la necesitas.\n");
	}
	return 0;
}

//BUSCAMINAS.CPP (Funciones)

//Inicializamos el tablero
void inicializar_tablero() {
	for (int i = 0; i < FILAS; i++) {
		for (int j = 0; j < COLUMNAS; j++) {
			tablero[i][j] = '?';
			tablero_oculto[i][j] = '?';
		}
	}
}

//Colocamos las minas
void colocar_minas(int primera_fila, int primera_columna) {
	int minas_colocadas = 0;
	while (minas_colocadas < MINAS) {
		int fila = rand() % FILAS;
		int columna = rand() % COLUMNAS;
		// Asegurarse de que la primera jugada no tenga una mina
		if ((fila != primera_fila || columna != primera_columna) && tablero_oculto[fila][columna]
			!= 'B') {
			tablero_oculto[fila][columna] = 'B';
			minas_colocadas++;
		}
	}
}

//Mostramos tablero
void mostrar_tablero() {
	printf("  A B C D E F G H\n");
	for (int i = 0; i < FILAS; i++) {
		printf("%c ", 'A' + i);
		for (int j = 0; j < COLUMNAS; j++) {
			printf("%c ", tablero[i][j]);
		}
		printf("\n");
	}
}

//Revelamos bombas
void revelar_todas_las_bombas() {
	for (int i = 0; i < FILAS; i++) {
		for (int j = 0; j < COLUMNAS; j++) {
			if (tablero_oculto[i][j] == 'B') {
				tablero[i][j] = 'B';
			}
		}
	}
}

//Opci�n explorar
void explorar(int fila, int columna) {
	
	//En caso de que la opcion salga de los casilleros
	if (fila < 0 || fila >= FILAS || columna < 0 || columna >= COLUMNAS) {
		printf("Formato de jugada err�neo\n");
		return;
	}
	
	//En caso de que la opcion ya este explorada
	if (tablero[fila][columna] != '?') {
		printf("Formato de jugada err�neo\n");
		return;
	}
	
	//Identifica la primer jugada para la colocacion de minas 
	if (primera_jugada) {
		colocar_minas(fila, columna);
		primera_jugada = 0;
	}
	
	//Revela la ubicacion de las bombas en caso de perder, condiciona la variable que indica que perdio el juego
	if (tablero_oculto[fila][columna] == 'B') {
		tablero[fila][columna] = 'B';
		revelar_todas_las_bombas();
		game_over = 1;
		return;
	}
	
	//En caso de encontrarse con un 0 revela casilleros adyacentes
	int minas_adyacentes = 0;
	for (int i = fila - 1; i <= fila + 1; i++) {
		for (int j = columna - 1; j <= columna + 1; j++) {
			if (i >= 0 && i < FILAS && j >= 0 && j < COLUMNAS && tablero_oculto[i][j] == 'B') {
				minas_adyacentes++;
			}
		}
	}
	tablero[fila][columna] = minas_adyacentes + '0';
	exploradas++;
	
	// Solo revelar las casillas adyacentes inmediatas seg�n la posici�n (centro, borde, esquina)
	if (minas_adyacentes == 0) {
		for (int i = fila - 1; i <= fila + 1; i++) {
			for (int j = columna - 1; j <= columna + 1; j++) {
				if (!(i == fila && j == columna)) {
					if (i >= 0 && i < FILAS && j >= 0 && j < COLUMNAS && tablero[i][j] == '?') {
						int minas_adyacentes_vecino = 0;
						for (int k = i - 1; k <= i + 1; k++) {
							for (int l = j - 1; l <= j + 1; l++) {
								if (k >= 0 && k < FILAS && l >= 0 && l < COLUMNAS && tablero_oculto[k][l] == 'B') {
									minas_adyacentes_vecino++;
								}
							}
						}
						tablero[i][j] = minas_adyacentes_vecino + '0';
						exploradas++;
					}
				}
			}
		}
	}
}

//Opcion marcar
void marcar(int fila, int columna) {
	if (fila < 0 || fila >= FILAS || columna < 0 || columna >= COLUMNAS) {
		printf("Formato de jugada err�neo\n");
		return;
	}
	
	//Marcar el casillero elegido suplantando ? por X
	if (tablero[fila][columna] == '?' || tablero[fila][columna] == 'X') {
		if (marcadas < MAX_MARCADAS || tablero[fila][columna] == 'X') {
			tablero[fila][columna] = (tablero[fila][columna] == '?') ? 'X' : '?'; // REVISARRR
			marcadas += (tablero[fila][columna] == 'X') ? 1 : -1;
		} else {
			printf("No se puede marcar m�s de %d casillas.\n", MAX_MARCADAS);
		}
	} else {
		printf("No se puede marcar esta casilla.\n");
	}
}

//"Desmonta" los casilleros explorados
int es_casilla_explorada(int fila, int columna) {
	if (fila < 0 || fila >= FILAS || columna < 0 || columna >= COLUMNAS) {
		return 0;
	}
	return tablero[fila][columna] != '?' && tablero[fila][columna] != 'X';
}


int es_casilla_adyacente_explorada(int fila, int columna) {
	for (int i = fila - 1; i <= fila + 1; i++) {
		for (int j = columna - 1; j <= columna + 1; j++) {
			if (es_casilla_explorada(i, j)) {
				return 1;
			}
		}
	}
	return 0;
}


//Opci�n buscar 
void buscar(int fila, int columna) {
	
	//En caso de que seleciona una opcion fuera del rango
	if (fila < 0 || fila >= FILAS || columna < 0 || columna >= COLUMNAS) {
		printf("Formato de jugada err�neo\n");
		return;
	}
	
	//En caso de que seleccione una casilla no explorada
	if (tablero[fila][columna] == '?' || tablero[fila][columna] == 'X') {
		printf("Jugada inv�lida: Debes buscar desde una casilla explorada\n");
		return;
	}
	
	//Identifica si existen casilleros marcados adyacentes
	int minas_marcadas_adyacentes = 0;
	for (int i = fila - 1; i <= fila + 1; i++) {
		for (int j = columna - 1; j <= columna + 1; j++) {
			if (i >= 0 && i < FILAS && j >= 0 && j < COLUMNAS && tablero[i][j] == 'X') {
				minas_marcadas_adyacentes++;
			}
		}
	}
	
	//En caso de que no existan minas adyacentes marcadas
	if (tablero[fila][columna] - '0' != minas_marcadas_adyacentes) {
		printf("Jugada inv�lida: La cantidad de minas marcadas adyacentes no coincide\n");
		return;
	}
	
	
	int revelado_ceros[FILAS][COLUMNAS] = {0}; // Matriz para marcar ceros revelados en la primera ronda
	
	for (int i = fila - 1; i <= fila + 1; i++) {
		for (int j = columna - 1; j <= columna + 1; j++) {
			if (i >= 0 && i < FILAS && j >= 0 && j < COLUMNAS && tablero[i][j] == '?') {
				if (tablero_oculto[i][j] == 'B') {
					revelar_todas_las_bombas();
					game_over = 1;
					return;
				} else {
					int minas_adyacentes = 0;
					for (int x = i - 1; x <= i + 1; x++) {
						for (int y = j - 1; y <= j + 1; y++) {
							if (x >= 0 && x < FILAS && y >= 0 && y < COLUMNAS && tablero_oculto[x][y] == 'B') {
								minas_adyacentes++;
							}
						}
					}
					tablero[i][j] = minas_adyacentes + '0';
					exploradas++;
					
					// Marcar ceros revelados
					if (minas_adyacentes == 0) {
						revelado_ceros[i][j] = 1;
					}
				}
			}
		}
	}
	
	// Segunda ronda para revelar adyacentes de los ceros revelados
	for (int i = 0; i < FILAS; i++) {
		for (int j = 0; j < COLUMNAS; j++) {
			if (revelado_ceros[i][j]) {
				for (int x = i - 1; x <= i + 1; x++) {
					for (int y = j - 1; y <= j + 1; y++) {
						if (x >= 0 && x < FILAS && y >= 0 && y < COLUMNAS && tablero[x][y] == '?') {
							if (tablero_oculto[x][y] == 'B') {
								revelar_todas_las_bombas();
								game_over = 1;
								return;
							} else {
								int minas_adyacentes = 0;
								for (int k = x - 1; k <= x + 1; k++) {
									for (int l = y - 1; l <= y + 1; l++) {
										if (k >= 0 && k < FILAS && l >= 0 && l < COLUMNAS && tablero_oculto[k][l] == 'B') {
											minas_adyacentes++;
										}
									}
								}
								tablero[x][y] = minas_adyacentes + '0';
								exploradas++;
							}
						}
					}
				}
			}
		}
	}
}

// Funci�n para limpiar el b�fer de entrada
void limpiar_buffer() {
	while(getchar()!='\n'){}
}

//Funcion provisoria para ver tablero "desnudo", llamar cuando se desee utilizar.
void mostrar_tablero_completo() {
	printf("  A B C D E F G H\n");
	for (int i = 0; i < FILAS; i++) {
		printf("%c ", 'A' + i);
		for (int j = 0; j < COLUMNAS; j++) {
			if (tablero_oculto[i][j] == 'B') {
				printf("B ");
			} else {
				int minas_adyacentes = 0;
				for (int x = i - 1; x <= i + 1; x++) {
					for (int y = j - 1; y <= j + 1; y++) {
						if (x >= 0 && x < FILAS && y >= 0 && y < COLUMNAS && tablero_oculto[x][y] == 'B') {
							minas_adyacentes++;
						}
					}
				}
				printf("%d ", minas_adyacentes);
			}
		}
		printf("\n");
	}
}

//Laboratorio parte 2 

int opciones_menu(){
	int opcion = 0;
	printf("╔════════════════╗\n");
	printf("║ Menú principal ║\n");
	printf("╚════════════════╝\n");
	printf("  1. Gestionar usuarios\n");
	printf("  2. Consultas\n");
	printf("  3. Jugar\n");
	printf("  4. Salir\n");
	do{
		printf("Ingresa una opción(1..4): ");
		scanf("%d", &opcion);
		if (opcion < 1 || opcion > 4){
			printf("La opción que has ingresado no es válida. Inténtalo otra vez.\n");
		}
	}
	while(opcion < 1 || opcion > 4);
	system("clear");
	return opcion;
}
	
	
	int opciones_usuarios(){
		int opcion = 0;
		system("clear");
		printf("╔═════════════════════════════╗\n");
		printf("║ Menú de Gestión de Usuarios ║\n");
		printf("╚═════════════════════════════╝\n");
		printf("  1. Alta de jugador\n");
		printf("  2. Baja de jugador\n");
		printf("  3. Modificación de jugador\n");
		printf("  0. Volver al menú principal\n");
		do{
			printf("Ingresa una opción(0..3): ");
			scanf("%d", &opcion);
			if (opcion < 0 || opcion > 3){
				printf("La opción que has ingresado no es válida. Inténtalo otra vez.\n");
			}
		}
		while(opcion < 0 || opcion > 3);
		system("clear");
		return opcion;
	}
		
int opciones_consultas(){
		int opcion = 0;
			system("clear");
			printf("╔═══════════════════╗\n");
			printf("║ Menú de consultas ║\n");
			printf("╚═══════════════════╝\n");
			printf("  1. Listado de jugadores\n");
			printf("  2. Listado de todas las partidas\n");
			printf("  3. Listado de partidas por jugador\n");
			printf("  4. Listado de partidas por fecha\n");
			printf("  0. Volver al menú principal\n");
			do{
				printf("Ingresa una opción(0..4): ");
				scanf("%d", &opcion);
				if (opcion < 0 || opcion > 4){
					printf("La opción que has ingresado no es válida. Inténtalo otra vez.\n");
				}
			}
			while(opcion < 0 || opcion > 4);
			system("clear");
			return opcion;
    }



word pedir_palabra(){
			word palabra;
			palabra.largo = 0;
			for(int i = 1; i <= PALABRA && (palabra.letra[i-1]=getchar())!='\n'; i++){
				palabra.largo += 1;
			}
			return palabra;
		}
			
date pedir_fecha(){
	date birthdate;
	int fecha_valida;
	do{
		scanf("%d-%d-%d", &birthdate.dia, &birthdate.mes, &birthdate.anio);
			fecha_valida = validar_fecha(birthdate);
	}
		while(fecha_valida != 1);
		return birthdate;
			}
				

id pedir_cedula(){
id document;
int ci_valida;

getchar();  //absorbe salto de linea de la selección de opción en menú
	do{
		document.largoCi = 0;
		for(int i = 1; i <= CI && (document.cedula[i-1]=getchar())!=10; i++){
			document.cedula[i-1] -= 48;
			document.largoCi += 1;
	}
		ci_valida = validar_ci(document); //si la cedula no es válida se pide ingresar una nueva
		}
	while(ci_valida != 1);
	return document;
	}


id pedir_cedula(){
	id document;
	int ci_valida;
	
	getchar();  //absorbe salto de linea de la selección de opción en menú
	do{
		document.largoCi = 0;
		for(int i = 1; i <= CI && (document.cedula[i-1]=getchar())!=10; i++){
			document.cedula[i-1] -= 48;
			document.largoCi += 1;
		}
		ci_valida = validar_ci(document); //si la cedula no es válida se pide ingresar una nueva
	}
	while(ci_valida != 1);
	return document;
}
	
	int validar_fecha(date birthdate){
		int fecha_valida = 1;
		if(birthdate.anio > 2021){
			fecha_valida = 3;
		}
		else if(birthdate.anio < 1900){
			fecha_valida = 4;
		}
		
		switch(birthdate.mes){
		case 2:
			if(birthdate.dia > 28){
				fecha_valida = 2;
			}
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			if(birthdate.dia > 30){
				fecha_valida = 2;
			}
			break;
			
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			if(birthdate.dia > 31){
				fecha_valida = 2;
			}
			break;
			
		default:
			fecha_valida = 0;
			
		}
		if(fecha_valida == 0){
			printf("El mes que ingresaste no es válido. Ingresa fecha nuevamente: ");
		}
		else if(fecha_valida == 2){
			printf("El dia que ingresaste no es válido. Ingresa fecha nuevamente: ");
		}
		else if(fecha_valida == 3){
			printf("¿Acaso vienes del futuro?. Ingresa fecha nuevamente: ");
		}
		else if(fecha_valida == 4){
			printf("No se acepta un año inferior al 1900. Ingresa fecha nuevamente: ");
		}
		else{
			printf("La fecha es válida.\n");
		}
		return fecha_valida;
	}


int validar_ci(id document){
			int multiplicador[] = {2, 9, 8, 7, 6, 3, 4};
			int resultado = 0;
			int suma = 0;
			int resto = 0;
			int dig_verif = 0;
			int ci_valida;
			
	if(document.largoCi == CI){
		for(int i = 0; i < CI-1; i++){
			resultado = multiplicador[i] * document.cedula[i];
			suma += resultado;
				}
		resto = suma % 10;
		dig_verif = (10 - resto) % 10;
		
		if(document.cedula[CI-1] == dig_verif){
		printf("CI válida.\n");
		ci_valida = 1; //cedula de identidad válida
				}
				else{
					printf("Digito verificador inválido. Intente nuevamente: ");
					getchar();
					ci_valida = 0; //cedula inválida
				}
			}
			else{
				printf("CI incompleta. Intente nuevamente: ");
				ci_valida = 0; //cedula inválida
			}
			return ci_valida;
		}


			int buscar_jugador(player jugadores[], int pos_players, word alias_aux){
				int i;
				for(i = 0; i < pos_players; i++){
					if(comparar_alias(jugadores[i].alias, alias_aux) == 0){
						return i;
					}
				}
				return -1; //si no se encuentra jugador con el mismo alias que "alias_aux"
			}
				
				int comparar_alias(word alias, word alias_aux){
					int i, c_iguales = 0;
					if(alias.largo != alias_aux.largo){ //compara los largos de ambos alias
						return 1; //si son diferentes retorna 1
					}
					else{
						for(i = 0; i < alias.largo; i++){ //recorre todas las posiciones de los alias
							if(alias.letra[i] == alias_aux.letra[i]){//compara cada caracter de ambos alias
								c_iguales++; //contador de caracteres iguales
							}
						}
						if(c_iguales == alias.largo){
							return 0;//si hay tantos caracteres iguales como el largo de los alias quiere decir que se encontró igualdad
						}
						else{
							return 1;
						}
					}
				}
					
void registrar_jugador(player players[], int *pos_players){
	word alias_aux;
	int existe_jugador;
	system("clear");
	printf("Jugador %d\n", *pos_players+1);
	printf("\nAlias: ");
	alias_aux = pedir_palabra();
    existe_jugador = buscar_jugador(players, *pos_players, alias_aux);
	if(existe_jugador != -1 && players[existe_jugador].status == 'I'){
		players[existe_jugador].status = 'A';
		printf("Jugador inactivo, se colocó como activo nuevamente.\n");
	}
	else if(existe_jugador != -1 && players[existe_jugador].status == 'A'){
		printf("El alias ingresado ya existe.\n");
	}
	else{//si no se encuentra un jugador con el alias ingresado previamente, se procede a ingresar los datos
	    players[*pos_players].alias = alias_aux;
		printf("\nNombre: ");
		players[*pos_players].nombre = pedir_palabra();
		printf("\nApellido: ");
		players[*pos_players].apellido = pedir_palabra();
		printf("\nFecha de nacimiento con el siguiente formato dd-mm-aaaa: ");
		players[*pos_players].fecha_nacimiento = pedir_fecha();
	    printf("\nCedula sin puntos ni guión: ");
		players[*pos_players].cedula = pedir_cedula();
//Al registrar jugador su estado se convierte en Activo							players[*pos_players].status = 'A';
		*pos_players += 1;
		getchar();
	}
pausa();
}


void mostrar_cedula(id document){
	for(int i = 0; i < CI; i++){
		printf("%d", document.cedula[i]);
	}
}
		
void mostrar_palabra(word cadena){
		for(int i = 0; i < cadena.largo; i++){
			printf("%c", cadena.letra[i]);
			}
		}
