
#include <stdio.h>
#include <math.h>
#include "environmentals.h"

float calculateDewPoint(const float temperature, const float humidity)
{
    int t = round(temperature);
    int h = round(humidity);
    if (t < -45 || t > 65 || h == 0) {
        return NAN;
    }

    float a = 17.62;
    float b = 243.12;

    /**
     * Magnus-Tetens formula (Sonntag90):
     *
     * Ts = (bα(T,RH)) / (a - α(T,RH))
     *
     * Ts is the dew point;
     * T is the temperature;
     * RH is the relative humidity of the air;
     * a and b are coefficients. For Sonntag90 constant set, a = 17.62
     * and b = 243.12°C;
     * α(T,RH) = ln(RH/100) + aT/(b+T)
     */
    float alpha = log(humidity / 100) + a * temperature / (b + temperature);
    float dewPoint = (b * alpha) / (a - alpha);

    return dewPoint;
}

float calculateVPD(const float temperature, const float humidity)
{
    //SVP = 610.78 x e^(T / (T +237.3) x 17.2694)
    float svp = 610.78 * exp(temperature / (temperature + 237.3) * 17.2694);
    //SVP x (1 – RH/100) = VPD
    //svp is divided by 1000 to get vpd in kPa
    float vpd = (svp/1000) * (1 - humidity/100);
    return vpd;
}