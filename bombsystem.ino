const int PresSens = A1;  // Puerto que voy a usar para leer el valor del sensor de presión
float VPS;                // Variable en la que voy a poner el valor obtenido en el sensado de la presión
int FlotanteSis = 3;      // Entrada digital que voy a usar para el flotante de la sisterna, seguramente va a ser un sensor óptico en verdad.
int BotVacMode = 4;       // Entrada digital que voy a usar en caso de que el sistema tenga menos uso del normal, va con una llave a masa.

bool FlotanteSIS = 0;     // Variable donde se guarda el estado del flotante  
bool BotVacMODE = 0;      // Variable donde se guarda el estado del switch del modo vacaciones.

int BombaOut = 8; // Salida utilizada para activar el relay de la bomba
int ElectrOut = 9; // Salida utilizada para activar el relay de la electrovalvula utilizada para permitir el llenado de la sisterna.
int ledpin=13; //led bulit in salida

const float TanqueLleno = 300; // valor prefijado de presión que tiene el sistema cuando el floante del tanque está cerrado y la bomba sigue encendida (300 es un valor aleatorio que hay que cambiar). 
const float SisVac = 300; // Valor prefijado de presión que tiene el sistema cuando la bomba está andando y la sisterna está vacía.
const float LLenand = 300; // valor prefijado de presión que tiene el sistema cuando se está llenando el tanque.
const float Off = 300; // valor prefijado bomba apagada
const float MasMenos = 50; //valor para sumar o restar y usar las variables anteriores con un margen de error de masmenos.

int timersis = 100000;
int Timersis = 100000;
int timerbomb =10000;
int Timerbomb =10000;
int timerresbomb =500;
int Timerresbomb =500;
int timerstartbomb=500;
int Timerstartbomb=500;

bool FlagSisterna = 0;

void setup() {
Serial.begin(9600);
pinMode(FlotanteSis, INPUT_PULLUP);
pinMode(BotVacMode, INPUT_PULLUP);


}

void loop() {
  
  VPS = analogRead(PresSens);
  FlotanteSIS = digitalRead(FlotanteSis);
  BotVacMODE = digitalRead(BotVacMode);  // leo los sensores y el selector de modo
  
  

    if (FlotanteSis == 1)  {
      digitalWrite(ElectrOut, 0); // si el flotante no marca que la sisterna está llena, lleno, (chequear lógica).
      timersis --; //
      FlagSisterna = 1 ; // habilito el llenado del tanque 

    }
    else {
      timersis = Timersis;           // reseteo el timer
      digitalWrite(ElectrOut, 1); // si está llena la sisterna, dejo de llenar
    }

    if(timersis == 0 ){
      Serial.println("error en la carga de la sisterna");
      digitalWrite(ledpin, 0);
      timersis = Timersis;           // reseteo el timer
      digitalWrite(ElectrOut, 1);
      bool state = digitalRead(BombaOut);
      digitalWrite(BombaOut,1);
      delay(50000);                    //freno un toque para enfriar y depaso bloqueo el código
      digitalWrite(ElectrOut, 0);
      digitalWrite(BombaOut,state);
    }

   
// caso puedo mantener la preción en el sistema:

  
 //   if ((PresSens < TanqueLleno + MasMenos) && (timerbomb != 0) && (timerresbomb == 0)){  // si la presión en el sistema es menor a la que tenemos cuando el tanque está lleno, arranco la bomba (timerbomb representa que no haya tardado una eternidad en llenar para detectar errores, timer resbomb es un timer que si está en 0 representa que queres llenar el tanque, mientras que se pone a contar una vez que el mismo está lleno)
 //    digitalWrite(BombaOut, 0); 
 //    timerbomb-- ;
 //  }
    
 // caso no puedo mantener la presción en el sistema  
 timerresbomb --; // cuento el tiempo para volver a intentar llenar
 if (timerresbomb < 0){timerresbomb=0;}  // evito números negativos (no recuerdo que tipo de variable es).

    if ((timerstartbomb != 0) && (timerresbomb == 0)){
        digitalWrite(BombaOut, 0); 
    //  timerbomb-- ;
    }
    // falta que hagamos algo si la presión cae por debajo del minimo o sea que la sisterna está vacia
    if(PresSens > TanqueLleno - MasMenos  ){ // si la presión es más que la predefinida de tnque lleno, apago la bomba
      digitalWrite(BombaOut, 1);
      timerstartbomb == Timerstartbomb;        // resetear esto pone denuevo el tiempo para medir el vaciado de la sisterna
      timerresbomb == Timerresbomb; // resetear esto pone a contar el tiempo para voler a intentar llenar
    }
      else{ // verificar cuando agregue lo de la presión baja, que este else no quede arafue
        timerstartbomb --;           //empiezo a contar el tiempo de vaciado de la sisterna (10 min aprox) 
        

      }
    if (timerstartbomb <= 99){  // 99 es un número aleatorio que representa el tiempo que tarda en estabilizar el sistema de subida de agua
      if (!((PresSens > LLenand + MasMenos) || (PresSens > LLenand - MasMenos)) || (timerstartbomb == 0 )) { // si la precion NO es mas o menos la que corresponde a que está llenando o terminó tiempo de llenado,.. 
      //  if (PresSens < SisVac - MasMenos){            //pregunto si la presión cayo
        digitalWrite(BombaOut, 1);            //apago la bomba 
        timerstartbomb = Timerstartbomb;     // reseteo el tiempo de llenado del tanque
        FlagSisterna = 0;         // deshabilito el llendao del tanque ya que se supone que la sisterna está vacia.
        }
    }

}
