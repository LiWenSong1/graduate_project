package com.example.test.bean;

public class BeforeDataBean {
    private String max_temp;
    private String min_temp;
    private String max_air;
    private String min_air;
    private String max_humid;
    private String min_humid;
    private String time;

    public BeforeDataBean(String max_temp, String min_temp, String max_air, String min_air, String max_humid, String min_humid, String time) {
        this.max_temp = max_temp;
        this.min_temp = min_temp;
        this.max_air = max_air;
        this.min_air = min_air;
        this.max_humid = max_humid;
        this.min_humid = min_humid;
        this.time = time;
    }

    public String getMax_temp() {
        return max_temp;
    }

    public void setMax_temp(String max_temp) {
        this.max_temp = max_temp;
    }

    public String getMin_temp() {
        return min_temp;
    }

    public void setMin_temp(String min_temp) {
        this.min_temp = min_temp;
    }

    public String getMax_air() {
        return max_air;
    }

    public void setMax_air(String max_air) {
        this.max_air = max_air;
    }

    public String getMin_air() {
        return min_air;
    }

    public void setMin_air(String min_air) {
        this.min_air = min_air;
    }

    public String getMax_humid() {
        return max_humid;
    }

    public void setMax_humid(String max_humid) {
        this.max_humid = max_humid;
    }

    public String getMin_humid() {
        return min_humid;
    }

    public void setMin_humid(String min_humid) {
        this.min_humid = min_humid;
    }

    public String getTime() {
        return time;
    }

    public void setTime(String time) {
        this.time = time;
    }
}
