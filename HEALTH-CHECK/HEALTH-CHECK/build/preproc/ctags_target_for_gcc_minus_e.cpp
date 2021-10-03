# 1 "d:\\tunaOneDrive\\OneDrive - Hanoi University of Science and Technology\\tunaOneDriveSync\\tunaProjects\\Mercenary-projects\\HEALTH-CHECK\\HEALTH-CHECK\\HEALTH-CHECK.ino"
// Thu vien doc cam bien nhiet do DS18B20
# 3 "d:\\tunaOneDrive\\OneDrive - Hanoi University of Science and Technology\\tunaOneDriveSync\\tunaProjects\\Mercenary-projects\\HEALTH-CHECK\\HEALTH-CHECK\\HEALTH-CHECK.ino" 2
# 4 "d:\\tunaOneDrive\\OneDrive - Hanoi University of Science and Technology\\tunaOneDriveSync\\tunaProjects\\Mercenary-projects\\HEALTH-CHECK\\HEALTH-CHECK\\HEALTH-CHECK.ino" 2






// #define _5_PHUT 5 * 60 * 1000 // thoi gian 5 phut thuc te
// #define _1_PHUT 1 * 60 * 1000 // thoi gian 1 phut thuc te



int tinhTrangSucKhoe = 0; // 0 = binh thuong, 1 = canh bao, 2 = nguy hiem
bool coNhietDoCanhBao = false;
bool SOS_Status = 0;
unsigned long rungBinhThuong = 0;
unsigned long thoiDiemNhietDoBatDauCao = 0;
const int NHIET_DO_CAO = 38; // neu nhiet do > 40.0 trong 1 phut thi bao rung
const int NHIET_DO_NGUY_HIEM_CAO = 40; // neu nhiet do > 45.0 thi bao SOS
const int NHIET_DO_NGUY_HIEM_THAP = 36;

// Ham doc nhiet do tu cam bien
float LM35_Read()
{
  float ret = 0;
  float adcValude = analogRead(A0 /* pin doc cam bien nhiet do*/);
  ret = adcValude * 500 / 1024;
  Serial.println(ret);
  return ret;
}

//Ham ngat nut nhan sos
void SOS()
{
  SOS_Status = !SOS_Status;
  if (SOS_Status == 1)
  {
    digitalWrite(6 /* pin dieu khien coi*/, 0x0);
  }
  else
  {
    digitalWrite(6 /* pin dieu khien coi*/, 0x1);
  }
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(5 /* pin dieu khien led*/, 0x1);
  digitalWrite(5 /* pin dieu khien led*/, 0x0);
  pinMode(3 /* pin dieu khien dong co rung*/, 0x1);
  digitalWrite(3 /* pin dieu khien dong co rung*/, 0x0);
  pinMode(6 /* pin dieu khien coi*/, 0x1);
  digitalWrite(6 /* pin dieu khien coi*/, 0x1);
  pinMode(2 /* pin doc nut nhan sos*/, 0x0);
  attachInterrupt(((2 /* pin doc nut nhan sos*/) == 2 ? 0 : ((2 /* pin doc nut nhan sos*/) == 3 ? 1 : -1)), SOS, 2);
  delay(1000);
}

void loop()
{
  // put your main code here, to run repeatedly:
  // Giam sat tinh trang suc khoe
  unsigned long hienTai = millis();
  int nhietDoHienTai = (int)LM35_Read();
  if ((nhietDoHienTai < NHIET_DO_NGUY_HIEM_THAP) || (nhietDoHienTai > NHIET_DO_NGUY_HIEM_CAO))
  {
    tinhTrangSucKhoe = 2;
  }
  if ((nhietDoHienTai >= NHIET_DO_NGUY_HIEM_THAP) && (nhietDoHienTai <= NHIET_DO_CAO))
  {
    tinhTrangSucKhoe = 0;
    coNhietDoCanhBao = false;
  }
  if ((nhietDoHienTai > NHIET_DO_CAO) && (tinhTrangSucKhoe != 2))
  {
    if (!coNhietDoCanhBao)
    {
      thoiDiemNhietDoBatDauCao = hienTai;
      coNhietDoCanhBao = true;
    }
    if (hienTai - thoiDiemNhietDoBatDauCao > 2000 /* thoi gian 1 phut  cho mo phong*/)
    {
      tinhTrangSucKhoe = 1;
    }
  }

  // Dua ra canh bao
  if (tinhTrangSucKhoe == 0)
  {
    // xu ly khi tinh trang suc khoe binh thuong
    if (hienTai - rungBinhThuong > 10000 /* thoi gian 5 phut cho mo phong*/)
    {
      rungBinhThuong = hienTai;
      analogWrite(3 /* pin dieu khien dong co rung*/, 150);
      delay(1000);
      analogWrite(3 /* pin dieu khien dong co rung*/, 0);
      delay(1000);
    }
    digitalWrite(5 /* pin dieu khien led*/, 0);
    analogWrite(3 /* pin dieu khien dong co rung*/, 0);
  }
  if (tinhTrangSucKhoe == 1)
  {
    // xu ly khi tinh trang sung khoe can canh bao
    analogWrite(3 /* pin dieu khien dong co rung*/, 150);
  }
  // xu ly khi tinh trang suc khoe nguy hiem, dong co rung manh, den led bao dong sang bao SOS
  if (tinhTrangSucKhoe == 2)
  {
    digitalWrite(5 /* pin dieu khien led*/, 0x1);
    analogWrite(3 /* pin dieu khien dong co rung*/, 255);
    delay(2000);
    analogWrite(3 /* pin dieu khien dong co rung*/, 0);
    delay(1000);
  }
}
