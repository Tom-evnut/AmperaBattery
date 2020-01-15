#include "config.h"
#include "BMSModuleManager.h"
#include "BMSUtil.h"
#include "Logger.h"

extern EEPROMSettings settings;

BMSModuleManager::BMSModuleManager()
{
  for (int i = 1; i <= MAX_MODULE_ADDR; i++) {
    modules[i].setExists(false);
    modules[i].setAddress(i);
  }
  lowestPackVolt = 1000.0f;
  highestPackVolt = 0.0f;
  lowestPackTemp = 200.0f;
  highestPackTemp = -100.0f;
  isFaulted = false;
}

void BMSModuleManager::clearmodules()
{
  for (int y = 1; y < MAX_MODULE_ADDR; y++)
  {
    if (modules[y].isExisting())
    {
      modules[y].clearmodule();
      modules[y].setExists(false);
      modules[y].setAddress(y);
    }
  }
}

bool BMSModuleManager::checkcomms()
{
  int g = 0;
  for (int y = 1; y < 63; y++)
  {
    if (modules[y].isExisting())
    {
      g = 1;
      if (modules[y].isReset())
      {
        //Do nothing as the counter has been reset
      }
      else
      {
        return false;
      }
    }
    modules[y].setReset(false);
  }
  if ( g == 0)
  {
    return false;
  }
  return true;
}

int BMSModuleManager::seriescells()
{
  spack = 0;
  for (int y = 1; y < MAX_MODULE_ADDR; y++)
  {
    if (modules[y].isExisting())
    {
      spack = spack + modules[y].getscells();
    }
  }
  return spack;
}

void BMSModuleManager::decodecan(CAN_message_t &msg)
{
  int Id, CMU, Cells = 0;

  CMU = (msg.id & 0x00F) + 1;
  Id =  msg.id & 0x0F0;
  Cells = 4;
  /*
    switch (msg.id)
    {
    ////Module 1 Cells
    case 0x460:
      CMU = 1;
      Id = 1;
      Cells =4;
      break;
    case 0x470:
      CMU = 1;
      Id = 2;
      Cells =4;
      break;
    case 0x461:
      CMU = 1;
      Id = 3;
      Cells =4;
      break;
    case 0x471:
      CMU = 1;
      Id = 4;
      Cells =4;
      break;
    case 0x462:
      CMU = 1;
      Id = 5;
      Cells =4;
      break;
    case 0x472:
      CMU = 1;
      Id = 6;
      Cells =4;
      break;
    case 0x463:
      CMU = 1;
      Id = 7;
      Cells =4;
      break;
    case 0x473:
      CMU = 1;
      Id = 8;
      Cells =4;
      break;
    ////Module 2 Cells
    case 0x464:
      CMU = 2;
      Id = 1;
      break;
    case 0x474:
      CMU = 2;
      Id = 2;
      break;
    case 0x465:
      CMU = 2;
      Id = 3;
      break;
    case 0x475:
      CMU = 2;
      Id = 4;
      break;
    case 0x466:
      CMU = 2;
      Id = 5;
      break;
    case 0x476:
      CMU = 2;
      Id = 6;
      break;
    ////Module 3 Cells
    case 0x468:
      CMU = 3;
      Id = 1;
      break;
    case 0x478:
      CMU = 3;
      Id = 2;
      Cells =4;
      break;
    case 0x469:
      CMU = 3;
      Id = 3;
      break;
    case 0x479:
      CMU = 3;
      Id = 4;
      break;
    case 0x46A:
      CMU = 3;
      Id = 5;
      Cells =4;
      break;
    case 0x47A:
      CMU = 3;
      Id = 6;
      Cells =4;
      break;
    ////Module 4 Cells
    case 0x46C:
      CMU = 4;
      Id = 1;
      break;
    case 0x47C:
      CMU = 4;
      Id = 1;
      break;
    case 0x46D:
      CMU = 4;
      Id = 2;
      break;
    case 0x47D:
      CMU = 4;
      Id = 3;
      break;
    case 0x46E:
      CMU = 4;
      Id = 4;
      break;
    case 0x47E:
      CMU = 4;
      Id = 5;
      break;

    ////Module 1 Temps
    case 0x7E0:
      CMU = 1;
      Id = 11;
      break;
    case 0x7E1:
      CMU = 1;
      Id = 13;
      break;
    case 0x7E2:
      CMU = 1;
      Id = 12;
      break;
    case 0x7E3:
      CMU = 1;
      Id = 14;
      break;
    ////Module 2 Temps
    case 0x7E4:
      CMU = 2;
      Id = 11;
      break;
    case 0x7E5:
      CMU = 2;
      Id = 12;
      break;
    case 0x7E6:
      CMU = 2;
      Id = 12;
      break;
    ////Module 3 Temps
    case 0x7E8:
      CMU = 3;
      Id = 11;
      break;
    case 0x7E9:
      CMU = 3;
      Id = 12;
      break;
    case 0x7EA:
      CMU = 3;
      Id = 13;
      break;
    ////Module 4 Temps
    case 0x7EC:
      CMU = 4;
      Id = 11;
      break;
    case 0x7ED:
      CMU = 4;
      Id = 12;
      break;
    case 0x7EE:
      CMU = 4;
      Id = 13;
      break;
    default:
      break;
    }
  */
  /*
    Serial.println();
    Serial.print(CMU);
    Serial.print(',');
    Serial.print(Id);
    Serial.println();
  */
  if (modules[CMU].getAddress() != CMU)
  {
    modules[CMU].setAddress(CMU);
  }
  modules[CMU].setExists(true);
  modules[CMU].setReset(true);
  modules[CMU].decodecan(Id, msg);
}

void BMSModuleManager::getAllVoltTemp()
{
  packVolt = 0.0f;
  for (int x = 1; x <= MAX_MODULE_ADDR; x++)
  {
    if (modules[x].isExisting())
    {
      packVolt += modules[x].getModuleVoltage();
      if (modules[x].getLowTemp() < lowestPackTemp) lowestPackTemp = modules[x].getLowTemp();
      if (modules[x].getHighTemp() > highestPackTemp) highestPackTemp = modules[x].getHighTemp();
    }
  }

  packVolt = packVolt / Pstring;
  if (packVolt > highestPackVolt) highestPackVolt = packVolt;
  if (packVolt < lowestPackVolt) lowestPackVolt = packVolt;
}

void BMSModuleManager::balanceCells()
{
  for (int c = 0; c < 8; c++)
  {
    msg.buf[c] = 0;
  }
  for (int y = 1; y < 9; y++)
  {
    if (modules[y].isExisting() == 1)
    {
      int balance = 0;
      for (int i = 1; i < 9; i++)
      {

        if (getLowCellVolt() < modules[y].getCellVoltage(i))
        {
          balance = balance | (1 << i);
        }
      }
      msg.buf[y - 1] = balance;
    }
  }
  msg.id  = 0x300;
  msg.len = 8;
  Can0.write(msg);

  for (int c = 0; c < 8; c++)
  {
    msg.buf[c] = 0;
  }
  for (int y = 9; y < MAX_MODULE_ADDR; y++)
  {
    if (modules[y].isExisting() == 1)
    {
      int balance = 0;
      for (int i = 1; i < 9; i++)
      {
        if (getLowCellVolt() < modules[y].getCellVoltage(i))
        {
          if (y < 11 || i < 4)
          {
            balance = balance | (1 << i);
          }
          else
          {
            balance = balance | (1 << (i + 1));
          }
        }
      }
      msg.buf[y - 8] = balance;
    }
  }
  msg.id  = 0x310;
  msg.len = 5;
  Can0.write(msg);
}

float BMSModuleManager::getLowCellVolt()
{
  LowCellVolt = 5.0;
  for (int x = 1; x <= MAX_MODULE_ADDR; x++)
  {
    if (modules[x].isExisting())
    {
      if (modules[x].getLowCellV() <  LowCellVolt)  LowCellVolt = modules[x].getLowCellV();
    }
  }
  return LowCellVolt;
}

float BMSModuleManager::getHighCellVolt()
{
  HighCellVolt = 0.0;
  for (int x = 1; x <= MAX_MODULE_ADDR; x++)
  {
    if (modules[x].isExisting())
    {
      if (modules[x].getHighCellV() >  HighCellVolt)  HighCellVolt = modules[x].getHighCellV();
    }
  }
  return HighCellVolt;
}

float BMSModuleManager::getPackVoltage()
{
  return packVolt;
}

float BMSModuleManager::getLowVoltage()
{
  return lowestPackVolt;
}

float BMSModuleManager::getHighVoltage()
{
  return highestPackVolt;
}

void BMSModuleManager::setBatteryID(int id)
{
  batteryID = id;
}

void BMSModuleManager::setPstrings(int Pstrings)
{
  Pstring = Pstrings;
}

void BMSModuleManager::setSensors(int sensor, float Ignore)
{


  for (int x = 1; x <= MAX_MODULE_ADDR; x++)
  {
    if (modules[x].isExisting())
    {
      Serial.println(x);
      Serial.print('x');
      modules[x].settempsensor(sensor);
      modules[x].setIgnoreCell(Ignore);
    }
  }
}

float BMSModuleManager::getAvgTemperature()
{
  float avg = 0.0f;
  lowTemp = 999.0f;
  highTemp = -999.0f;
  int y = 0; //counter for modules below -70 (no sensors connected)
  numFoundModules = 0;
  for (int x = 1; x <= MAX_MODULE_ADDR; x++)
  {
    if (modules[x].isExisting())
    {
      numFoundModules++;
      if (modules[x].getAvgTemp() > -70)
      {
        avg += modules[x].getAvgTemp();
        if (modules[x].getHighTemp() > highTemp)
        {
          highTemp = modules[x].getHighTemp();
        }
        if (modules[x].getLowTemp() < lowTemp)
        {
          lowTemp = modules[x].getLowTemp();
        }
      }
      else
      {
        y++;
      }
    }
  }
  avg = avg / (float)(numFoundModules - y);

  return avg;
}

float BMSModuleManager::getHighTemperature()
{
  return highTemp;
}

float BMSModuleManager::getLowTemperature()
{
  return lowTemp;
}

float BMSModuleManager::getAvgCellVolt()
{
  float avg = 0.0f;
  for (int x = 1; x <= MAX_MODULE_ADDR; x++)
  {
    if (modules[x].isExisting()) avg += modules[x].getAverageV();
  }
  avg = avg / (float)numFoundModules;

  return avg;
}

void BMSModuleManager::printPackSummary()
{
  uint8_t faults;
  uint8_t alerts;
  uint8_t COV;
  uint8_t CUV;

  Logger::console(0, "");
  Logger::console(0, "");
  Logger::console(0, "");
  Logger::console(0, "Modules: %i  Cells: %i  Voltage: %fV   Avg Cell Voltage: %fV     Avg Temp: %fC ", numFoundModules, seriescells(),
                  getPackVoltage(), getAvgCellVolt(), getAvgTemperature());
  Logger::console(0, "");
  for (int y = 1; y < MAX_MODULE_ADDR; y++)
  {
    if (modules[y].isExisting())
    {
      faults = modules[y].getFaults();
      alerts = modules[y].getAlerts();
      COV = modules[y].getCOVCells();
      CUV = modules[y].getCUVCells();

      Logger::console(0, "                               Module #%i", y);

      Logger::console(0, "  Voltage: %fV   (%fV-%fV)     Temperatures: (%fC-%fC)", modules[y].getModuleVoltage(),
                      modules[y].getLowCellV(), modules[y].getHighCellV(), modules[y].getLowTemp(), modules[y].getHighTemp());
      if (faults > 0)
      {
        Logger::console(0, "  MODULE IS FAULTED:");
        if (faults & 1)
        {
          SERIALCONSOLE.print("    Overvoltage Cell Numbers (1-6): ");
          for (int i = 0; i < 32; i++)
          {
            if (COV & (1 << i))
            {
              SERIALCONSOLE.print(i + 1);
              SERIALCONSOLE.print(" ");
            }
          }
          SERIALCONSOLE.println();
        }
        if (faults & 2)
        {
          SERIALCONSOLE.print("    Undervoltage Cell Numbers (1-6): ");
          for (int i = 0; i < 32; i++)
          {
            if (CUV & (1 << i))
            {
              SERIALCONSOLE.print(i + 1);
              SERIALCONSOLE.print(" ");
            }
          }
          SERIALCONSOLE.println();
        }
        if (faults & 4)
        {
          Logger::console(0, "    CRC error in received packet");
        }
        if (faults & 8)
        {
          Logger::console(0, "    Power on reset has occurred");
        }
        if (faults & 0x10)
        {
          Logger::console(0, "    Test fault active");
        }
        if (faults & 0x20)
        {
          Logger::console(0, "    Internal registers inconsistent");
        }
      }
      if (alerts > 0)
      {
        Logger::console(0, "  MODULE HAS ALERTS:");
        if (alerts & 1)
        {
          Logger::console(0, "    Over temperature on TS1");
        }
        if (alerts & 2)
        {
          Logger::console(0, "    Over temperature on TS2");
        }
        if (alerts & 4)
        {
          Logger::console(0, "    Sleep mode active");
        }
        if (alerts & 8)
        {
          Logger::console(0, "    Thermal shutdown active");
        }
        if (alerts & 0x10)
        {
          Logger::console(0, "    Test Alert");
        }
        if (alerts & 0x20)
        {
          Logger::console(0, "    OTP EPROM Uncorrectable Error");
        }
        if (alerts & 0x40)
        {
          Logger::console(0, "    GROUP3 Regs Invalid");
        }
        if (alerts & 0x80)
        {
          Logger::console(0, "    Address not registered");
        }
      }
      if (faults > 0 || alerts > 0) SERIALCONSOLE.println();
    }
  }
}

void BMSModuleManager::printPackDetails(int digits, bool port)
{
  uint8_t faults;
  uint8_t alerts;
  uint8_t COV;
  uint8_t CUV;
  int cellNum = 0;
  if (port == 0)
  {
    Logger::console(0, "");
    Logger::console(0, "");
    Logger::console(0, "");
    Logger::console(0, "Modules: %i Cells: %i Strings: %i  Voltage: %fV   Avg Cell Voltage: %fV  Low Cell Voltage: %fV   High Cell Voltage: %fV Delta Voltage: %zmV   Avg Temp: %fC ", numFoundModules, seriescells(),
                    Pstring, getPackVoltage(), getAvgCellVolt(), LowCellVolt, HighCellVolt, (HighCellVolt - LowCellVolt) * 1000, getAvgTemperature());
    Logger::console(0, "");
    for (int y = 1; y < MAX_MODULE_ADDR; y++)
    {
      if (modules[y].isExisting())
      {
        faults = modules[y].getFaults();
        alerts = modules[y].getAlerts();
        COV = modules[y].getCOVCells();
        CUV = modules[y].getCUVCells();

        SERIALCONSOLE.print("Module #");
        SERIALCONSOLE.print(y);
        if (y < 10) SERIALCONSOLE.print(" ");
        SERIALCONSOLE.print("  ");
        SERIALCONSOLE.print(modules[y].getModuleVoltage(), digits);
        SERIALCONSOLE.print("V");
        if (modules[y].getCellsUsed() > 12 )
        {
          for (int i = 1; i < 12; i++)
          {
            if (cellNum < 10) SERIALCONSOLE.print(" ");
            SERIALCONSOLE.print("  Cell");
            SERIALCONSOLE.print(cellNum++);
            SERIALCONSOLE.print(": ");
            SERIALCONSOLE.print(modules[y].getCellVoltage(i), digits);
            SERIALCONSOLE.print("V");
          }
          if (modules[y].getCellsUsed() > 12 && modules[y].getCellsUsed() < 24)
          {
            for (int i = 13; i < modules[y].getCellsUsed() + 1; i++)
            {
              if (cellNum < 10) SERIALCONSOLE.print(" ");
              SERIALCONSOLE.print("  Cell");
              SERIALCONSOLE.print(cellNum++);
              SERIALCONSOLE.print(": ");
              SERIALCONSOLE.print(modules[y].getCellVoltage(i), digits);
              SERIALCONSOLE.print("V");
            }
          }
          else
          {
            for (int i = 13; i < 24; i++)
            {
              if (cellNum < 10) SERIALCONSOLE.print(" ");
              SERIALCONSOLE.print("  Cell");
              SERIALCONSOLE.print(cellNum++);
              SERIALCONSOLE.print(": ");
              SERIALCONSOLE.print(modules[y].getCellVoltage(i), digits);
              SERIALCONSOLE.print("V");
            }
          }
          SERIALCONSOLE.println("               ");
          if (modules[y].getCellsUsed() > 25)
          {
            for (int i = 25; i < modules[y].getCellsUsed() + 1; i++)
            {
              if (cellNum < 10) SERIALCONSOLE.print(" ");
              SERIALCONSOLE.print("  Cell");
              SERIALCONSOLE.print(cellNum++);
              SERIALCONSOLE.print(": ");
              SERIALCONSOLE.print(modules[y].getCellVoltage(i), digits);
              SERIALCONSOLE.print("V");
            }
          }
        }
        else
        {
          for (int i = 1; i < modules[y].getCellsUsed() + 1 ; i++)
          {
            if (cellNum < 10) SERIALCONSOLE.print(" ");
            SERIALCONSOLE.print("  Cell");
            SERIALCONSOLE.print(cellNum++);
            SERIALCONSOLE.print(": ");
            SERIALCONSOLE.print(modules[y].getCellVoltage(i), digits);
            SERIALCONSOLE.print("V");
          }
        }
        SERIALCONSOLE.println("               ");
        SERIALCONSOLE.print(" Temp 1: ");
        SERIALCONSOLE.print(modules[y].getTemperature(0));
        SERIALCONSOLE.println("               ");
      }
    }
  }
  else
  {
    Logger::console(1, "");
    Logger::console(1, "");
    Logger::console(1, "");
    Logger::console(1, "Modules: %i Cells: %i Strings: %i  Voltage: %fV   Avg Cell Voltage: %fV  Low Cell Voltage: %fV   High Cell Voltage: %fV Delta Voltage: %zmV   Avg Temp: %fC ", numFoundModules, seriescells(),
                    Pstring, getPackVoltage(), getAvgCellVolt(), LowCellVolt, HighCellVolt, (HighCellVolt - LowCellVolt) * 1000, getAvgTemperature());
    Logger::console(1, "");
    for (int y = 1; y < MAX_MODULE_ADDR; y++)
    {
      if (modules[y].isExisting())
      {
        faults = modules[y].getFaults();
        alerts = modules[y].getAlerts();
        COV = modules[y].getCOVCells();
        CUV = modules[y].getCUVCells();

        Serial2.print("Module #");
        Serial2.print(y);
        if (y < 10) Serial2.print(" ");
        Serial2.print("  ");
        Serial2.print(modules[y].getModuleVoltage(), digits);
        Serial2.print("V");
        if (modules[y].getCellsUsed() > 12 )
        {
          for (int i = 1; i < 12; i++)
          {
            if (cellNum < 10) Serial2.print(" ");
            Serial2.print("  Cell");
            Serial2.print(cellNum++);
            Serial2.print(": ");
            Serial2.print(modules[y].getCellVoltage(i), digits);
            Serial2.print("V");
          }
          if (modules[y].getCellsUsed() > 12 && modules[y].getCellsUsed() < 24)
          {
            for (int i = 13; i < modules[y].getCellsUsed() + 1; i++)
            {
              if (cellNum < 10) Serial2.print(" ");
              Serial2.print("  Cell");
              Serial2.print(cellNum++);
              Serial2.print(": ");
              Serial2.print(modules[y].getCellVoltage(i), digits);
              Serial2.print("V");
            }
          }
          else
          {
            for (int i = 13; i < 24; i++)
            {
              if (cellNum < 10) Serial2.print(" ");
              Serial2.print("  Cell");
              Serial2.print(cellNum++);
              Serial2.print(": ");
              Serial2.print(modules[y].getCellVoltage(i), digits);
              Serial2.print("V");
            }
          }
          Serial2.println("               ");
          if (modules[y].getCellsUsed() > 25)
          {
            for (int i = 25; i < modules[y].getCellsUsed() + 1; i++)
            {
              if (cellNum < 10) Serial2.print(" ");
              Serial2.print("  Cell");
              Serial2.print(cellNum++);
              Serial2.print(": ");
              Serial2.print(modules[y].getCellVoltage(i), digits);
              Serial2.print("V");
            }
          }
        }
        else
        {
          for (int i = 1; i < modules[y].getCellsUsed() + 1 ; i++)
          {
            if (cellNum < 10) Serial2.print(" ");
            Serial2.print("  Cell");
            Serial2.print(cellNum++);
            Serial2.print(": ");
            Serial2.print(modules[y].getCellVoltage(i), digits);
            Serial2.print("V");
          }
        }
        Serial2.println("               ");
        Serial2.print(" Temp 1: ");
        Serial2.print(modules[y].getTemperature(0));
        Serial2.println("               ");
      }
    }
  }
}

void BMSModuleManager::printAllCSV(unsigned long timestamp, float current, int SOC)
{
  for (int y = 1; y < 63; y++)
  {
    if (modules[y].isExisting())
    {
      SERIALCONSOLE.print(timestamp);
      SERIALCONSOLE.print(",");
      SERIALCONSOLE.print(current, 0);
      SERIALCONSOLE.print(",");
      SERIALCONSOLE.print(SOC);
      SERIALCONSOLE.print(",");
      SERIALCONSOLE.print(y);
      SERIALCONSOLE.print(",");
      for (int i = 0; i < 8; i++)
      {
        SERIALCONSOLE.print(modules[y].getCellVoltage(i));
        SERIALCONSOLE.print(",");
      }
      SERIALCONSOLE.print(modules[y].getTemperature(0));
      SERIALCONSOLE.print(",");
      SERIALCONSOLE.print(modules[y].getTemperature(1));
      SERIALCONSOLE.print(",");
      SERIALCONSOLE.print(modules[y].getTemperature(2));
      SERIALCONSOLE.println();
    }
  }
  for (int y = 1; y < 63; y++)
  {
    if (modules[y].isExisting())
    {
      Serial2.print(timestamp);
      Serial2.print(",");
      Serial2.print(current, 0);
      Serial2.print(",");
      Serial2.print(SOC);
      Serial2.print(",");
      Serial2.print(y);
      Serial2.print(",");
      for (int i = 0; i < 8; i++)
      {
        Serial2.print(modules[y].getCellVoltage(i));
        Serial2.print(",");
      }
      Serial2.print(modules[y].getTemperature(0));
      Serial2.print(",");
      Serial2.print(modules[y].getTemperature(1));
      Serial2.print(",");
      Serial2.print(modules[y].getTemperature(2));
      Serial2.println();
    }
  }
}
