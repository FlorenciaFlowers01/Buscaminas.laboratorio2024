#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FILAS 8
#define COLUMNAS 8
#define MINAS 16
#define CASILLAS_SIN_MINAS 48
#define MAX_MARCADAS 16
#define CI_LEN 8

int validar_ci(char ci[]);
int validar_fecha(char fecha[]);
int alias_unico(char alias[]);
void alta_jugador();
void modificar_jugador();
void listar_jugadores();
void listar_partidas();
void inicializar_tablero();
void colocar_minas(int primera_fila, int primera_columna);
void mostrar_tablero();
void revelar_todas_las_bombas();
void explorar(int fila, int columna);
void marcar(int fila, int columna);
int es_casilla_explorada(int fila, int columna);
int es_casilla_adyacente_explorada(int fila, int columna);
void buscar(int fila, int columna);
void rendirse();
void limpiar_buffer();
void jugar();
void reiniciar_estado_del_juego();  // Función para reiniciar el estado del juego
void volver_al_menu_principal();

typedef struct {
	char ci[CI_LEN + 1];
	char fecha_nacimiento[11];
	char nombre[50];
	char apellido[50];
	char alias[50];
	int activo;
	int partidas_ganadas;
} Jugador;

typedef struct {
	char fecha[20];
	char alias[50];
	char resultado[20];
} Partida;

int main() {
	int opcion;
	do {
		printf("\nMenú principal:\n");
		printf("\n1. Gestionar usuario\n");
		printf("2. Consultas\n");
		printf("3. Jugar\n");
		printf("4. Salir\n");
		printf("\nSeleccione una opción: ");
		scanf("%d", &opcion);
		limpiar_buffer();
		switch (opcion) {
		case 1:
			printf("\n1. Alta de jugador\n");
			printf("2. Modificación de jugador\n");
			printf("3. Volver al menú principal\n");
			int sub_opcion;
			printf("\nSeleccione una opción: ");
			scanf("%d", &sub_opcion);
			limpiar_buffer();
			if (sub_opcion == 1) {
				alta_jugador();
			} else if (sub_opcion == 2) {
				modificar_jugador();
			}else if (sub_opcion == 3){
				volver_al_menu_principal();
			}else {
				printf("\nOpción inválida\n");
			}
			break;
		case 2:
			printf("\n1. Listado de jugadores\n");
			printf("2. Listado de todas las partidas\n");
			printf("3. Volver al menú principal\n");
			printf("\nSeleccione una opción: ");
			scanf("%d", &sub_opcion);
			limpiar_buffer();
			if (sub_opcion == 1) {
				listar_jugadores();
			} else if (sub_opcion == 2) {
				listar_partidas();
			}else if(sub_opcion == 3){
				volver_al_menu_principal();
			}else {
				printf("\nOpción inválida\n");
			}
			break;
		case 3:
			jugar();
			break;
		case 4:
			printf("\nSaliendo del programa.\n");
			break;
		default:
			printf("\nOpción inválida\n");
		}
	} while (opcion != 4);
	return 0;
}

Jugador jugadores[100];
int num_jugadores = 0;
Partida partidas[100];
int num_partidas = 0;
char tablero[FILAS][COLUMNAS];
char tablero_oculto[FILAS][COLUMNAS];
int marcadas = 0;
int exploradas = 0;
int primera_jugada = 1;
int game_over = 0;

int validar_ci(char ci[]) {
	// Verificar la longitud de la cédula
	if (strlen(ci) != CI_LEN) {
		printf("CI incompleta. Intente nuevamente: ");
		return 0;
	}
	
	// Verificar que todos los caracteres sean dígitos
	for (int i = 0; i < CI_LEN; i++) {
		if (ci[i] < '0' || ci[i] > '9') {
			printf("CI inválida. Intente nuevamente: ");
			return 0;
		}
	}
	
	// Calcular el dígito verificador
	int multiplicador[] = {2, 9, 8, 7, 6, 3, 4};
	int suma = 0;
	for (int i = 0; i < CI_LEN - 1; i++) {
		suma += (ci[i] - '0') * multiplicador[i];
	}
	int resto = suma % 10;
	int dig_verif = (10 - resto) % 10;
	
	// Comparar el dígito verificador calculado con el ingresado
	if (ci[CI_LEN - 1] - '0' == dig_verif) {
		printf("CI válida.\n");
		return 1;
	} else {
		printf("Dígito verificador inválido. Intente nuevamente: ");
		return 0;
	}

int validar_fecha(char fecha[]) {
	int d, m, a, len = 0;
	for (; fecha[len] != '\0'; len++);
	
	// Comprobar que la cadena tenga el formato correcto (dd-mm-yyyy)
	if (len!= 10 || fecha[2]!= '-' || fecha[5]!= '-') {
		return 0;
	}
	
	// Extraer los valores de día, mes y año
	d = 10 * (fecha[0] - '0') + (fecha[1] - '0');
	m = 10 * (fecha[3] - '0') + (fecha[4] - '0');
	a = 1000 * (fecha[6] - '0') + 100 * (fecha[7] - '0') + 10 * (fecha[8] - '0') + (fecha[9] - '0');
	
	// Verificar que los valores estén dentro de rangos válidos
	if (m < 1 || m > 12) {
		return 0;  // Mes inválido
	}
	
	if (m == 2) {  // Febrero
		if (a % 4 == 0 && (a % 100!= 0 || a % 400 == 0)) {  // Año bisiesto
			if (d < 1 || d > 29) {
				return 0;  // Día inválido
			}
		} else {
			if (d < 1 || d > 28) {
				return 0;  // Día inválido
			}
		}
	} else if (m == 4 || m == 6 || m == 9 || m == 11) {  // Meses con 30 días
		if (d < 1 || d > 30) {
			return 0;  // Día inválido
		}
	} else {  // Meses con 31 días
		if (d < 1 || d > 31) {
			return 0;  // Día inválido
		}
	}
	
	return 1;  // Fecha válida
}

int alias_unico(char alias[]) {
	for (int i = 0; i < num_jugadores; i++) {
		int j = 0;
		while (alias[j]!= '\0' && jugadores[i].alias[j]!= '\0') {
			if (alias[j]!= jugadores[i].alias[j]) {
				break;
			}
			j++;
		}
		if (alias[j] == '\0' && jugadores[i].alias[j] == '\0') {
			return 0;
		}
	}
	return 1;
}

void alta_jugador() {
	Jugador nuevo;
	printf("\nIngrese CI (8 dígitos): ");
	scanf("%8s", nuevo.ci);
	while (!validar_ci(nuevo.ci)) {
		scanf("%8s", nuevo.ci);
	}
	printf("\nIngrese fecha de nacimiento (DD-MM-YYYY): ");
	scanf("%10s", nuevo.fecha_nacimiento);
	while (!validar_fecha(nuevo.fecha_nacimiento)) {
		printf("\nFecha inválida. Ingrese fecha de nacimiento (DD-MM-YYYY): ");
		scanf("%10s", nuevo.fecha_nacimiento);
	}
	printf("\nIngrese nombre: ");
	scanf("%49s", nuevo.nombre);
	printf("\nIngrese apellido: ");
	scanf("%49s", nuevo.apellido);
	printf("\nIngrese alias: ");
	scanf("%49s", nuevo.alias);
	while (!alias_unico(nuevo.alias)) {
		printf("\nAlias ya en uso. Ingrese otro alias: ");
		scanf("%49s", nuevo.alias);
	}
	printf("\n¿Está activo? (s/n): ");
	char activo;
	scanf(" %c", &activo);
	nuevo.activo = (activo == 's'|| activo == 'S');
	nuevo.partidas_ganadas = 0;
	jugadores[num_jugadores++] = nuevo;
	printf("\nJugador %s registrado exitosamente.\n", nuevo.alias);
}

void modificar_jugador() {
	char alias[50];
	printf("\nIngrese el alias del jugador a modificar: ");
	scanf("%49s", alias);
	for (int i = 0; i < num_jugadores; i++) {
		if (strcmp(jugadores[i].alias, alias) == 0) {
			Jugador *jugador = &jugadores[i];
			printf("\nModificando datos de %s\n", alias);
			printf("\nCI (%s): ", jugador->ci);
			scanf("%8s", jugador->ci);
			while (!validar_ci(jugador->ci)) {
				scanf("%8s", jugador->ci);
			}
			printf("\nFecha de nacimiento (%s): ", jugador->fecha_nacimiento);
			scanf("%10s", jugador->fecha_nacimiento);
			while (!validar_fecha(jugador->fecha_nacimiento)) {
				printf("\nFecha inválida. Ingrese fecha de nacimiento (DD-MM-YYYY): ");
				scanf("%10s", jugador->fecha_nacimiento);
			}
			printf("\nNombre (%s): ", jugador->nombre);
			scanf("%49s", jugador->nombre);
			printf("\nApellido (%s): ", jugador->apellido);
			scanf("%49s", jugador->apellido);
			printf("\n¿Está activo? (s/n) (%c): ", jugador->activo ? 's' : 'n');
			char activo;
			scanf(" %c", &activo);
			jugador->activo = (activo == 's'|| activo == 'S');
			printf("\nDatos de %s modificados exitosamente.\n", alias);
			return;
		}
	}
	printf("\nNo se encontró ningún jugador con el alias %s.\n", alias);
}

void listar_jugadores() {
	// Ordenar jugadores por alias en orden alfabético utilizando ordenamiento burbuja
	for (int i = 0; i < num_jugadores - 1; i++) {
		for (int j = 0; j < num_jugadores - i - 1; j++) {
			if (strcmp(jugadores[j].alias, jugadores[j + 1].alias) > 0) {
				// Intercambiar jugadores[j] y jugadores[j + 1]
				Jugador temp = jugadores[j];
				jugadores[j] = jugadores[j + 1];
				jugadores[j + 1] = temp;
			}
		}
	}
	
	printf("\nListado de jugadores activos:\n");
	for (int i = 0; i < num_jugadores; i++) {
		if (jugadores[i].activo) {
			printf("\nAlias: %s, CI: %s, Nombre: %s %s, Partidas Ganadas: %d\n", jugadores[i].alias, jugadores[i].ci, jugadores[i].nombre, jugadores[i].apellido, jugadores[i].partidas_ganadas);
		}
	}
}

/*void listar_jugadores() {
	printf("\nListado de jugadores activos:\n");
	for (int i = 0; i < num_jugadores; i++) {
		if (jugadores[i].activo) {
			printf("\nAlias: %s, CI: %s, Nombre: %s %s, Partidas Ganadas: %d\n", jugadores[i].alias, jugadores[i].ci, jugadores[i].nombre, jugadores[i].apellido, jugadores[i].partidas_ganadas);
		}
	}
}*/

void listar_partidas() {
	printf("\nListado de todas las partidas:\n");
	for (int i = 0; i < num_partidas; i++) {
		printf("\nFecha: %s, Alias: %s, Resultado: %s\n", partidas[i].fecha, partidas[i].alias, partidas[i].resultado);
	}
}

void reiniciar_estado_del_juego() {
	marcadas = 0;
	exploradas = 0;
	primera_jugada = 1;
	game_over = 0;
	inicializar_tablero();
}


void inicializar_tablero() {
	for (int i = 0; i < FILAS; i++) {
		for (int j = 0; j < COLUMNAS; j++) {
			tablero[i][j] = '?';
			tablero_oculto[i][j] = '?';
		}
	}
}

void colocar_minas(int primera_fila, int primera_columna) {
	int minas_colocadas = 0;
	while (minas_colocadas < MINAS) {
		int fila = rand() % FILAS;
		int columna = rand() % COLUMNAS;
		if ((fila != primera_fila || columna != primera_columna) && tablero_oculto[fila][columna]
			!= 'B') {
			tablero_oculto[fila][columna] = 'B';
			minas_colocadas++;
		}
	}
}

void mostrar_tablero() {
	printf("\n  A B C D E F G H\n");
	for (int i = 0; i < FILAS; i++) {
		printf("%c ", 'A' + i);
		for (int j = 0; j < COLUMNAS; j++) {
			printf("%c ", tablero[i][j]);
		}
		printf("\n");
	}
}

void revelar_todas_las_bombas() {
	for (int i = 0; i < FILAS; i++) {
		for (int j = 0; j < COLUMNAS; j++) {
			if (tablero_oculto[i][j] == 'B') {
				tablero[i][j] = 'B';
			}
		}
	}
}
void explorar(int fila, int columna) {
	if (fila < 0 || fila >= FILAS || columna < 0 || columna >= COLUMNAS) {
		printf("Formato de jugada erróneo\n");
		return;
	}
	if (tablero[fila][columna] != '?') {
		printf("Formato de jugada erróneo\n");
		return;
	}
	
	if (primera_jugada) {
		colocar_minas(fila, columna);
		primera_jugada = 0;
	}
	if (tablero_oculto[fila][columna] == 'B') {
		tablero[fila][columna] = 'B';
		revelar_todas_las_bombas();
		game_over = 1;
		return;
	}
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

void marcar(int fila, int columna) {
	if (fila < 0 || fila >= FILAS || columna < 0 || columna >= COLUMNAS) {
		printf("Formato de jugada erróneo\n");
		return;
	}
	//Marcar el casillero elegido suplantando ? por X
	if (tablero[fila][columna] == '?' || tablero[fila][columna] == 'X') {
		if (marcadas < MAX_MARCADAS || tablero[fila][columna] == 'X') {
			tablero[fila][columna] = (tablero[fila][columna] == '?') ? 'X' : '?'; // 
			marcadas += (tablero[fila][columna] == 'X') ? 1 : -1;
		} else {
			printf("No se puede marcar más de %d casillas.\n", MAX_MARCADAS);
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

void buscar(int fila, int columna) {
	
	//En caso de que seleciona una opcion fuera del rango
	if (fila < 0 || fila >= FILAS || columna < 0 || columna >= COLUMNAS) {
		printf("Formato de jugada erróneo\n");
		return;
	}
	
	//En caso de que seleccione una casilla no explorada
	if (tablero[fila][columna] == '?' || tablero[fila][columna] == 'X') {
		printf("Jugada inválida: Debes buscar desde una casilla explorada\n");
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
	if (tablero[fila][columna] - '0' != minas_marcadas_adyacentes) {
		printf("Jugada inválida: La cantidad de minas marcadas adyacentes no coincide\n");
		return;
	}
	int revelado_ceros[FILAS][COLUMNAS] = {0}; // Matriz para marcar ceros revelados en la primera ronda
	for (int i = fila - 1; i <= fila + 1; i++) {
		for (int j = columna - 1; j <= columna + 1; j++) {
			if (i >= 0 && i < FILAS && j >= 0 && j < COLUMNAS && tablero[i][j] =='?') {
				if (tablero_oculto[i][j] == 'B') {
					revelar_todas_las_bombas();
					game_over = 1;
					return;
				} 
				else {
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



void rendirse() {
	printf("Usted se ha rendido.\n");
	revelar_todas_las_bombas();
	game_over = 1;
}

void volver_al_menu_principal() {
	main();
}


void limpiar_buffer() {
	while (getchar() != '\n') {}
}

void jugar() {
	char alias[50];
	printf("\nIngrese su alias: ");
	scanf("%49s", alias);
	int encontrado = 0;
	for (int i = 0; i < num_jugadores; i++) {
		if (strcmp(jugadores[i].alias, alias) == 0) {
			encontrado = 1;
			break;
		}
	}
	if (!encontrado) {
		printf("\nAlias no encontrado. Juego cancelado.\n");
		return;
	}
	srand(time(NULL));
	reiniciar_estado_del_juego();
	printf("\n¡Bienvenido al Buscaminas!\n");
	printf("Para jugar, elige una opción seguida de una fila (A-H) y una columna (A-H).\n");
	printf("Las opciones son: (E)xplorar, (M)arcar, (B)uscar y (R)endirse.\n");
	mostrar_tablero();
	while (exploradas < CASILLAS_SIN_MINAS && !game_over) {
		char opcion;
		char fila;
		char columna;
		printf("\nElige una opción: ");
		scanf(" %c", &opcion);
		if (opcion == 'R') {
			rendirse();
			break;
		}
		scanf(" %c %c", &fila, &columna);
		limpiar_buffer();
		fila -= 'A';
		columna -= 'A';
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
			printf("\nOpción inválida\n");
		}
		mostrar_tablero();
	}
	if (exploradas == CASILLAS_SIN_MINAS) {
		printf("\n¡Pfff... fue pura suerte, la próxima no será tan fácil!\n");
	} else if (game_over) {
		printf("\n¡¡¡¡ PERDISTE !!!! Suerte la próxima, se nota que la necesitas.\n");
	}
	
	// Almacenar la partida
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char fecha_partida[20];
	sprintf(fecha_partida, "%04d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	strcpy(partidas[num_partidas].fecha, fecha_partida);
	strcpy(partidas[num_partidas].alias, alias);
	strcpy(partidas[num_partidas].resultado, exploradas == CASILLAS_SIN_MINAS ? "Ganada" : "Perdida");
	num_partidas++;
	
	// Actualizar partidas ganadas si el jugador ganó
	if (exploradas == CASILLAS_SIN_MINAS) {
		for (int i = 0; i < num_jugadores; i++) {
			if (strcmp(jugadores[i].alias, alias) == 0) {
				jugadores[i].partidas_ganadas++;
				break;
			}
		}
	}
}
