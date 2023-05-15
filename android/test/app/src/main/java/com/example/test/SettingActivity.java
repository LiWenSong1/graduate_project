package com.example.test;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import org.eclipse.paho.android.service.MqttAndroidClient;
import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttToken;
import org.eclipse.paho.client.mqttv3.MqttException;

import java.io.Serializable;

public class SettingActivity extends AppCompatActivity implements Serializable {

    private EditText max_temp,min_temp,max_air,min_air;
    private Button publish;
    String max_temp_value,min_temp_value,max_air_value,min_air_value;
    private MqttAndroidClient client;
    private SharedPreferences mSharedPreferences;
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
                        editor.putString("max_temp",max_temp_value);
                        editor.putString("min_temp",min_temp_value);
                        editor.putString("max_air",max_air_value);
                        editor.putString("min_air",min_air_value);
                        editor.commit();
                        String messege="{MaxT:"+max_temp_value+",MinT:"+min_temp_value+",MaxA:"+max_air_value+",MinA:"+min_air_value+",}";
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
    public void mqttPublish(String topic,String msg) {
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


    }
}
