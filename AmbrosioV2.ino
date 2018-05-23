#include <SPI.h>
#include <Ethernet.h>
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 151);
IPAddress server(192, 168, 1, 150);
EthernetClient client;
#include <Servo.h> 
Servo myservo; 

#define dirPin1 7
#define dirPin2 8
#define step1 6
#define step2 9
#define sensorcalibrado A1
#define servopin 5
#define S2 3
#define S3 4
#define outPin 2 // Salida del sensor
#define sensor_led A0
int sensorDelay = 1; //tiempo para que el sensor tome su lectura y se estabilice   
double r=0;
double g=0;
double b=0;
double w=0;

int contadornada=0;
int control=0;
char temporaldatosrecibidos = 0;
int controlembudo = 0;

#define ROJO    1
#define VERDE   2
#define AZUL    3
#define BLANCO  4
#define NADA    5


double colorRead(int color, boolean LEDstate)
{ 
    
    //sélectiona el filtro 
    if(color == 0)
    {//clear
    digitalWrite(S3, LOW); //S3 
    digitalWrite(S2, HIGH); //S2 
    }
    else if(color == 1)
    {//red 
    digitalWrite(S3, LOW); //S3 
    digitalWrite(S2, LOW); //S2 
    }
    else if(color == 2)
    {//blue 
    digitalWrite(S3, HIGH); //S3 
    digitalWrite(S2, LOW); //S2 
    }
    else if(color == 3)
    {//green 
    digitalWrite(S3, HIGH); //S3 
    digitalWrite(S2, HIGH); //S2 
    } 
    
    double readPulse; 


    if(LEDstate == 0)
    {
      digitalWrite(sensor_led, LOW);
    }

    if(LEDstate == 1)
    {
      digitalWrite(sensor_led, HIGH);
    }


    
    // lee el pulso 
    delay(sensorDelay); 
    readPulse = pulseIn(outPin, LOW, 80000); 
    //si el tiempo es mayor de lo que lee pulsein regresa cero 
    if(readPulse < .1){ readPulse = 80000; } 
    

    //digitalWrite(sensor_led, LOW);
    return readPulse; 



}


int compruebaColor()
{   

      w=0;
      r=0;
      g=0;
      b=0;


    for(int i=0; i<=9;  i++ )
      {
        w = w+(colorRead(0,1)*10); 
        r = r+(w/colorRead(1,1)*10);  // sacamos media
        g = g+(w/colorRead(3,1)*10); 
        b = b+(w/colorRead(2,1)*10); 
        
      }

      w=w/10;
      r=r/100;
      g=g/100;    // sacamos media
      b=b/100;
Serial.print("Blanco-->");
Serial.println(w);
Serial.print("Rojo-->");
Serial.println(r);
Serial.print("verde-->");
Serial.println(g);
Serial.print("azul-->");
Serial.println(b);

    if(w > 300)
      {
      
        Serial.println("NADA");

        contadornada++;
        return NADA;

      }   


     else if(w<115)
      {
      
        Serial.println("BLANCO!!!");

        contadornada=0;
        client.print(6);


        return BLANCO;
      }

      else if(r > b && r > g)
      {
      
        Serial.println("ROJO!!!");


        contadornada=0;
        client.print(4);

        return ROJO;
      }   
  
      else if(b > g && b > r)
      {
      
        Serial.println("AZUL");

        contadornada=0;
        client.print(5);


        return AZUL;

      }   

      


}


void step(boolean dir,int steps,int stepperpin, int dirstepper)
{
  digitalWrite(dirstepper,dir);
  delay(50);
    
    for(int i=0;i<steps;i++)
    {
      digitalWrite(stepperpin, HIGH);
      delayMicroseconds(300);
      digitalWrite(stepperpin, LOW);
      delayMicroseconds(300);
    }
}


void calibrar ()
{
      while(analogRead(sensorcalibrado) < 750)
      {
              step(false,10,step1,dirPin1);


      }

  step(true,10,step1,dirPin1);
  delay(500);




}

void clasificacion(int colordetectado)
  {
      if(colordetectado==ROJO)

      
      {

        if(controlembudo==0)
        {
             
         
          step(true,160,step2,dirPin2);
          step(false,320,step1,dirPin1);
          delay(1200);
          controlembudo=1;


        }

        else if(controlembudo==1)
        {
             
        step(false,320,step1,dirPin1);
        delay(900);
        controlembudo=1;


        }


        else if(controlembudo==2)
        {
             
         
        step(true,320,step2,dirPin2);
        step(false,320,step1,dirPin1);
        delay(1200);
        controlembudo=1;

     
        }





      } 


      if(colordetectado==BLANCO)
      {

        if(controlembudo==0)
        {
            
          step(false,320,step1,dirPin1);
          delay(900);
          controlembudo=0;

         }

          else if(controlembudo==1)
        {
            
          step(false,160,step2,dirPin2);
          step(false,320,step1,dirPin1);
          delay(1200);
          controlembudo=0;

         }

          else if(controlembudo==2)
        {
            
          step(true,160,step2,dirPin2);
          step(false,320,step1,dirPin1);
          delay(1200);
          controlembudo=0;

         }


        

      }



      if(colordetectado==AZUL)
      {


        if(controlembudo==0)
        {
            
          step(false,160,step2,dirPin2);
          step(false,320,step1,dirPin1);
          delay(1200);
          controlembudo=2;

         }


         else if(controlembudo==1)
        {
            
          step(false,320,step2,dirPin2);
          step(false,320,step1,dirPin1);
          delay(1200);
          controlembudo=2;

         }


         else if(controlembudo==2)
        {
            
          step(false,320,step1,dirPin1);
          delay(900);
          controlembudo=2;

         }


      }


       if(colordetectado==NADA)
      {

        


        step(false,320,step1,dirPin1);

      }

  }


int sp=50;
void servoinit(bool control)
{
  
  if(control==true)
  {
    
    
    myservo.write(sp); 
    
      if (sp == 50) { sp=150; }
      else { sp=50; }
      
  }
 
}


void controldatosrecibidos()
{


    if(client.available())
    {
        temporaldatosrecibidos=client.read();
        
        Serial.print("Incoming data: ");
        Serial.println(temporaldatosrecibidos, DEC);

    }


    if(temporaldatosrecibidos=='1')
    {
        temporaldatosrecibidos=0;
        control=1;
    }

    else if(temporaldatosrecibidos=='2')
    {
        temporaldatosrecibidos=0;
        control=2;

    }

    else if(temporaldatosrecibidos=='3')
    {
        temporaldatosrecibidos=0;
        control=3;

    }



}


void conexionsocket() 
{

    while(!client.connected())
    { 
        Serial.println("No conectado. Intentando...");
        client.stop();
        client.connect(server, 5000);
        digitalWrite(sensor_led, HIGH);
        delay(150);
        digitalWrite(sensor_led, LOW);
        delay(150);
    }
    //Serial.println("Conectado!");

}


void setup() 
{
  
  Serial.begin(9600);
  
  Serial.println("Ethernet Begin");
  Ethernet.begin(mac, ip);
  myservo.attach(servopin);

  servoinit(false);

  pinMode(dirPin1, OUTPUT);
  pinMode(step1, OUTPUT);
  pinMode(dirPin2, OUTPUT);
  pinMode(step2, OUTPUT);
  pinMode(S2,OUTPUT);
  pinMode(S3,OUTPUT);
  pinMode(sensor_led,OUTPUT);
  pinMode(servopin, OUTPUT);

  Serial.println("Calibrar");
  calibrar();


}







void loop()
{

conexionsocket();
controldatosrecibidos();
    

    
    if(control==1)
    {
    
        servoinit(true);
        clasificacion(compruebaColor());


    }


    if(control==2)
    {

        servoinit(false);
        control=0;


    }


    if(control==3 || contadornada==10)
    {
        contadornada=0;
        control=0;
        servoinit(false);
        client.print(9);


        if(controlembudo==1)
        {
            
          step(false,160,step2,dirPin2);
          controlembudo=0;

         }

          if(controlembudo==2)
        {
            
          step(true,160,step2,dirPin2);
          controlembudo=0;

         }
    }



  
}
