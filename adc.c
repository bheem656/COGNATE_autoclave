// #include <SPI.h>

// #include <Arduino.h>



// int fullScale = 9630; // max pressure (span) adjust

// // variables that required to convert voltage into resistance
// float C = -268.55;    ////-319; //-326; //Constant of straight line (Y = mx + C)
// float slope = 871.11; // 851//883.34; // Slope of straight line (Y = mx + C)
// float R0 = 1000;
// float alpha = 0.00385;

// float TS1(void)
// {

//   float V11 = 0;
//   float ts11 = 0;
//   float Rx11 = 0;
//   float temp11 = 0;

//   curr_time = millis();
//   for (int i = 0; i < sample; i++)
//   {
//     ts11 += analogRead(rtd2);

//     // while (millis() - curr_time <= sample_time)
//     // {
//     //   /* code */
//     // }
//     delay(10);
//   }

//   ts11 /= sample;
//   V11 = (ts11 / 1023.0) * 5.0; // (bits/2^n-1)*Vmax
//                                // // Serial1.print("VOLATAGE : ");
//                                // // Serial1.println(V11);
//                                //  Rx11 = 1000 * ((2.17 * V11) / (5 - V11)); // 2.18
//   Rx11 = V11 * slope + C;      // y=mx+c

//   // // Serial1.print("resistance : ");
//   // // Serial1.println(Rx11);

//   temp11 = ((Rx11 / R0) - 1) / alpha;

//   // float C = 88;
//   // float slope = 14.187;
//   // float R0 = 100.0;
//   // float alpha = 0.00385;

//   // Rx11 = V11*slope+C; //y=mx+c
//   //   // Resistance to Temperature
//   //   temp11= (Rx11/R0-1.0)/alpha; // from Rx = R0(1+alpha*X)
//   //         // Serial1.print("resistance : ");
//   // // Serial1.println(Rx1);

//   // Uncommect to convet celsius to fehrenheit
//   // temp = temp*1.8+32;
//   // Serial.println(temp);

//   return temp11;
// }

// float TS2(void)
// {

//   float V12 = 0;
//   float ts12 = 0;
//   float Rx12 = 0;
//   float temp12 = 0;

//   curr_time = millis();
//   for (int i = 0; i < sample; i++)
//   {
//     ts12 += analogRead(rtd3);
//     // while (millis() - curr_time <= sample_time)
//     // {
//     //   /* code */
//     // }
//     delay(10);
//   }

//   ts12 /= sample;

//   Serial1.print("ts12 : ");
//   Serial1.println(V12);

//   V12 = (ts12 / 1023.0) * 5.0; // (bits/2^n-1)*Vmax
//   Serial1.print("VOLATAGE : ");
//   Serial1.println(V12);

//   //  Rx12 = 1000 * ((2.19 * V12) / (5 - V12));
//   Rx12 = V12 * slope + C; // y=mx+c
//   Serial1.print("resistance : ");
//   Serial1.println(Rx12);

//   temp12 = ((Rx12 / R0) - 1) / alpha;

//   //     float C = 88;
//   // float slope = 14.187;
//   // float R0 = 100.0;
//   // float alpha = 0.00385;

//   // Rx12 = V12*slope+C; //y=mx+c
//   //   // Resistance to Temperature
//   //   temp12= (Rx12/R0-1.0)/alpha; // from Rx = R0(1+alpha*X)
//   //         // Serial1.print("resistance : ");
//   // // Serial1.println(Rx1);

//   // Uncommect to convet celsius to fehrenheit
//   // temp = temp*1.8+32;
//   // Serial.println(temp);

//   return temp12;
// }

// float TS3(void)
// {

//   float V1;
//   float Rx1;
//   float temp1, temp;
//   float ts1 = 0;

//   curr_time = millis();
//   for (int i = 0; i < sample; i++)
//   {
//     ts1 += analogRead(rtd5);
//     // while (millis() - curr_time <= sample_time)
//     // {
//     //   /* code */
//     // }

//     delay(10);
//   }

//   ts1 /= sample;
//   V1 = (ts1 / 1023.0) * 5.0; // (bits/2^n-1)*Vmax
//                              // // Serial1.print("VOLATAGE : ");
//                              // // Serial1.println(V1);
//                              //  Rx1 = 1000 * ((2.17 * V1) / (5 - V1));
//   Rx1 = V1 * slope + C;      // y=mx+c

//   // // Serial1.print("resistance : ");
//   // // Serial1.println(Rx1);

//   temp1 = ((Rx1 / R0) - 1) / alpha;
//   // float C = 88;
//   // float slope = 14.187;
//   // float R0 = 100.0;
//   // float alpha = 0.00385;

//   // Rx1 = V1*slope+C; //y=mx+c
//   //   // Resistance to Temperature
//   //   temp= (Rx1/R0-1.0)/alpha; // from Rx = R0(1+alpha*X)
//   //         // Serial1.print("resistance : ");
//   // // Serial1.println(Rx1);

//   // Uncommect to convet celsius to fehrenheit
//   // temp = temp*1.8+32;
//   // Serial.println(temp);

//   return temp1;
// }

// float mpx(void)
// {

//   float offset = 127.5;
//   curr_time = millis();

//   int raw = 0;

//   // for (int i = 0; i < sample; i++)
//   // {
//   //   raw += analogRead(A4);
//   //   while (millis() - curr_time <= sample_time)
//   //   {
//   //     /* code */
//   //   }

//   //   // delay(5);
//   // }

//   raw = analogRead(A4); // Reads the MAP sensor raw value on analog port 0

//   // raw /= sample;
//   float voltage = raw * 0.004887586; // let the compiler determine how many digits it can handle
//   float pressure = 0.0;

//   //   // Serial1.print("voltage: \t");
//   //  // Serial1.println(voltage,  3 );
//   // MATH
//   if (voltage < 4.6)
//   {
//     pressure = voltage * 700.0 / 4.6; // it raises 700KPA over 4.6 volt      faster =>   pressure =  voltage * 152.173913;
//   }
//   else
//   {
//     pressure = 700 + (voltage - 4.6) * 100 / 0.3; //   last piece raises  100KPa over 0.3 Volt.   // can be optimized to   p = c1 + v * c2
//   }

//   pressure = pressure - offset;

//   //  uint32_t current_time = millis();
//   //
//   // int  rawValue = 0;
//   // int offset = 220; //300; // zero pressure adjust
//   //
//   //  for (int x = 0; x < 10; x++)
//   //  {
//   //    // 2ms sampling rate or 500 hz
//   //    while(millis() - current_time < 2)
//   //    {
//   //      }
//   //    rawValue = rawValue + analogRead(A4);
//   //    }
//   //  rawValue = rawValue/10;
//   //  volatile float prr = (rawValue - offset) * 700.0 / (fullScale - offset); // pressure conversion

//   //
//   //  // Serial1.print("Raw A/D is  ");
//   //  // Serial1.print(rawValue);
//   //  // Serial1.print("   Pressure is  ");
//   //  // Serial1.print(prr, 1); // one decimal places
//   //  // Serial1.println("  kPa");

//   return pressure;
// }