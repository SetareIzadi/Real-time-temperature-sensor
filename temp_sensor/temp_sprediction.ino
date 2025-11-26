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

  // Perform non-linear exponential fit every 10 samples
  if (dataCount >= 10 && nr % 10 == 0) {
    float A, B;
    exponentialFit(tempData, dataCount, A, B);  // now it became an exponential fit.

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

// Nichtlineare Regression für y = A * exp(B * x)
// Gauss-Newton-Verfahren mit ein paar Iterationen
void exponentialFit(float *y, int n, float &A, float &B) {
  if (n < 2) {
    A = y[0];
    B = 0.0;
    return;
  }

  // 1) initialize beginning values
  float y0 = (y[0] > 0) ? y[0] : 1e-6;
  float yn = (y[n - 1] > 0) ? y[n - 1] : 1e-6;

  // x = 0 ... n-1
  float x0 = 0.0;
  float xn = (float)(n - 1);

  // Rough estimation for B
  B = log(yn / y0) / (xn - x0);
  A = y0;  // bei x0 = 0 ist A ~ y(0)

  const int maxIter = 8;   // Number of gauß-newton steps
  const float lambda = 1.0; // dampening factor (1.0 = step width)

  for (int iter = 0; iter < maxIter; iter++) {
    float S_aa = 0.0f;
    float S_ab = 0.0f;
    float S_bb = 0.0f;
    float g_a  = 0.0f;
    float g_b  = 0.0f;

    for (int i = 0; i < n; i++) {
      float xi = (float)i;
      float yi = (y[i] > 0) ? y[i] : 1e-6;  // kleine Absicherung

      float eBx   = exp(B * xi);
      float model = A * eBx;
      float ri    = yi - model;            // Residuum

      // Ableitungen von r_i nach A und B
      float dA = -eBx;
      float dB = -A * xi * eBx;

      // Aufbau von J^T * J
      S_aa += dA * dA;
      S_ab += dA * dB;
      S_bb += dB * dB;

      // Aufbau von J^T * r
      g_a  += dA * ri;
      g_b  += dB * ri;
    }

    float det = S_aa * S_bb - S_ab * S_ab;
    if (fabs(det) < 1e-12) {
      // If the system is bad => abbort
      break;
    }

    // Löse 2x2-System:
    // [S_aa S_ab] [dA] = [g_a]
    // [S_ab S_bb] [dB]   [g_b]
    float dA = ( g_a * S_bb - g_b * S_ab) / det;
    float dB = ( S_aa * g_b - S_ab * g_a) / det;

    // updated dampening factor
    A += lambda * dA;
    B += lambda * dB;

    // 
    if (A <= 0.0f) {
      A = 1e-6;
    }
  }
}