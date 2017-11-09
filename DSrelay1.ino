// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2
#define contour_banya 13//pin d13
#define contour_Buderus 12 //pin d12

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
/*class Contour{
  friend class DallasTemperature;
  float ContTemp;
  int SetPoint;
  unsigned long previousMillis; 
  DeviceAddress adr;
  //DeviceAddress addr;//адрес датчика контроля контура
  public:


  static int contourCount;
  Contour(
  DeviceAddress addr,//адрес датчика контроля контура
  int IO,//адрес исполнительного механизма - реле, насоса, реле и насоса, например - D12 - pin 12
  int ContourLength, //длина контура, влияет на скорость прокачивания контура, труба 20мм, внутренний диаметр 12, объём на метр = 0,012 м3 воды
  int BuildClass,//класс энергоэффективности здания
  //пусть классы энергоэффективности делятся от 0 до 5, 5 - наивысший класс, 0 - наинизший
 
  int HeatDissipValue,//сколько секций какой мощности (1.8 ватт на каждую секцию в здании класса BuildClass) Heat Dissipation Value
  int TempMax, //максимальная температура контура  int TempMax, //максимальная температура контура
  int TemperatureHyst,//гистерезис допустимых температур, например - дома с 7 до 8 температура может быть от (TempMax=)24 до 22 *С, гистерезис = 2*С
  int TempEnSaving,//температура энергосбережения, например - после 8 утра допустимая температура м.б. 18 градусов
  int HeatCapacity,//теплоёмкость теплоносителя, для воды = 4183 Дж/(кг·град)
  int Priority//приоритет, дом - наивысший приоритет - наибольшее число
  ){
    contourCount++;//increase contour counter
    * adr = addr;
  }
  void Update(){
    unsigned long currentMillis = millis();
//    ContTemp = DallasTemperature::getTempC(adr);
    //pinMode(A5, INPUT);
//    if (ContTemp>0)
    previousMillis = currentMillis;
  }
  private:
  int ActiveTime;//сколько времени был активен контур
  int ToBeActive;//ожидаемое время активности контура
   
};
int Contour::contourCount = 0;//счётчик контуров обнулен
*/
// Pass our oneWire reference to Dallas Temperature. 


/*
 * The setup function. We only start the sensors here
 * логика работы контуров
 * если в доме <23 выключаем насос, закрываем клапан
 * если в бане <23 включаем насос, открываем клапан
 * приоритет дом
 * если нет сиг с датчика в доме (1) открываем клапан, выключаем насос
 * если нет сиг с датчика бани (0) открываем клапан, выключаем насос
 * 
 * 
 * 
 */
 DeviceAddress insideThermometer, outsideThermometer, banyaThermometer, obratkaThermometer;//only for NotFound condition
 int PreviousDeviceCount = 0;//to check if device count has changed during loop
 int sensorValue = 0; 
 enum contour{RUN = LOW, STOP = HIGH };
 float House, Banya, Street, Obratka;

 DeviceAddress HouseAdr   = {0x28, 0x1E, 0x84, 0xDC, 0x04, 0x00, 0x00, 0x0F};
 DeviceAddress BanyaAdr   = {0x28, 0x8E, 0xF4, 0xA9, 0x04, 0x00, 0x00, 0xFA};
 DeviceAddress StreetAdr  = {0x28, 0xC8, 0x22, 0x3A, 0x03, 0x00, 0x00, 0xEC};
 DeviceAddress ObratkaAdr = {0x28, 0x87, 0x30, 0x3A, 0x03, 0x00, 0x00, 0x42};
 Contour HouseC(HouseAdr, contour_Buderus, 12/*адрес ножки контроллера*/, 3/*энергоэффективность*/, 16 /*секций*/,23/*макс.темп.контура*/,2/*hysteresis*/,18/*min.Temp*/,4183,10);
void setup(void)
{
  //pinMode(RXLED, OUTPUT);  // Set RX LED as an VALVE STATE LED - no serial!
  // start serial port
  Serial.begin(9600);
  //while (!Serial) ;
    // Start up the library
  sensors.begin();  // Start up the library
  sensors.setResolution(11);

  Serial.println("Dallas Temperature IC Control Library Demo");
  pinMode(contour_banya, OUTPUT);
  pinMode(contour_Buderus, OUTPUT);
  digitalWrite(contour_banya, STOP);
  digitalWrite(contour_Buderus, STOP);
  pinMode(A5, INPUT);
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");
// search for devices on the bus and assign based on an index.
  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Датчик дома 0 не найден"); 
  if (!sensors.getAddress(outsideThermometer, 1)) Serial.println("Датчик внешний 1 не найден"); 
  if (!sensors.getAddress(banyaThermometer, 2)) Serial.println("Датчик температуры бани 2 не найден"); 
  if (!sensors.getAddress(obratkaThermometer, 3)) Serial.println("Датчик температуры обратки 3 не найден"); 

}

/*
 * Main function, get and show the temperature
 */

void MapSensorsToNames(){
  House   = sensors.getTempC(HouseAdr);
  Banya   = sensors.getTempC(BanyaAdr);
  Street  = sensors.getTempC(StreetAdr);
  Obratka = sensors.getTempC(ObratkaAdr);
}


void loop(void)
{ 
  sensorValue = analogRead(A5);//punp current value
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  //Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  //Serial.println("DONE");
  delay(1000);
  // After we got the temperatures, we can print them here.
  MapSensorsToNames();//
  //Serial.print("Внутренняя температура  ");
  //Serial.print(sensors.getAddress(insideThermometer, 0));
  Serial.print(House);  // sensor
  Serial.print(',');//comma for separated grapning
  Serial.print(Banya); // sensor
  Serial.print(',');
  Serial.print(Street);
  Serial.print(',');
  Serial.print(Obratka);
  Serial.print(',');
  Serial.print(digitalRead(contour_banya)*50+5);
  Serial.print(',');
  Serial.println(digitalRead(contour_Buderus)*50+5);//Активация контура котла, горелки
  
  if ((Banya<22)&&(House>23)){
    //we should heat the banya
    digitalWrite(contour_banya, RUN);//open 
    digitalWrite(contour_Buderus, RUN); //stop pump - не используется, пока нет насоса на контур дома
    } 

  if (House<22){
    digitalWrite(contour_banya, STOP);//close valve
    digitalWrite(contour_Buderus, RUN); //stop pump не используется, пока нет второго насоса на контур дома
   } 
   if (Banya>23){
     digitalWrite(contour_banya, STOP); //stop pump, температуру регулирует котельная автоматика, пока нет насоса - контур не останавливаем.
   }
   
   if ((Banya>22)&&(House>23)){
     digitalWrite(contour_Buderus, STOP);
   }
   
   
}

