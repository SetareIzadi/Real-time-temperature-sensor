
#include <math.h>

#define MAX_POINTS   50           // rolling buffer length
#define WINDOW_SIZE   5           // moving average window (odd recommended)

// --- Buffers ---
float         tempData[MAX_POINTS];
unsigned long timeData[MAX_POINTS];
int           dataCount   = 0;
int           headIndex   = 0;
int           nr          = 0;     // sample counter

// --- Control / display ---
float setPoint = 20.0f;            // °C

void setup() {
  Serial.begin(9600);
  while (!Serial) { /* wait for Serial on some boards */ }
  Serial.println("Starting temperature monitor…");
}

// ---------------- SENSOR READ ----------------
float Read_NTC10k_1() {
  // Calibration for 10k NTC (example coefficients)
  const float a = 639.5f, b = -0.1332f, c = -162.5f;

  float Vntc = (analogRead(A0) * 5.0f) / 1023.0f;
  if (Vntc < 0.001f) return NAN;           // avoid division by near-zero

  float Rntc = 10000.0f * ((5.0f / Vntc) - 1.0f);
  float Temp = a * powf(Rntc, b) + c;
  return Temp;
}

// ---------------- MOVING AVERAGE ----------------
void smoothData(const float *raw, int n, int window, float *smoothed) {
  if (window < 1) window = 1;
  int half = window / 2;                    // for odd window sizes

  for (int i = 0; i < n; i++) {
    double sum = 0.0;
    int count = 0;
    for (int j = i - half; j <= i + half; j++) {
      if (j >= 0 && j < n) {
        sum += raw[j];
        count++;
      }
    }
    smoothed[i] = (count > 0) ? (float)(sum / count) : raw[i];
  }
}

// ---------------- EXPONENTIAL FIT ----------------
// Fits ln(y) = ln(A) + B * x, where x is time in seconds *relative to t[0]*.
// Returns true on success and sets A, B, R2 (goodness of fit in log-space).
bool exponentialFit(const float *y, const unsigned long *t, int n,
                    float &A, float &B, float &R2) {
  if (n < 2) { A = 0; B = 0; R2 = 0; return false; }

  // Use times relative to the oldest sample for numerical stability
  const double t0 = (double)t[0];

  // First pass: accumulate for linear regression in log-space
  int m = 0;           // number of valid points (y > 0)
  double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;

  for (int i = 0; i < n; i++) {
    if (y[i] <= 0 || isnan(y[i])) continue;            // skip invalid
    double xi = ((double)t[i] - t0) / 1000.0;          // seconds since t0
    double yi = log((double)y[i]);                     // ln(y)
    sumX  += xi;
    sumY  += yi;
    sumXY += xi * yi;
    sumX2 += xi * xi;
    m++;
  }

  if (m < 2) { A = 0; B = 0; R2 = 0; return false; }

  const double denom = (m * sumX2 - sumX * sumX);
  if (fabs(denom) < 1e-12) { A = 0; B = 0; R2 = 0; return false; }

  double B_d  = (m * sumXY - sumX * sumY) / denom;
  double lnA  = (sumY - B_d * sumX) / m;
  A = (float)exp(lnA);
  B = (float)B_d;

  // Second pass: R^2 in log-space
  double meanY = sumY / m;
  double SSR = 0.0;   // sum of squared residuals
  double SST = 0.0;   // total sum of squares

  for (int i = 0; i < n; i++) {
    if (y[i] <= 0 || isnan(y[i])) continue;
    double xi = ((double)t[i] - t0) / 1000.0;
    double yi = log((double)y[i]);
    double yhat = lnA + B_d * xi;
    SSR += (yi - yhat) * (yi - yhat);
    SST += (yi - meanY) * (yi - meanY);
  }

  R2 = (SST > 1e-15) ? (float)(1.0 - SSR / SST) : 0.0f;
  return true;
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
  Serial.print(TempC_1, 3);
  Serial.println(" °C");

  // Perform prediction + print A, B, R^2 every 30 samples (≈30 seconds)
  if (dataCount >= 30 && (nr % 30 == 0)) {

    // 1) Build chronological buffers (oldest -> newest) from the circular buffer
    float chronTemp[MAX_POINTS];
    unsigned long chronTime[MAX_POINTS];

    int start = (headIndex - dataCount + MAX_POINTS) % MAX_POINTS;
    for (int k = 0; k < dataCount; k++) {
      int idx = (start + k) % MAX_POINTS;
      chronTemp[k] = tempData[idx];
      chronTime[k] = timeData[idx];
    }

    // 2) Smooth temperatures (moving average)
    float smoothed[MAX_POINTS];
    smoothData(chronTemp, dataCount, WINDOW_SIZE, smoothed);

    // 3) Fit exponential model in log-space using *relative time*
    float A, B, R2;
    bool ok = exponentialFit(smoothed, chronTime, dataCount, A, B, R2);

    // 4) Report fit parameters (A, B, R^2)
    if (ok && A > 0 && fabs(B) > 1e-9) {
      Serial.print("Fit params: A = ");
      Serial.print(A, 6);           // A ~ °C (at x=0, oldest sample)
      Serial.print(", B = ");
      Serial.print(B, 6);           // 1/s
      Serial.print(" (1/s), R^2 = ");
      Serial.println(R2, 4);
    } else {
      Serial.println("Fit params invalid or insufficient data.");
    }

    // 5) Predict temperature 30 seconds from *now* using the same origin
    //    x = seconds since chronTime[0]
    double x_now      = ((double)now - (double)chronTime[0]) / 1000.0; // seconds since t0
    double x_future   = x_now + 30.0;                                   // +30 seconds
    float  pred30     = (ok) ? (A * exp(B * x_future)) : NAN;

    Serial.print("Predicted temperature after 30 seconds: ");
    if (isnan(pred30)) {
      Serial.println("N/A");
    } else {
      Serial.println(pred30, 3);
    }

    // 6) Estimate time to reach setpoint (seconds from chronTime[0])
    if (ok && setPoint > 0 && A > 0 && fabs(B) > 1e-9) {
      double targetX = (log((double)setPoint / (double)A)) / (double)B;  // seconds since t0
      double secondsRemaining = targetX - x_now;
      if (secondsRemaining > 0 && isfinite(secondsRemaining)) {
        Serial.print("Estimated time to reach ");
        Serial.print(setPoint, 2);
        Serial.print(" °C: ");
        Serial.print(secondsRemaining, 2);
        Serial.println(" s");
      } else {
        Serial.println("Setpoint already reached or prediction not valid.");
      }
    }
  }

  // Keep 1 s sampling cadence
  delay(1000);
  nr++;
}
