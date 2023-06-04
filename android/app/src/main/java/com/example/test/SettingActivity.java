package com.example.test;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TimePicker;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import org.eclipse.paho.android.service.MqttAndroidClient;
import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttToken;
import org.eclipse.paho.client.mqttv3.MqttException;

import java.io.Serializable;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class SettingActivity extends AppCompatActivity implements Serializable {

    private EditText max_temp,min_temp,max_air,min_air,hour_text,minute_text;
    private Button publish;

    String max_temp_value,min_temp_value,max_air_value,min_air_value,hour_text_value,minute_text_value;
    private MqttAndroidClient client;
    private SharedPreferences mSharedPreferences;
    private static Pattern NUMBER_PATTERN = Pattern.compile("-?[0-9]+(\\.[0-9]+)?");
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.setting);
        initview();
        client=Start_page.getclient();
         mSharedPreferences = getSharedPreferences("last_data", Context.MODE_PRIVATE);
        max_temp.setText(mSharedPreferences.getString("max_temp",""));
        min_temp.setText( mSharedPreferences.getString("min_temp",""));
        max_air.setText(mSharedPreferences.getString("max_air",""));
         min_air.setText(mSharedPreferences.getString("min_air",""));
        max_temp.setText(mSharedPreferences.getString("max_temp",""));
        hour_text.setText(mSharedPreferences.getString("hour",""));
        minute_text.setText(mSharedPreferences.getString("minute",""));
        publish.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                try{
                    if(client.isConnected()){
                        SharedPreferences.Editor editor = mSharedPreferences.edit();
                        max_temp_value=max_temp.getText().toString();
                        min_temp_value= min_temp.getText().toString();
                        max_air_value=max_air.getText().toString();
                        min_air_value=min_air.getText().toString();
                        hour_text_value=hour_text.getText().toString();
                        minute_text_value=minute_text.getText().toString();
                        Matcher isNum = NUMBER_PATTERN.matcher(max_temp_value);
                        if (!isNum.matches()) {
                        Toast.makeText(SettingActivity.this,"请输入数字",Toast.LENGTH_SHORT).show();
                        return;

                        }
                       isNum = NUMBER_PATTERN.matcher(min_temp_value);
                        if (!isNum.matches()) {
                            Toast.makeText(SettingActivity.this,"请输入数字",Toast.LENGTH_SHORT).show();
                            return;
                        }
                        isNum = NUMBER_PATTERN.matcher(max_air_value);
                        if (!isNum.matches()) {
                            Toast.makeText(SettingActivity.this,"请输入数字",Toast.LENGTH_SHORT).show();
                            return;
                        }
                       isNum = NUMBER_PATTERN.matcher(min_air_value);
                        if (!isNum.matches()) {
                            Toast.makeText(SettingActivity.this,"请输入数字",Toast.LENGTH_SHORT).show();
                            return;
                        }
                        isNum = NUMBER_PATTERN.matcher(hour_text_value);
                        if (!isNum.matches()) {
                            Toast.makeText(SettingActivity.this,"请输入数字",Toast.LENGTH_SHORT).show();
                            return;
                        }
                        isNum = NUMBER_PATTERN.matcher(minute_text_value);
                        if (!isNum.matches()) {
                            Toast.makeText(SettingActivity.this,"请输入数字",Toast.LENGTH_SHORT).show();
                            return;
                        }


                        editor.putString("max_temp",max_temp_value);
                        editor.putString("min_temp",min_temp_value);
                        editor.putString("max_air",max_air_value);
                        editor.putString("min_air",min_air_value);
                        editor.putString("hour",hour_text_value);
                        editor.putString("minute",minute_text_value);
                        editor.commit();
                        String messege="{MaxT:"+max_temp_value+",MinT:"+min_temp_value+",MaxA:"+max_air_value+",MinA:"+min_air_value+",hour:"+hour_text_value+",minute:"+minute_text_value+",}";
                        Log.d("messege",messege);
                        mqttPublish("msg",messege);
                        Toast.makeText(SettingActivity.this,"设置成功",Toast.LENGTH_SHORT).show();
                        Thread.sleep(1000);
                        Intent backIntent = new Intent(SettingActivity.this, MainActivity.class);
                        startActivity(backIntent);
                        SettingActivity.this.finish();

                    }
                    else {
                        Toast.makeText(SettingActivity.this,"请先建立连接",Toast.LENGTH_SHORT).show();
                    }


                } catch (Exception e) {
                    Toast.makeText(SettingActivity.this,"输入错误",Toast.LENGTH_SHORT).show();
                }


            }
        });



    }
    private void mqttPublish(String topic,String msg) {
        Integer qos = 0;
        Boolean retained = false;
        try {
            if (client != null) {
                client.publish(topic, msg.getBytes(), qos.intValue(), retained.booleanValue(), null, new IMqttActionListener() {
                    @Override
                    public void onSuccess(IMqttToken asyncActionToken) {
                        Toast.makeText(SettingActivity.this, "发布消息成功", Toast.LENGTH_SHORT).show();
                    }

                    @Override
                    public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                        Toast.makeText(SettingActivity.this, "发布消息失败", Toast.LENGTH_SHORT).show();
                    }
                });
            }
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }

    private void initview(){
        max_temp=findViewById(R.id.max_temp);
        min_temp=findViewById(R.id.min_temp);
        max_air=findViewById(R.id.max_air);
        min_air=findViewById(R.id.min_air);
        publish=findViewById(R.id.publish);
        hour_text=findViewById(R.id.hour_text);
        minute_text=findViewById(R.id.minute_text);



    }
}
