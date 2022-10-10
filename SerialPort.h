#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define ARDUINO_WAIT_TIME 2000
#define INPUT_DATA_BYTES 7


class SerialPort
{
  private:
    HANDLE handleToCom;
    bool connected;
    COMSTAT status;
    DWORD errors;
    COMMTIMEOUTS timeouts;
  public:
    SerialPort(char* portName){
      errors = 0;
      status = {0};
      connected = false;
      timeouts.ReadIntervalTimeout = 0;
      timeouts.ReadTotalTimeoutMultiplier = 0;
      timeouts.ReadTotalTimeoutConstant = 1;
      timeouts.WriteTotalTimeoutMultiplier = 0;
      timeouts.WriteTotalTimeoutConstant = 0;
      

      //Cria e abre o dispositivo COM I/O com a funcao Create File. Isso retorna um manipulador para o dispositivo COM (porta serial)
      handleToCom = CreateFileA(static_cast<LPCSTR> (portName), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
      // Usa a funcao da API do Win32 para pegar um codigo de erro
      DWORD errMsg = GetLastError();

      //Configura parametros de tempo de leitura da funcao ReadFile
      SetCommTimeouts(handleToCom, &timeouts);

      // Codigo para dispositivo nao encontrado DWORD = 2
      if(errMsg == 2)
        printf("Tem que ligar o trem no PC primeiro meu consagrado\n");
      
      // Codigo para dispositivo ja sendo usado (outro aplicativo utilizando a porta COM acessada) DWORD = 5
      else if(errMsg == 5)
        printf("Tem outro APP usando essa porta, feche ou mude de porta\n");
      // Verifiquei so esses dois erros, agora se nao tiver nenhum errMsg = 0, que e o caso q usaremos pra porosseguir
      else if(!errMsg){
        //DCB (Device Control Block) eh um tipo de dado da API do Win32 que tem os dados e configuracoes da porta COM
        DCB dcbSerialParameters = { 0 };

        //Utiliza a funcao da API GetCommState para verificar se conseguiu acessar os dados da porta
        if(!GetCommState(handleToCom, &dcbSerialParameters))
          printf("Falha em acessar os parametros da porta serial\n");

        //Caso esteja tudo certo, configura alguns parametros dcb
        else{
          dcbSerialParameters.BaudRate = CBR_9600;
          dcbSerialParameters.ByteSize = 8;
          dcbSerialParameters.StopBits = ONESTOPBIT;
          dcbSerialParameters.Parity = NOPARITY;
          dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;

          //Verifica se as configuracoes foram realizadas com sucesso
          if(!SetCommState(handleToCom, &dcbSerialParameters))
            printf("ALERTA: Nao foi possivel configurar os parametros da porta serial\n");

          else{
            connected = true;
            PurgeComm(handleToCom, PURGE_RXCLEAR | PURGE_TXCLEAR);
            Sleep(ARDUINO_WAIT_TIME);
          }
        }

      }

    }
    ~SerialPort(){
      //Destructor. Verifica se ha alguma porta COM ligada e encerra a conexao destruindo o manipulador (handle)
      if(connected){
        connected = false;
        CloseHandle(handleToCom);
      }

    }

    int ReadSerialPort(char* buffer, unsigned int buf_size){
      DWORD bytesRead;
      unsigned int toRead = 0;

      //Pega informacao da porta e limpa as flag de erro obtidas
      ClearCommError(handleToCom, &errors, &status);

      //Descobre quantos bytes estao na fila para serem lidos pelo metodo ReadFile()
      //
      //cbInQue eh o numero de bytes recebidos pela port serial (no buffer) mas ainda nao lidos
      //pelo metodo ReadFile()
      //
      //Estamos lendo apenas 7 bytes por vez, essa operacao verifica se ha mais ou menos bytes
      //a serem lidos no buffer para nao exceder a quantidade desejada
      if(status.cbInQue > 0)
        toRead = buf_size;
      else
        toRead = status.cbInQue;
      
      //Usa o metodo ReadFile para ler a quantidade solicitada de bytes do buffer(toRead) e retorna o numero de bytes
      if(ReadFile(handleToCom, buffer, toRead, &bytesRead, NULL))
        return bytesRead;
      
      return 0;
    }
    int WriteSerialPort(char* buffer, unsigned int buf_size){

      DWORD writtenBytes;
      unsigned int toWrite = 0;
      //Pega informacao da porta e limpa as flag de erro obtidas
      ClearCommError(handleToCom, &errors, &status);

      //Verifica se ha dados para serem escritos ja na fila, caso nao, escreve o conteudo passado no buffer
      
        if(status.cbOutQue > 0)
        	toWrite = status.cbOutQue;
      	else
	  		toWrite = buf_size;
      
      //escreve no buffer usando a funcao WriteFile
      if(WriteFile(handleToCom, buffer, toWrite, &writtenBytes, NULL))
        return writtenBytes;
      
      return 0;
    }
    bool isConnected(){
      return connected;
    }
};