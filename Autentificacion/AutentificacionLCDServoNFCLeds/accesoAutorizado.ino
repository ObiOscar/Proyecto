#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 10                     //Pin de la tarjeta MFRC522 SDA
#define RST_PIN 9                     //Pin de la tarjeta MFRC522 RESET
#define LED_VERDE 5                   //Led verde para saber si tenemos acceso
#define LED_ROJO 6                    //Led rojo que no tenemos acceso
#define BUZZER 2                      //buzzer pin
#define VELOCIDAD 150                 // Velocidad a la que se mueve el texto



MFRC522 mfrc522(SS_PIN, RST_PIN);     //creo la instancia de MFRC522 PASANDO POR PARÁMETRO EL SDA Y EL RESET
Servo servoApertura;                  //Defino mi servo
String texto_identificar = "Identifiquese";
LiquidCrystal_I2C lcd(0x27, 20, 4);   // configuro la dirección de LCD en 0x27 para una pantalla de 16 caracteres y 2 líneas

#define NUMERO_USUARIOS_CON_ACCESO 2
String tarjetasConAcceso[NUMERO_USUARIOS_CON_ACCESO] = { "16 1A D0 02", "16 1A D0 05" };

void setup()
{
  Serial.begin(9600);         //Inicializo la comunicación serial
  SPI.begin();                //inicia el bus spi de la tarjeta MFRC522
  mfrc522.PCD_Init();         //Inicio la tarjeta
  servoApertura.attach(3);    //Digo al servo cual va a ser el pin de datos (3 analógico)
  servoApertura. write(0);     //Inicializo los led, el buzzer y el servo apagados o en 0
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_ROJO, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  noTone(BUZZER);

  // inicializa la LCD
  lcd.init();
  lcd.backlight();
  moverTexto();


  Serial.println(texto_identificar);
  Serial.println();

}
void loop()
{
  // Esta esperando nuevas tarjetas
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  //Muesta la UID pero solo en el serial, no quiero que se vea en la LCD, esto valdrá en un futuro para introducir nuevas tarjetas
  Serial.print("número de UID:");
  String uidObteneido = "";

  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    uidObteneido.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    uidObteneido.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("MENSAJE : ");
  uidObteneido.toUpperCase();

  for (int i = 0; i < NUMERO_USUARIOS_CON_ACCESO; i++) {
    if (uidObteneido.substring(1) == tarjetasConAcceso[i] || uidObteneido.substring(1) == tarjetasConAcceso[i + 1]) //Compruebo mi array con las tarjetas que tienen acceso
    {
      Serial.println("Acceso Autorizado");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Acceso Autorizado");
      delay(1000);
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("BIENVENIDO");
      lcd.setCursor(4, 1);
      lcd.print("Sr.Oscar");
      delay(500);
      digitalWrite(LED_VERDE, HIGH);
      tone(BUZZER, 500);
      delay(300);
      noTone(BUZZER);
      servoApertura.write(180);
      delay(5000);
      servoApertura.write(0);
      digitalWrite(LED_VERDE, LOW);
      lcd.clear();
      moverTexto();

    }
    else   {
      lcd.clear();
      lcd.setCursor(0, 0);
      Serial.println("Aceeso denegando");
      lcd.print("Aceeso denegando");
      digitalWrite(LED_ROJO, HIGH);
      tone(BUZZER, 300);
      delay(2000);
      digitalWrite(LED_ROJO, LOW);
      noTone(BUZZER);
      lcd.clear();
      moverTexto();

    }
  }
}
void moverTexto() {
  // Obtenemos el tamaño del texto
  int tam_texto = texto_identificar .length();

  while (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {    //Siempre que no haya ninguna tarjeta...
    // Mostramos el texto por la derecha
    for (int i = tam_texto; i > 0 ; i--)
    {
      String texto = texto_identificar.substring(i - 1);

      //Compruebo si han acercado una tarjeta
      if (mfrc522.PICC_IsNewCardPresent() || mfrc522.PICC_ReadCardSerial()) {
        return;
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(texto);
      delay(VELOCIDAD);
    }

    // Desplazamos el texto hacia la izquierda
    for (int i = 1; i <= 16; i++)
    {
      //Compruebo si han acercado una tarjeta
      if (mfrc522.PICC_IsNewCardPresent() || mfrc522.PICC_ReadCardSerial()) {
        return;
      }
      lcd.clear();
      lcd.setCursor(i, 0);
      lcd.print(texto_identificar);
      delay(VELOCIDAD);
    }

    // Desplazamos el texto hacia la izquierda  de nuevo
    for (int i = 16; i >= 1; i--)
    {
      //Compruebo si han acercado una tarjeta
      if (mfrc522.PICC_IsNewCardPresent() || mfrc522.PICC_ReadCardSerial()) {
        return;
      }
      lcd.clear();
      lcd.setCursor(i, 0);
      lcd.print(texto_identificar);
      delay(VELOCIDAD);
    }

    // Mostramos salida del texto por la derecha
    for (int i = 1; i <= tam_texto ; i++)
    {
      //Compruebo si han acercado una tarjeta
      if (mfrc522.PICC_IsNewCardPresent() || mfrc522.PICC_ReadCardSerial()) {
        return;
      }
      String texto = texto_identificar.substring(i - 1);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(texto);
      delay(VELOCIDAD);
    }
  }
}
