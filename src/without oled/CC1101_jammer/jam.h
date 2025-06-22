static void cc1101initialize(void)
{
  ELECHOUSE_cc1101.setSpiPin(sck, miso, mosi, ss);
  ELECHOUSE_cc1101.setGDO(gdo0, gdo2);
  ELECHOUSE_cc1101.Init();
  ELECHOUSE_cc1101.setGDO0(gdo0);
  ELECHOUSE_cc1101.setCCMode(1);
  ELECHOUSE_cc1101.setModulation(2);
  ELECHOUSE_cc1101.setDeviation(47.60);
  ELECHOUSE_cc1101.setChannel(0);
  ELECHOUSE_cc1101.setChsp(199.95);
  ELECHOUSE_cc1101.setRxBW(812.50);
  ELECHOUSE_cc1101.setDRate(9.6);
  ELECHOUSE_cc1101.setPA(12);
  ELECHOUSE_cc1101.setSyncMode(2);
  ELECHOUSE_cc1101.setSyncWord(211, 145);
  ELECHOUSE_cc1101.setAdrChk(0);
  ELECHOUSE_cc1101.setAddr(0);
  ELECHOUSE_cc1101.setWhiteData(0);
  ELECHOUSE_cc1101.setPktFormat(0);
  ELECHOUSE_cc1101.setLengthConfig(1);
  ELECHOUSE_cc1101.setPacketLength(0);
  ELECHOUSE_cc1101.setCrc(0);
  ELECHOUSE_cc1101.setCRC_AF(0);
  ELECHOUSE_cc1101.setDcFilterOff(0);
  ELECHOUSE_cc1101.setManchester(0);
  ELECHOUSE_cc1101.setFEC(0);
  ELECHOUSE_cc1101.setPRE(0);
  ELECHOUSE_cc1101.setPQT(0);
  ELECHOUSE_cc1101.setAppendStatus(0);
}
void jam(float frequency, byte payload) {
  unsigned long startTime = millis();
  unsigned long duration = jam_delay * 1000;  
  ELECHOUSE_cc1101.setMHZ(frequency); 
  while (true){
    server.handleClient();
    if (jam_delay > 0 && millis() - startTime >= duration) break;
    if (jam_break == true) break;
    ELECHOUSE_cc1101.SendData(jamdata, payload);
  }
  jam_break = false;
}
void range_jam(float start_frequency, float stop_frequency, float step, byte payload){
  unsigned long startTime = millis();
  unsigned long duration = jam_delay * 1000;
  while (true){
    for (float frequency = start_frequency; frequency <= stop_frequency; frequency+=step){
    server.handleClient();
    if (jam_delay > 0 && millis() - startTime >= duration) jam_break = true;
    if (jam_break == true) break;
    ELECHOUSE_cc1101.setMHZ(frequency);
    ELECHOUSE_cc1101.SendData(jamdata, payload);
    }
    if (jam_break == true) break;
  }
  jam_break = false;
}
void hopper_jam(float* hopperlist, int count, byte payload){
  unsigned long startTime = millis();
  unsigned long duration = jam_delay * 1000;  
  while (true){
    for (int i = 0; i < count; i++){
      server.handleClient();
      if (jam_delay > 0 && millis() - startTime >= duration) jam_break = true;
      if (jam_break == true) break;
      ELECHOUSE_cc1101.setMHZ(hopperlist[i]);
      ELECHOUSE_cc1101.SendData(jamdata, payload);
    }
    if (jam_break == true) break;
  }
  jam_break = false;
}