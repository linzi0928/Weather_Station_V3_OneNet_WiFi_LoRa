package iotgui.esdk.onenet.com.iotgui;

final class IoTData {
    private float temp; // temperature
    private float humi; // humidity
    private float press;
    private float gas_r;
    private float pm2_5;
    private float uvv;
    private float eCO2;
    private float TVOC;
    private float voltage;
    private String r_time;

    public float getTemperature() {
        return this.temp;
    }
    public void setTemperature(float value) {
        this.temp = value;
    }
    public float getHumidity() {
        return this.humi;
    }
    public void setHumidity(float value) {
        this.humi = value;
    }
    public float getPressure() {
        return this.press;
    }
    public void setPressure(float value) {
        this.press = value;
    }
    public float getGas_r() {
        return this.gas_r;
    }
    public void setGas_r(float value) {
        this.gas_r = value;
    }
    public float getPm2_5() {
        return this.pm2_5;
    }
    public void setPm2_5(float value) {
        this.pm2_5 = value;
    }
    public float getUvv() {
        return this.uvv;
    }
    public void setUvv(float value) {
        this.uvv = value;
    }
    public float getvoltage() {
        return this.voltage;
    }
    public void setvoltage(float value) {
        this.voltage = value;
    }
    public float getCO2() {
        return this.eCO2;
    }
    public void setCO2(float value) {
        this.eCO2 = value;
    }
    public float getTVOC() {
        return this.TVOC;
    }
    public void setTVOC(float value) {
        this.TVOC = value;
    }
    public String getTime() {
        return this.r_time;
    }
    public void setTime(String value) {
        this.r_time = value;
    }
}
