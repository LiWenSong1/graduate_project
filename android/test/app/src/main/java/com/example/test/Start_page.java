package com.example.test;

import android.content.Intent;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.os.PersistableBundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import org.eclipse.paho.android.service.MqttAndroidClient;
import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;

import java.util.Arrays;

public class Start_page extends AppCompatActivity {

    public static MqttAndroidClient client;
    private MqttConnectOptions conOpt;
    private TextView connecttext;
    private Button start;
    private Switch conswitch;
    private static final String SERVER_URL = "tcp://mqtt.heclouds.com:6002";
    private static final String CLIENT_ID = "1067058780";
    private static final String USER_NAME = "590945";
    private static final String PASSWORD = "1";
    private MqttCallback mqttCallback;
    private IMqttActionListener mqttActionListener;
@Override
public void onCreate(@Nullable Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.start_page);
    start=findViewById(R.id.comeIn_btn);
    connecttext=findViewById(R.id.connecttext);
    conswitch=findViewById(R.id.s_v);

    client = new MqttAndroidClient(this, SERVER_URL, CLIENT_ID);
    //client.setCallback(mqttCallback);
    conOpt = new MqttConnectOptions();
    conOpt.setCleanSession(true);
    conOpt.setConnectionTimeout(10);
    conOpt.setKeepAliveInterval(30);
    conOpt.setUserName(USER_NAME.trim());
    conOpt.setPassword(PASSWORD.trim().toCharArray());
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
        requestPermissions(new String[]{"android.permission.INTERNET"}, 1);
    }
    mqttActionListener = new IMqttActionListener() {

        @Override
        public void onSuccess(IMqttToken asyncActionToken) {
            Log.d("connect", "连接成功");
        }

        @Override
        public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
            Log.d("connect", "连接失败");
        }
    };
    conswitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(CompoundButton compoundButton, boolean isChecked) {
            if (isChecked){
                if (!client.isConnected()) {
                    try {
                        // 连接
                        client.connect(conOpt, null, mqttActionListener);
                        Toast.makeText(Start_page.this, "mqttConnect succed:" , Toast.LENGTH_SHORT).show();
                        connecttext.setText("已连接");

                    } catch (MqttException e) {
                        Toast.makeText(Start_page.this, "mqttConnect fail:" + Arrays.toString(e.getStackTrace()), Toast.LENGTH_SHORT).show();
                    }
                }

            }else {
                try {
                    client.disconnect();
                } catch (MqttException e) {
                    e.printStackTrace();
                }
                connecttext.setText("未连接");

            }

        }
    });


    start.setOnClickListener(new View.OnClickListener() {
        @Override
        public void onClick(View view) {
            if (!client.isConnected()) {
                Toast.makeText(Start_page.this, "请先连接", Toast.LENGTH_SHORT).show();

            }else{
                mqttSubscribe("msg");

            }
            Intent intent=new Intent(Start_page.this,MainActivity.class);
            startActivity(intent);
        }
    });


}
    public static MqttAndroidClient getclient(){
        return client;
    }
    //订阅主题
    public void mqttSubscribe(String topic) {
        try {
            // 订阅myTopic话题
            client.subscribe(topic, 1, null, new IMqttActionListener() {
                @Override
                public void onSuccess(IMqttToken asyncActionToken) {
                    Toast.makeText(Start_page.this, "订阅成功", Toast.LENGTH_SHORT).show();
                }

                @Override
                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                    Toast.makeText(Start_page.this, "订阅失败", Toast.LENGTH_SHORT).show();
                }
            });
        } catch (MqttException e) {
            Toast.makeText(Start_page.this, "订阅失败：" + e.getStackTrace(), Toast.LENGTH_SHORT).show();
        }
    }



}
