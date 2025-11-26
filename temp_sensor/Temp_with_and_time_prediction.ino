#include <math.h>

#define MAX_POINTS 50

float tempData[MAX_POINTS];
unsigned long timeData[MAX_POINTS];
int dataCount = 0;
int headIndex = 0;
int nr = 0;  // sample counter

float setPoint = 20.0;  // °C

void setup() {
  Serial.begin(9600);
}

// ---------------- SENSOR READ ----------------
float Read_NTC10k_1() {
  float a = 639.5, b = -0.1332, c = -162.5;
  float Rntc, Vntc, Temp;
  Vntc = (analogRead(A0) * 5.0) / 1023.0;
  if (Vntc < 0.001) return NAN;  // avoid division by zero
  Rntc = 10000.0 * ((5.0 / Vntc) - 1);
  Temp = a * pow(Rntc, b) + c;
  return Temp;
}

float Read_NTC10k_2() {
  float a = 639.5, b = -0.1332, c = -162.5;
  float Rntc, Vntc, Temp;
  Vntc = (analogRead(A1) * 5.0) / 1023.0;
  if (Vntc < 0.001) return NAN;
  Rntc = 10000.0 * ((5.0 / Vntc) - 1);
  Temp = a * pow(Rntc, b) + c;
  return Temp;
}

// ---------------- MOVING AVERAGE ----------------
void smoothData(float *raw, int n, int window, float *smoothed) {
  for (int i = 0; i < n; i++) {
    float sum = 0;
    int count = 0;
    for (int j = i - window/2; j <= i + window/2; j++) {
      if (j >= 0 && j < n) {
        sum += raw[j];
        count++;
      }
    }
    smoothed[i] = sum / count;
  }
}

// ---------------- EXPONENTIAL FIT ----------------
void exponentialFit(float *y, unsigned long *t, int n, float &A, float &B) {
  if (n < 2) { A = 0; B = 0; return; }

  double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
  for (int i = 0; i < n; i++) {
    double xi = t[i] / 1000.0;  // convert ms → seconds
    if (y[i] <= 0) continue;
    double yi = log(y[i]);
    sumX += xi;
    sumY += yi;
    sumXY += xi * yi;
    sumX2 += xi * xi;
  }

  double denom = (n * sumX2 - sumX * sumX);
  if (fabs(denom) < 1e-9) { A = 0; B = 0; return; }

  B = (n * sumXY - sumX * sumY) / denom;
  double lnA = (sumY - B * sumX) / n;
  A = exp(lnA);
}

// ---------------- MAIN LOOP ----------------
void loop() {
  // Read sensor
  float TempC_1 = Read_NTC10k_1();

  // Store in rolling buffer
  unsigned long now = millis();
  tempData[headIndex] = TempC_1;
  timeData[headIndex] = now;
  headIndex = (headIndex + 1) % MAX_POINTS;
  if (dataCount < MAX_POINTS) dataCount++;

  // Print current temperature every second
  Serial.print("Sample ");
  Serial.print(nr);
  Serial.print(" | Temp: ");
  Serial.print(TempC_1);
  Serial.println(" °C");

  // Perform exponential fit every 10 samples
  if (dataCount >= 10 && nr % 10 == 0) {
    float smoothed[MAX_POINTS];
    smoothData(tempData, dataCount, 5, smoothed);  // 5-point moving average

    float A, B;
    exponentialFit(smoothed, timeData, dataCount, A, B);

    // Predict next 5 seconds
    Serial.println("Predicted future temperatures:");
    for (int s = 1; s <= 5; s++) {
      float pred = A * exp(B * ((now + s * 1000) / 1000.0));
      Serial.print("t+");
      Serial.print(s);
      Serial.print("s: ");
      Serial.println(pred);
    }

    // Estimate time to reach setpoint
    if (setPoint > 0 && A > 0 && fabs(B) > 1e-6) {
      float targetTime = (log(setPoint / A)) / B;  // in seconds
      float secondsRemaining = targetTime - (now / 1000.0);
      if (secondsRemaining > 0) {
        Serial.print("Estimated time to reach ");
        Serial.print(setPoint);
        Serial.print(" °C: ");
        Serial.print(secondsRemaining);
        Serial.println(" seconds");
      } else {
        Serial.println("Setpoint already reached or prediction not valid.");
      }
    }
  }

  delay(1000);  // keep 1s sampling
  nr++;         // increment sample counter
}
