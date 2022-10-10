#include <iostream>
#include <iomanip> // pra funcao std::setprecision() quando usar std::cout
#include <thread>
#include <string> // conversao de string pra double
#include <typeinfo>
#include "SerialPort.h"

#define SLEEP_TIME 500
#define COUT_PRECISION 2

using namespace std;

char inputData[INPUT_DATA_BYTES];
double inputValDbl = 0.0;

char comport[] = "COM3";
char *port = comport;


int main(){
  
  SerialPort arduino(port);
  char command[] = "0";

  if(arduino.isConnected())
    cout << "Connected to port " << port << endl;
  else
    cout << "Conect the device to port " << port << endl; 
  while(arduino.isConnected()){
  /* Le a string de dados InputData do dispositivo que deve se parecer com : "60.00\r\n", que possui 7 caracteres
     o caracter \r significa "carriage return", que volta o cursor para o inicio da linha
     o caracter \n sigifica "new line", que muda o cursor para a linha abaixo*/
    arduino.ReadSerialPort(inputData, 3);
    //Converte char[] para std::string
    string inputValStr(inputData);
    //Converte a string em um double
    //if(inputValStr.size() > 6)
      //inputValDbl = stod(inputValStr);

    //cout << fixed << setprecision(COUT_PRECISION);
    cout << inputValStr <<'\r';
    printf("Data received= %s\n", inputData);
    
    cin >> command;
    if(command[0] == '5'){
      cout << "Terminated\n";
      break;
    }
    arduino.WriteSerialPort(command, 1);
    Sleep(SLEEP_TIME);
  }
  return 0;
}