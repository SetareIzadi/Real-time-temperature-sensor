#include <math.h>

float TempC_1, TempC_2;
int nr = 0;

#define MAX_POINTS 50
float tempData[MAX_POINTS];
int dataCount = 0;

// User-defined setpoint temperature
float setPoint = 25.0;  // °C

void setup() {
  Serial.begin(9600);
}

void loop() {
  TempC_1 = Read_NTC10k_1();
  TempC_2 = Read_NTC10k_2();

  Serial.print("Number: ");
  Serial.print(nr);
  Serial.print("\t");
  Serial.print(TempC_1);
  Serial.println("°C \t");
  //Serial.println(TempC_2);

  if (dataCount < MAX_POINTS) {
    tempData[dataCount] = TempC_1;
    dataCount++;
  }

  // Perform exponential fit every 10 samples
  if (dataCount >= 10 && nr % 10 == 0) {
    float A, B;
    exponentialFit(tempData, dataCount, A, B);

    Serial.println("Predicted future temperatures:");
    for (int i = dataCount; i < dataCount + 5; i++) {
      float pred = A * exp(B * i);
      Serial.print("t+");
      Serial.print(i - dataCount + 1);
      Serial.print(": ");
      Serial.println(pred);
    }

    // Calculate time to reach setpoint
    if (setPoint > 0 && A > 0 && B != 0) {
      float n_target = (log(setPoint / A)) / B;
      float seconds_remaining = (n_target - dataCount);  // relative to current index
      if (seconds_remaining > 0) {
        Serial.print("Estimated time to reach ");
        Serial.print(setPoint);
        Serial.print("°C: ");
        Serial.print(seconds_remaining);
        Serial.println(" seconds");
      } else {
        Serial.println("Setpoint already reached or prediction not valid.");
      }
    }
  }

  delay(1000);
  nr += 1;
}

float Read_NTC10k_1() {
  float a = 639.5, b = -0.1332, c = -162.5;
  float Rntc, Vntc, Temp;
  Vntc = (analogRead(A0) * 5.0) / 1023.0;
  Rntc = 10000.0 * ((5.0 / Vntc) - 1);
  Temp = a * pow(Rntc, b) + c;
  return Temp;
}

float Read_NTC10k_2() {
  float a = 639.5, b = -0.1332, c = -162.5;
  float Rntc, Vntc, Temp;
  Vntc = (analogRead(A1) * 5.0) / 1023.0;
  Rntc = 10000.0 * ((5.0 / Vntc) - 1);
  Temp = a * pow(Rntc, b) + c;
  return Temp;
}

void exponentialFit(float *y, int n, float &A, float &B) {
  float sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
  for (int i = 0; i < n; i++) {
    float xi = i;
    float yi = log(y[i] > 0 ? y[i] : 1e-6);  // avoid log(0)
    sumX += xi;
    sumY += yi;
    sumXY += xi * yi;
    sumX2 += xi * xi;
  }

  B = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
  float lnA = (sumY - B * sumX) / n;
  A = exp(lnA);
}

