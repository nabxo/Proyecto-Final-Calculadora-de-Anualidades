// =================================================================
// UTILIDADES DE INTERFAZ
// =================================================================

// Muestra/Oculta los campos de entrada según la variable a calcular
function toggleInputs() {
    const varCalc = document.getElementById('variableACalcular').value;
    
    // Ocultar todos los campos opcionales al inicio
    document.querySelectorAll('.input-item').forEach(el => el.classList.remove('hidden'));
    document.getElementById('inputF').classList.add('hidden');
    document.getElementById('inputP').classList.add('hidden');

    // Ocultar el campo que se va a calcular
    document.getElementById('input' + varCalc).classList.add('hidden');

    // Reglas para el campo Renta (R) o Periodos (n): Necesitan F o P
    if (varCalc === 'R' || varCalc === 'n') {
        // En estos casos, R o n están ocultos, y F y P deben ser mostrados para que el usuario elija
        document.getElementById('inputF').classList.remove('hidden');
        document.getElementById('inputP').classList.remove('hidden');
    }
}

// Resetea el formulario y los resultados al cambiar el tipo de anualidad
function resetForm() {
    // Resetear solo los valores de los inputs, no el formulario completo
    document.getElementById('R').value = '';
    document.getElementById('i').value = '';
    document.getElementById('n').value = '';
    document.getElementById('F').value = '';
    document.getElementById('P').value = '';
    
    document.getElementById('resultado').classList.add('hidden');
    toggleInputs();
}

// Inicializar la interfaz al cargar la página
window.onload = toggleInputs;

// =================================================================
// FUNCIONES DE CÁLCULO
// =================================================================

// Factor de Valor Futuro (FVA): [((1+i)^n - 1) / i]
function factorFV(i, n) {
    if (i === 0) return n; // Caso especial para i=0 (interés simple)
    return (Math.pow(1 + i, n) - 1) / i;
}

// Factor de Valor Actual (PVA): [(1 - (1+i)^-n) / i]
function factorPV(i, n) {
    if (i === 0) return n; // Caso especial para i=0 (interés simple)
    return (1 - Math.pow(1 + i, -n)) / i;
}

function calcularAnualidad() {
    const tipoAnualidad = document.getElementById('tipoAnualidad').value;
    const varCalc = document.getElementById('variableACalcular').value;
    
    document.getElementById('resultado').classList.add('hidden');
    
    // Recolección de datos
    let R = parseFloat(document.getElementById('R').value || 0);
    let i = parseFloat(document.getElementById('i').value || 0);
    let n = parseInt(document.getElementById('n').value || 0);
    let F = parseFloat(document.getElementById('F').value || 0);
    let P = parseFloat(document.getElementById('P').value || 0);

    let resultado = null;
    let nombreVar = '';

    // Validaciones básicas
    if (i < 0 || n <= 0) {
        mostrarResultado("⚠️ Error: La tasa (i) debe ser positiva o cero y el número de periodos (n) debe ser mayor que cero.", false);
        return;
    }
    
    try {
        if (varCalc === 'F') {
            nombreVar = 'Monto (F)';
            let factor = factorFV(i, n);
            resultado = (tipoAnualidad === 'anticipada') ? R * factor * (1 + i) : R * factor;

        } else if (varCalc === 'P') {
            nombreVar = 'Valor Actual (P)';
            let factor = factorPV(i, n);
            resultado = (tipoAnualidad === 'anticipada') ? R * factor * (1 + i) : R * factor;
            
        } else if (varCalc === 'R') {
            nombreVar = 'Renta (R)';
            // El usuario debe elegir si usa F o P
            if (F > 0 && P === 0) { // Calcular R dada F
                let factor = factorFV(i, n);
                resultado = F / factor;
                if (tipoAnualidad === 'anticipada') resultado /= (1 + i);
            } else if (P > 0 && F === 0) { // Calcular R dada P
                let factor = factorPV(i, n);
                resultado = P / factor;
                if (tipoAnualidad === 'anticipada') resultado /= (1 + i);
            } else {
                mostrarResultado("⚠️ Error: Para calcular Renta, ingrese solo el Monto (F) O solo el Valor Actual (P).", false);
                return;
            }

        } else if (varCalc === 'n') {
            nombreVar = 'Número de Períodos (n)';
            
            let R_calc = R;
            let P_calc = P;
            let F_calc = F;

            if (tipoAnualidad === 'anticipada') {
                // Para usar la fórmula logarítmica de la vencida, ajustamos R
                R_calc = R * (1 + i);
            }
            
            if (P > 0 && F === 0) { // Calcular n dada P
                let arg_log = 1 - (P_calc * i) / R_calc;
                if (arg_log <= 0 || R_calc <= 0) throw new Error("Argumento logarítmico inválido o Renta nula. Revise sus valores.");
                resultado = -Math.log(arg_log) / Math.log(1 + i);
            } else if (F > 0 && P === 0) { // Calcular n dada F
                let arg_log = 1 + (F_calc * i) / R_calc;
                if (arg_log <= 0 || R_calc <= 0) throw new Error("Argumento logarítmico inválido o Renta nula. Revise sus valores.");
                resultado = Math.log(arg_log) / Math.log(1 + i);
            } else {
                mostrarResultado("⚠️ Error: Para calcular 'n', ingrese solo el Monto (F) O solo el Valor Actual (P).", false);
                return;
            }
            // Para 'n', el resultado es el número entero de pagos, redondeamos hacia arriba (techo)
            resultado = Math.ceil(resultado);
        }

        // Mostrar el resultado final
        if (resultado !== null && !isNaN(resultado)) {
            let decimales = (varCalc === 'n' ? 0 : 4);
            let resultadoFormateado = resultado.toFixed(decimales);
            mostrarResultado(`El ${nombreVar} es: ${resultadoFormateado}`, true);
        } else {
            mostrarResultado("⚠️ Error: No se pudo realizar el cálculo. Revise sus entradas.", false);
        }

    } catch (e) {
        mostrarResultado(`❌ Error de Cálculo: ${e.message}`, false);
    }
}

// Muestra el resultado en la caja de resultados
function mostrarResultado(mensaje, exito) {
    const resultadoDiv = document.getElementById('resultado');
    const resultadoTexto = document.getElementById('resultadoTexto');
    
    resultadoTexto.textContent = mensaje;
    resultadoDiv.classList.remove('hidden');
    resultadoDiv.style.borderColor = exito ? '#28a745' : '#dc3545';
    resultadoDiv.style.borderLeftColor = exito ? '#28a745' : '#dc3545';
    resultadoDiv.style.backgroundColor = exito ? '#e9f7ef' : '#f8d7da';
    resultadoTexto.style.color = exito ? '#007bff' : '#dc3545';
}
