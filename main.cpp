#include "datos.h"
#include "math.h"
#include "mbed.h"
 

#define longitudTrama 200
#define fs (50 * 20)

Timer timer;
float resultado;
int tiempo;

struct estructuraMedidas {
  float vrms;
  float irms;
  float potenciaActiva;
  float potenciaReactiva;
  float potenciaAparente;
  float factorDePotencia;
  float energiaConsumida;
};

float calcularRMS(const uint16_t *datos, int longitud);
void calcularDatos(const uint16_t *datosV, const uint16_t *datosI, int longitud, estructuraMedidas *medidas);

int main() {

  timer.reset();
  timer.start();
  resultado = calcularRMS(datos, longitudTrama);
  timer.stop();
  printf("\n****El valor Vrms es %f calculado en %lld us ****\n\n", resultado,
         timer.elapsed_time().count());

  estructuraMedidas medidas = {0}; // Se inicializa todo a 0

  timer.reset();
  timer.start();
  calcularDatos(datosV, datosI, longitudTrama, &medidas);
  timer.stop();
  printf("**** Datos calculados en %lld us ****\n",
         timer.elapsed_time().count());
  printf("**** El valor Vrms es %f ****\n", medidas.vrms);
  printf("**** El valor Irms es %f ****\n", medidas.irms);
  printf("**** La potencia activa es %f ****\n", medidas.potenciaActiva);
  printf("**** La potencia reactiva es %f ****\n", medidas.potenciaReactiva);
  printf("**** La potencia aparente es %f ****\n", medidas.potenciaAparente);
  printf("**** La energia consumida es %f ****\n", medidas.energiaConsumida);
  printf("**** El factor de potencia es es %f ****\n",
         medidas.factorDePotencia);

  while (true) {
  }
}

// Esta función calcula el valor RMS
 inline float calcularRMS(const uint16_t *datos, int longitud) {
  float rms = 0.0;
  const float constante = 800.0f / 65536.0f; // divido al revés para que sea más rápido y así luego multiplico

  for (int i = 0; i < longitud; i++) {
    const float datoV = ((datos[i]) * constante) - 400.0f;
    rms += datoV * datoV;
  }
  
  return sqrt(rms / longitud);

}

void calcularDatos(const uint16_t *datosV, const uint16_t *datosI, int longitud,
                   estructuraMedidas *medidas) {
  float Vrms = 0.0, Irms = 0.0, P = 0.0;

  // vuelvo a declarar porque si hago como global tarda más
  const float constante =
      800.0 /65536; // divido al revés para que sea más rápido y así luego multiplico
  const float constante_2 = 5.0 / 65536;

  for (int i = 0; i < longitud; i++) {
    const float datoV = ((datosV[i]) * constante) - 400.0;
    Vrms += datoV * datoV;
    const float datoI = ((datosI[i]) * constante_2) - 2.5;
    Irms += datoI * datoI;
    P += datoV * datoI;
  }

  // FUNCION SQRT? no hace falta
  // SE PUEDE HACER COMO EN MATLAB LO DE METER A LA DCHA? format document
  
  medidas->vrms = sqrt(Vrms / longitud);
  medidas->irms = sqrt(Irms / longitud);
  medidas->potenciaActiva = P / longitud;
  medidas->potenciaAparente = medidas->vrms * medidas->irms;
  medidas->potenciaReactiva = sqrt((medidas->potenciaAparente)*(medidas->potenciaAparente)- (medidas->potenciaActiva)*(medidas->potenciaActiva));
  medidas->factorDePotencia = medidas->potenciaActiva / medidas->potenciaAparente;
  medidas->energiaConsumida += medidas->potenciaActiva / fs * longitudTrama / 3600000; // simplificamos
}
