package com.example.test;

import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;

import android.annotation.TargetApi;
import android.app.ActivityManager;
import android.content.Intent;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import com.example.test.bean.BeforeDataBean;

import com.example.test.sqlite.MyDBOpenHelper;

import org.eclipse.paho.android.service.MqttAndroidClient;

import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;

import org.eclipse.paho.client.mqttv3.IMqttToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;

import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;
import java.util.TimeZone;
import java.util.regex.Matcher;
import java.util.regex.Pattern;


public class  MainActivity extends AppCompatActivity implements View.OnClickListener{



    public static MqttAndroidClient client;
    private MqttCallback mqttCallback;
    private static final String TAG="MQTT";
    private Calendar cal;
    private String year;
    private String day;
    private String month;
    private String time;
    private String msg;
    private int recive_falg=0;
    TextView temp1,humid,air,connectstaus,foodValue;
    Switch swit;
    EditText Date_EditText;
    ListView data_before_list;
    Button search,cam,flush,subbtn,setting,addfood;
    String  humid_value="0";
    String  temp_value="0";
    String  air_value="0";
    String  food_value="0";
    MyDBOpenHelper myDBOpenHelper;
    Intent intent;
    private SQLiteDatabase db;
    private List<BeforeDataBean> list_data;
    ListAdapter adapter;
    private static Pattern NUMBER_PATTERN = Pattern.compile("-?[0-9]+(\\.[0-9]+)?");
    @RequiresApi(api = Build.VERSION_CODES.M)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initview();
        iniData();
        client=Start_page.getclient();
        client.setCallback(mqttCallback);
        cal = Calendar.getInstance();
        cal.setTimeZone(TimeZone.getTimeZone("GMT+8:00"));
        search.setOnClickListener(this);
        cam.setOnClickListener(this);
        flush.setOnClickListener(this);
        setting.setOnClickListener(this);
        addfood.setOnClickListener(this);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            requestPermissions(new String[]{"android.permission.INTERNET"}, 1);
        }
        if (client.isConnected()) {
            connectstaus.setText("已连接");

        }
        else{
            connectstaus.setText("未连接");
        }


        sqlite_update();

        //获取数据
        list_data=getBeforeData(null);
        //创建适配器
        adapter=new ListAdapter(this,list_data);
        //设置适配器
        data_before_list.setAdapter(adapter);
        //                        list_data.clear();
//                        list_data=getBeforeData();
//                        adapter.notifyDataSetChanged();
    }


    public void initview(){

        temp1 = findViewById(R.id.temp);
        humid = findViewById(R.id.humid);
        air = findViewById(R.id.air);
        search=findViewById(R.id.search);
        setting=findViewById(R.id.setting);
        connectstaus=findViewById(R.id.connectstaus);
        cam=findViewById(R.id.cam);
        flush=findViewById(R.id.flush);
        Date_EditText=findViewById(R.id.Date_editText);
        data_before_list=findViewById(R.id.data_before);
        foodValue=findViewById(R.id.food);
        addfood=findViewById(R.id.add_food);

    }


    public void getvalue(){
        year = String.valueOf(cal.get(Calendar.YEAR));
        month = String.valueOf(cal.get(Calendar.MONTH)+1);
        day = String.valueOf(cal.get(Calendar.DATE));
        time = year + '-' + month + '-' + day;
        String strbuf;
        //Date date1 = new Date();
        // String time = date1.toLocaleString();
       // Toast.makeText(MainActivity.this, msg, Toast.LENGTH_SHORT).show();
        if(msg.length()>41){
            strbuf=msg.substring(0,msg.length()-3);
            strbuf=strbuf+','+'}';
            String bufstring = strbuf.substring(strbuf.indexOf("\"temp\"") + 7);
            Log.i("msg",bufstring);
            temp_value = bufstring.substring(0, bufstring.indexOf(','));
            bufstring = strbuf.substring(strbuf.indexOf("\"humid\"") + 8);
            humid_value = bufstring.substring(0, bufstring.indexOf(','));
            bufstring = strbuf.substring(strbuf.indexOf("\"Air\"") + 6);
            air_value = bufstring.substring(0, bufstring.indexOf(','));
            bufstring = strbuf.substring(strbuf.indexOf("\"food\"") + 7);
            food_value = bufstring.substring(0, bufstring.indexOf(','));
            // Toast.makeText(MainActivity.this, time, Toast.LENGTH_SHORT).show();

            Log.i(TAG, time);

        }


    }
    private void mqttPublish(String topic,String msg) {
        Integer qos = 0;
        Boolean retained = false;
        try {
            if (client != null) {
                client.publish(topic, msg.getBytes(), qos.intValue(), retained.booleanValue(), null, new IMqttActionListener() {
                    @Override
                    public void onSuccess(IMqttToken asyncActionToken) {
                        Toast.makeText(MainActivity.this, "发布消息成功", Toast.LENGTH_SHORT).show();
                    }

                    @Override
                    public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                        Toast.makeText(MainActivity.this, "发布消息失败", Toast.LENGTH_SHORT).show();
                    }
                });
            }
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }

            //更新数据库数据
    public void sqlite_update() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                int max_temp,min_temp,max_humid,min_humid,max_air,min_air;
                Cursor cursor=null;
                //连接数据库
                myDBOpenHelper=new MyDBOpenHelper(MainActivity.this,"Data.db",null,2);
                while (true) {
                    if(recive_falg==1) {
                        recive_falg = 0;
                        getvalue();
                        if (NUMBER_PATTERN.matcher(temp_value).matches() && NUMBER_PATTERN.matcher(humid_value).matches() && NUMBER_PATTERN.matcher(air_value).matches() && NUMBER_PATTERN.matcher(food_value).matches()) {
                            try {
                                db = myDBOpenHelper.getWritableDatabase();
                                //查询是否存在该日期
                                cursor = db.rawQuery("SELECT * from datatable where Date=?", new String[]{time});
                                if (cursor.getCount() == 0) {
                                    db.execSQL("INSERT INTO datatable(max_humid,min_humid,max_temp,min_temp,max_air,min_air,Date) VALUES(?,?,?,?,?,?,?)", new String[]{humid_value, humid_value, temp_value, temp_value, air_value, air_value, time});

                                } else {
                                    //获取指定数据
                                    cursor.moveToFirst();
                                    int max_temp_index = cursor.getColumnIndex("max_temp");
                                    int min_temp_index = cursor.getColumnIndex("min_temp");
                                    max_temp = Integer.parseInt(cursor.getString(max_temp_index));
                                    min_temp = Integer.parseInt(cursor.getString(min_temp_index));

                                    int max_humid_index = cursor.getColumnIndex("max_humid");
                                    int min_humid_index = cursor.getColumnIndex("min_humid");
                                    max_humid = Integer.parseInt(cursor.getString(max_humid_index));
                                    min_humid = Integer.parseInt(cursor.getString(min_humid_index));

                                    int max_air_index = cursor.getColumnIndex("max_air");
                                    int min_air_index = cursor.getColumnIndex("min_air");
                                    max_air = Integer.parseInt(cursor.getString(max_air_index));
                                    min_air = Integer.parseInt(cursor.getString(min_air_index));
                                    //更新温度值
                                    if (Integer.parseInt(temp_value) > max_temp) {
                                        db.execSQL("UPDATE datatable SET max_temp = ? WHERE Date = ?", new String[]{temp_value, time});

                                    } else if (Integer.parseInt(temp_value) < min_temp) {
                                        db.execSQL("UPDATE datatable SET min_temp = ? WHERE Date = ?", new String[]{temp_value, time});

                                    }
                                    //更新湿度值
                                    if (Integer.parseInt(humid_value) > max_humid) {
                                        db.execSQL("UPDATE datatable SET max_humid = ? WHERE Date = ?", new String[]{humid_value, time});

                                    } else if (Integer.parseInt(humid_value) < min_humid) {
                                        db.execSQL("UPDATE datatable SET min_humid = ? WHERE Date = ?", new String[]{humid_value, time});

                                    }
                                    //更新气体浓度值
                                    if (Integer.parseInt(air_value) > max_air) {
                                        db.execSQL("UPDATE datatable SET max_air = ? WHERE Date = ?", new String[]{air_value, time});

                                    } else if (Integer.parseInt(air_value) < min_air) {
                                        db.execSQL("UPDATE datatable SET min_air = ? WHERE Date = ?", new String[]{air_value, time});

                                    }

                                }
                                cursor.close();
                                //设置文本改变
                                temp1.post(new Runnable() {
                                    @Override
                                    public void run() {
                                        temp1.setText(String.format("%s", temp_value));
                                    }
                                });
                                humid.post(new Runnable() {
                                    @Override
                                    public void run() {
                                        humid.setText(String.format("%s", humid_value));
                                    }
                                });
                                air.post(new Runnable() {
                                    @Override
                                    public void run() {
                                        air.setText(String.format("%s", air_value));
                                    }
                                });
                                foodValue.post(new Runnable() {
                                    @Override
                                    public void run() {
                                        foodValue.setText(String.format("%s g", food_value));
                                    }
                                });

                            } catch (Exception e) {
                                e.printStackTrace();
                            }
                        }
                    }

                }


            }
        }).start();
    }


    //定义数据回调函数
    private void iniData() {
        mqttCallback = new MqttCallback() {
            @Override
            public void connectionLost(Throwable cause) {

            }
            @Override
            public void messageArrived(String topic, MqttMessage message) throws Exception {
                // 收到消息，打印消息
                msg = new String(message.getPayload());
                recive_falg=1;
                Log.i("msg",msg);

            }

            @Override
            public void deliveryComplete(IMqttDeliveryToken token) {

            }
        };

    }


    //获取历史数据
    public  List<BeforeDataBean> getBeforeData(String date){

        List<BeforeDataBean> list=new ArrayList<>();
        //连接数据库
        myDBOpenHelper=new MyDBOpenHelper(MainActivity.this,"Data.db",null,2);
        db=myDBOpenHelper.getWritableDatabase();
        BeforeDataBean bean;
        Cursor cursor=null;
        //如果是刷新或者是第一次进入，便获取所有数据
        if(date==null)
        {
            cursor=db.rawQuery("SELECT * from datatable",null);
        }
        //如果是搜索，便获取指定日期的数据
        else{
            cursor=db.rawQuery("SELECT * from datatable where Date=?",new String[]{date});
        }

        if(cursor.getCount()==0) {

        }
        else{
            cursor.moveToFirst();
            while(!cursor.isLast()){
               bean=new BeforeDataBean(cursor.getString((int)cursor.getColumnIndex("max_temp")),
                        cursor.getString((int)cursor.getColumnIndex("min_temp")),
                        cursor.getString((int)cursor.getColumnIndex("max_humid")),
                        cursor.getString((int)cursor.getColumnIndex("min_humid")),
                        cursor.getString((int)cursor.getColumnIndex("max_air")),
                        cursor.getString((int)cursor.getColumnIndex("min_air")),
                        cursor.getString((int)cursor.getColumnIndex("Date"))
                        );
                list.add(bean);
                cursor.moveToNext();
            }
           bean=new BeforeDataBean(cursor.getString((int)cursor.getColumnIndex("max_temp")),
                    cursor.getString((int)cursor.getColumnIndex("min_temp")),
                    cursor.getString((int)cursor.getColumnIndex("max_humid")),
                    cursor.getString((int)cursor.getColumnIndex("min_humid")),
                    cursor.getString((int)cursor.getColumnIndex("max_air")),
                    cursor.getString((int)cursor.getColumnIndex("min_air")),
                    cursor.getString((int)cursor.getColumnIndex("Date"))
            );
            list.add(bean);

        }

        return list;

    }
//    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
//    private void exitAPP() {
//        ActivityManager activityManager = (ActivityManager) MainActivity.this.getApplicationContext().getSystemService(MainActivity.ACTIVITY_SERVICE);
//        List<ActivityManager.AppTask> appTaskList = activityManager.getAppTasks();
//        for (ActivityManager.AppTask appTask : appTaskList) {
//            appTask.finishAndRemoveTask();
//        }
//    }


    @Override
    public void onClick(View view) {
        switch(view.getId())
        {
            case R.id.cam:
                intent=new Intent(MainActivity.this, CamActivity.class);
                startActivity(intent);
                MainActivity.this.finish();
                break;
            //点击刷新按钮
            case R.id.flush:
                Date_EditText.setText("");
                list_data.clear();
                list_data.addAll(getBeforeData(null));
                adapter.notifyDataSetChanged();
                break;
            //点击搜索按钮
            case R.id.search:
                //获取输入框信息
                String Date_msg=Date_EditText.getText().toString().trim();
                //清空数据源
                list_data.clear();
                //重新获取数据添加到数据源中
                list_data.addAll(getBeforeData(Date_msg));
                //通知适配器数据已改变
                adapter.notifyDataSetChanged();
                break;
            case R.id.setting:
                intent=new Intent(MainActivity.this, SettingActivity.class);
                startActivity(intent);
                MainActivity.this.finish();
                break;
            case R.id.add_food:
                mqttPublish("msg","{ADD}");
                addfood.setText("已投料");
                break;

        }

    }
    /**
     * ......................主线程终止处......................
     **/
    protected void onDestroy()
    {
        super.onDestroy();
    }

}
