#include <iostream>
#include <cmath>    // Para usar pow() y log()
#include <iomanip>  // Para formatos de salida
#include <limits>   // Para manejar la limpieza de buffer

using namespace std;

// Constante para un pequeño número de tolerancia
const double EPSILON = 1e-6;

// =================================================================
// ESTRUCTURAS DE DATOS Y UTILIDADES
// =================================================================

// Función para limpiar el buffer de entrada en caso de errores
void limpiarBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Función para solicitar un valor (doble) con validación básica
double solicitarValor(const string& nombre) {
    double valor;
    while (!(cout << "Ingrese el valor de " << nombre << ": " && cin >> valor) || valor < 0) {
        cout << "Entrada inválida. Por favor, ingrese un número positivo." << endl;
        limpiarBuffer();
    }
    return valor;
}

// Función para solicitar un número entero (periodos) con validación
int solicitarEntero(const string& nombre) {
    int valor;
    while (!(cout << "Ingrese el valor de " << nombre << " (entero): " && cin >> valor) || valor <= 0) {
        cout << "Entrada inválida. Por favor, ingrese un número entero positivo." << endl;
        limpiarBuffer();
    }
    return valor;
}

// =================================================================
// FÓRMULAS DE ANUALIDAD VENCIDA (ORDINARIA)
// =================================================================

// Monto (F): F = R * [((1+i)^n - 1) / i]
double calcularMontoVencida(double R, double i, int n) {
    double factor = (pow(1 + i, n) - 1) / i;
    return R * factor;
}

// Valor Actual (P): P = R * [(1 - (1+i)^-n) / i]
double calcularValorActualVencida(double R, double i, int n) {
    double factor = (1 - pow(1 + i, -n)) / i;
    return R * factor;
}

// Renta (R) dado Monto (F): R = F * [i / ((1+i)^n - 1)]
double calcularRentaDadoFVencida(double F, double i, int n) {
    double factor_inverso = i / (pow(1 + i, n) - 1);
    return F * factor_inverso;
}

// Renta (R) dado Valor Actual (P): R = P * [i / (1 - (1+i)^-n)]
double calcularRentaDadoPVencida(double P, double i, int n) {
    double factor_inverso = i / (1 - pow(1 + i, -n));
    return P * factor_inverso;
}

// Periodos (n) dado Monto (F): n = log(1 + (F*i)/R) / log(1+i)
int calcularPeriodosDadoFVencida(double F, double R, double i) {
    if (abs(R) < EPSILON || abs(i) < EPSILON) return -1; // Evitar división por cero
    double numerador = log(1 + (F * i) / R);
    double denominador = log(1 + i);
    return round(numerador / denominador);
}

// Periodos (n) dado Valor Actual (P): n = -log(1 - (P*i)/R) / log(1+i)
int calcularPeriodosDadoPVencida(double P, double R, double i) {
    if (abs(R) < EPSILON || abs(i) < EPSILON) return -1; // Evitar división por cero
    double argumento_log = 1 - (P * i) / R;
    if (argumento_log <= 0) return -1; // Argumento inválido para log
    double numerador = -log(argumento_log);
    double denominador = log(1 + i);
    return round(numerador / denominador);
}

// =================================================================
// FÓRMULAS DE ANUALIDAD ANTICIPADA
// =================================================================

// Monto (F) Anticipada: F = F_Vencida * (1+i)
double calcularMontoAnticipada(double R, double i, int n) {
    return calcularMontoVencida(R, i, n) * (1 + i);
}

// Valor Actual (P) Anticipada: P = P_Vencida * (1+i)
double calcularValorActualAnticipada(double R, double i, int n) {
    return calcularValorActualVencida(R, i, n) * (1 + i);
}

// Renta (R) dado Monto (F) Anticipada: R = R_Vencida / (1+i)
double calcularRentaDadoFAnticipada(double F, double i, int n) {
    return calcularRentaDadoFVencida(F, i, n) / (1 + i);
}

// Renta (R) dado Valor Actual (P) Anticipada: R = R_Vencida / (1+i)
double calcularRentaDadoPAnticipada(double P, double i, int n) {
    return calcularRentaDadoPVencida(P, i, n) / (1 + i);
}

// Periodos (n) dado Monto (F) Anticipada: Se usa la misma fórmula logarítmica que la vencida.
int calcularPeriodosDadoFAnticipada(double F, double R, double i) {
    // La fórmula de la vencida se puede usar ajustando R: R_Vencida = R_Anticipada * (1+i)
    double R_equivalente = R * (1 + i);
    return calcularPeriodosDadoFVencida(F, R_equivalente, i);
}

// Periodos (n) dado Valor Actual (P) Anticipada: Se usa la misma fórmula logarítmica que la vencida.
int calcularPeriodosDadoPAnticipada(double P, double R, double i) {
    // La fórmula de la vencida se puede usar ajustando R: R_Vencida = R_Anticipada * (1+i)
    double R_equivalente = R * (1 + i);
    return calcularPeriodosDadoPVencida(P, R_equivalente, i);
}

// =================================================================
// MENÚS Y LÓGICA DE INTERFAZ
// =================================================================

// Función para ejecutar el cálculo de la variable seleccionada
void ejecutarCalculo(int tipoAnualidad, char variableACalcular) {
    // Definir la función de cálculo según el tipo de anualidad
    function<double(double, double, int)> funcF = (tipoAnualidad == 1) ? calcularMontoVencida : calcularMontoAnticipada;
    function<double(double, double, int)> funcP = (tipoAnualidad == 1) ? calcularValorActualVencida : calcularValorActualAnticipada;
    function<double(double, double, int)> funcR_F = (tipoAnualidad == 1) ? calcularRentaDadoFVencida : calcularRentaDadoFAnticipada;
    function<double(double, double, int)> funcR_P = (tipoAnualidad == 1) ? calcularRentaDadoPVencida : calcularRentaDadoPAnticipada;
    function<int(double, double, double)> funcN_F = (tipoAnualidad == 1) ? calcularPeriodosDadoFVencida : calcularPeriodosDadoFAnticipada;
    function<int(double, double, double)> funcN_P = (tipoAnualidad == 1) ? calcularPeriodosDadoPVencida : calcularPeriodosDadoPAnticipada;

    double R = 0.0, i = 0.0, P = 0.0, F = 0.0;
    int n = 0;
    double resultado = 0.0;
    int resultado_n = 0;

    cout << "\n--- Cálculo de Anualidad " << (tipoAnualidad == 1 ? "Vencida" : "Anticipada") << " ---" << endl;

    // Se solicitan solo las variables necesarias para el cálculo
    // La variable a calcular se ignora en la solicitud de entrada

    if (variableACalcular != 'R') R = solicitarValor("R (Renta/Pago)");
    if (variableACalcular != 'i') i = solicitarValor("i (Tasa por Período, en decimal)");
    if (variableACalcular != 'n') n = solicitarEntero("n (Número de Períodos)");

    if (variableACalcular == 'F') {
        P = solicitarValor("P (Valor Actual/Capital)");
        resultado = funcF(R, i, n);
        cout << fixed << setprecision(2);
        cout << "\n✅ Monto (Valor Futuro) Calculado (F): " << resultado << endl;
    } else if (variableACalcular == 'P') {
        F = solicitarValor("F (Monto/Valor Futuro)");
        resultado = funcP(R, i, n);
        cout << fixed << setprecision(2);
        cout << "\n✅ Valor Actual (Capital) Calculado (P): " << resultado << endl;
    } else if (variableACalcular == 'R') {
        // Para calcular Renta, el usuario debe proveer P o F
        char eleccionPF;
        cout << "¿Desea calcular R con (P) Valor Actual o (F) Monto? (P/F): ";
        cin >> eleccionPF;
        limpiarBuffer();

        if (toupper(eleccionPF) == 'P') {
            P = solicitarValor("P (Valor Actual/Capital)");
            i = solicitarValor("i (Tasa por Período, en decimal)");
            n = solicitarEntero("n (Número de Períodos)");
            resultado = funcR_P(P, i, n);
            cout << fixed << setprecision(2);
            cout << "\n✅ Renta (Pago Periódico) Calculada (R): " << resultado << endl;
        } else if (toupper(eleccionPF) == 'F') {
            F = solicitarValor("F (Monto/Valor Futuro)");
            i = solicitarValor("i (Tasa por Período, en decimal)");
            n = solicitarEntero("n (Número de Períodos)");
            resultado = funcR_F(F, i, n);
            cout << fixed << setprecision(2);
            cout << "\n✅ Renta (Pago Periódico) Calculada (R): " << resultado << endl;
        } else {
            cout << "\nOpción inválida." << endl;
        }
    } else if (variableACalcular == 'n') {
        // Para calcular Periodos, el usuario debe proveer P o F
        char eleccionPF;
        cout << "¿Desea calcular n con (P) Valor Actual o (F) Monto? (P/F): ";
        cin >> eleccionPF;
        limpiarBuffer();

        if (toupper(eleccionPF) == 'P') {
            P = solicitarValor("P (Valor Actual/Capital)");
            R = solicitarValor("R (Renta/Pago)");
            i = solicitarValor("i (Tasa por Período, en decimal)");
            resultado_n = funcN_P(P, R, i);
        } else if (toupper(eleccionPF) == 'F') {
            F = solicitarValor("F (Monto/Valor Futuro)");
            R = solicitarValor("R (Renta/Pago)");
            i = solicitarValor("i (Tasa por Período, en decimal)");
            resultado_n = funcN_F(F, R, i);
        } else {
            cout << "\nOpción inválida." << endl;
            return;
        }

        if (resultado_n != -1) {
            cout << "\n✅ Número de Períodos Calculado (n): " << resultado_n << endl;
            // Recordar que el resultado es la cantidad entera de pagos.
        } else {
            cout << "\n❌ ERROR: No se pudo calcular 'n'. Verifique los valores de entrada (R y i no pueden ser cero; argumento logarítmico debe ser positivo)." << endl;
        }
    } else {
        cout << "\nOpción de cálculo no implementada o inválida." << endl;
    }
}

// =================================================================
// FUNCIÓN PRINCIPAL
// =================================================================

int main() {
    int opcionAnualidad = 0;
    char opcionCalculo;
    
    cout << setprecision(2) << fixed;

    while (opcionAnualidad != 3) {
        cout << "\n==========================================" << endl;
        cout << "  🛠️  CALCULADORA DE ANUALIDADES SIMPLES  " << endl;
        cout << "==========================================" << endl;
        cout << "1. Anualidad Vencida (Ordinaria)" << endl;
        cout << "2. Anualidad Anticipada" << endl;
        cout << "3. Salir" << endl;
        cout << "Seleccione el tipo de anualidad: ";

        if (!(cin >> opcionAnualidad)) {
            limpiarBuffer();
            opcionAnualidad = 0; // Para forzar la repetición
            continue;
        }

        if (opcionAnualidad == 1 || opcionAnualidad == 2) {
            cout << "\n------------------------------------------" << endl;
            cout << "  SELECCIONE LA VARIABLE A CALCULAR  " << endl;
            cout << "------------------------------------------" << endl;
            cout << "F - Monto (Valor Futuro)" << endl;
            cout << "P - Valor Actual (Capital)" << endl;
            cout << "R - Renta (Pago Periódico)" << endl;
            cout << "n - Número de Períodos" << endl;
            cout << "Seleccione (F/P/R/n): ";
            cin >> opcionCalculo;
            limpiarBuffer();
            opcionCalculo = toupper(opcionCalculo);

            if (opcionCalculo == 'F' || opcionCalculo == 'P' || opcionCalculo == 'R' || opcionCalculo == 'N') {
                ejecutarCalculo(opcionAnualidad, opcionCalculo);
            } else {
                cout << "\n⚠️ Opción de cálculo no válida. Intente de nuevo." << endl;
            }
        } else if (opcionAnualidad == 3) {
            cout << "\n¡Gracias por usar la calculadora! Saliendo del programa." << endl;
        } else {
            cout << "\n⚠️ Opción de anualidad no válida. Intente de nuevo." << endl;
        }
    }

    return 0;
}
