static void cc1101initialize(void)
{
  ELECHOUSE_cc1101.setSpiPin(sck, miso, mosi, ss);
  ELECHOUSE_cc1101.setGDO(gdo0, gdo2);
  ELECHOUSE_cc1101.Init();
  ELECHOUSE_cc1101.setGDO0(gdo0);
  ELECHOUSE_cc1101.setCCMode(1);
  ELECHOUSE_cc1101.setModulation(2);
  ELECHOUSE_cc1101.setMHZ(433.92);
  ELECHOUSE_cc1101.setDeviation(47.60);
  ELECHOUSE_cc1101.setChannel(0);
  ELECHOUSE_cc1101.setChsp(199.95);
  ELECHOUSE_cc1101.setRxBW(812.50);
  ELECHOUSE_cc1101.setDRate(9.6);
  ELECHOUSE_cc1101.setPA(10);
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
void jam(float frequency)
{
  display.clearDisplay(); 
  display.drawBitmap(0, 0, bitmap_keyfob_jam, 128, 64, WHITE); 
  display.display();
  ELECHOUSE_cc1101.setMHZ(frequency);
  buttOK.tick();
  while (!buttOK.isSingle()){
    buttOK.tick();
    for (int j = 0; j < 60; j++) {
      jamdata[j] = (byte)random(255);
    }
    ELECHOUSE_cc1101.SendData(jamdata, 60);
  }
}