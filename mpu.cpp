#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
Adafruit_MPU6050 mpu;
sensors_event_t a,g,t;
float mpu_Ax = 0.0f, mpu_Ay=0.0f, mpu_Az=0.0f;
float mpu_Gx=0.0f ,mpu_Gy=0.0f, mpu_Gz=0.0f;
float AngleX = 0.0f;
float AngleY = 0.0f;
float time_ = 0.0f;
float current_time = 0.0f;
float prev_time = 0.0f;
float Ax[10] = {};float Az[10] = {};float Ay[10] = {};
float Gx[10] = {};float Gz[10] = {};float Gy[10] = {};
float sumAX = 0.0f;float sumAY = 0.0f;float sumAZ = 0.0f;
float sumGX = 0.0f;float sumGY = 0.0f;float sumGZ = 0.0f;
float AX =0, AY =0, AZ =0, GX =0, GY =0, GZ =0;
int n = 2;

void calibrate()
{
    mpu.getEvent(&a,&g,&t);
    static float cax[100], cay[100], caz[100], cgx[100], cgy[100], cgz[100];
    float sumCAX = 0.0f;float sumCAY = 0.0f;float sumCAZ = 0.0f;
    float sumCGX = 0.0f;float sumCGY = 0.0f;float sumCGZ = 0.0f;
    for(int i =0; i < 100; i ++)
    {
      cax[i] = a.acceleration.x; cay[i] = a.acceleration.y; caz[i] = a.acceleration.z;
      cgx[i] = g.gyro.x; cgy[i] = g.gyro.y; cgz[i] = g.gyro.z;
      sumCAX += cax[i]; sumCAY += cay[i]; sumCAZ += caz[i];
      sumCGX += cgx[i]; sumCGY += cgy[i]; sumCGZ += cgz[i];
    }
    AX = sumCAX / 100.0f; AY = sumCAY / 100.0f; AZ = sumCAZ / 100.0f;
    GX = sumCGX / 100.0f; GY = sumCGY / 100.0f; GZ = sumCGZ / 100.0f;
    delay(10);
    
}

void setup()
{
  Serial.begin(115200);
  Wire.begin(21,18);    //SDA, SCL
  delay(20);
  if(!mpu.begin(0x68, &Wire))
  {
    Serial.println("Not connected!");
  }
  else 
  {
    Serial.println("Connected");
  }
  calibrate();
}

void loop() {
  
  current_time = millis();
  time_ = (current_time - prev_time) / 1000.0f;
  prev_time = current_time;
  for(int i = 0; i< n; i++)
  {
    mpu.getEvent(&a,&g,&t);
    Ax[i] = a.acceleration.x; Ay[i] = a.acceleration.y; Az[i] = a.acceleration.z;
    Gx[i] = g.gyro.x; Gy[i] = g.gyro.y; Gz[i] = g.gyro.z;
    sumAX += Ax[i]; sumAY += Ay[i]; sumAZ += Az[i];
    sumGX += Gx[i]; sumGY += Gy[i]; sumGZ += Gz[i];
  }
  
  float aX = sumAX / n; float aY = sumAY / n; float aZ = sumAZ / n;
  float gX = sumGX / n; float gY = sumGY / n; float gZ = sumGZ / n;
  aX = aX - AX; aY = aY - AY; //aZ = aZ - AZ; 
  gX -= GX; gY -= GY; gZ -= GZ;

  //AngleX = (0.98 * (AngleX + gX * time_) + 0.02 * atan2(aY,sqrt((aX*aX)+(aZ*aZ))));
  //AngleY = (0.98 * (AngleY + gY * time_) + 0.02 * atan2(aX,sqrt((aZ*aZ)+(aY*aY))));
  AngleX = (0.98 * (AngleX + gX * time_) + 0.02 * atan2(aY,aZ));
  AngleY = (0.98 * (AngleY + gY * time_) + 0.02 * atan2(aX,aZ));


  //Serial.print(String("AngleX: ")+ ((AngleX*180)/PI));Serial.println(String("|AngleY: ")+ (AngleY*180)/PI);
  Serial.print(AngleX); Serial.print(",");
  Serial.print(AngleY); Serial.print(",");
  Serial.println(gZ);

  //time = 0.0f;
  sumAX = 0.0f; sumAY = 0.0f; sumAZ = 0.0f; sumGX = 0.0f; sumGY = 0.0f; sumGZ = 0.0f;
  for(int i = 0; i<10; i++)
  {
    Ax[i] = 0.0f; Ay[i] = 0.0f; Az[i] = 0.0f;
    Gx[i] = 0.0f; Gy[i] = 0.0f; Gz[i] = 0.0f; 
  }
}