package com.example.test;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.media.MediaPlayer;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.webkit.JsResult;
import android.webkit.WebChromeClient;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.Socket;
import java.net.SocketException;
import java.nio.charset.StandardCharsets;
import java.util.Enumeration;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

//public class CamActivity extends AppCompatActivity implements SurfaceHolder.Callback{
public class CamActivity extends AppCompatActivity {

    public ImageView IMG_imageView;
    public EditText ip_editText,port_editText;
    public Button conbtn,backbtn;
    private Socket socket;
    private InputStream inputStream;
    private OutputStream outputStream;
    byte[] RevBuff = new byte[1024];
    byte[] temp = new byte[0];
    private int headFlag=0;
    Bitmap bitmap = null;
    private SharedPreferences mSharedPreferences;
    private String ip,port;
    String validIpRegex = "^(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|[1-9])\\."
            + "(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)\\."
            + "(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)\\."
            + "(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)$";
    Pattern NUMBER_PATTERN,PORT_PATTERN;
    /*******************************************************************************
     * ......................Handler此种形式需在主线程开始前定义......................
     ******************************************************************************/
    private Handler myHandler = new Handler(Looper.myLooper())
    {
        public void handleMessage(Message msg)
        {
            super.handleMessage(msg);
            switch (msg.what)
            {
                case 0:
                    Toast.makeText(CamActivity.this,"连接成功",Toast.LENGTH_SHORT).show();
                    break;
                case 1:
                    //Toast.makeText(CamActivity.this,"连接异常",Toast.LENGTH_SHORT).show();
                    break;
                case 3:
                    bitmap = BitmapFactory.decodeByteArray(temp, 0,temp.length);
                    if(bitmap!=null){

                        IMG_imageView.setImageBitmap(bitmap);//这句就能显示图片(bitmap数据没问题的情况下) 存在图像闪烁情况 待解决


                    }
                    temp = new byte[0];  //一帧图像显示结束  将 temp清零

                    break;

                default:
                    break;
            }
        }
    };
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.camweb);
        ip_editText=findViewById(R.id.ipeditText);
        IMG_imageView=findViewById(R.id.imageView);
        port_editText=findViewById(R.id.porteditText);
        conbtn=findViewById(R.id.conBtn);
        backbtn=findViewById(R.id.backbtn);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            requestPermissions(new String[]{"android.permission.INTERNET"}, 1);
        }
      NUMBER_PATTERN = Pattern.compile(validIpRegex);
        PORT_PATTERN= Pattern.compile("-?[0-9]+(\\.[0-9]+)?");
        mSharedPreferences = getSharedPreferences("last_data", Context.MODE_PRIVATE);
        ip_editText.setText(mSharedPreferences.getString("ip",""));
        port_editText.setText(mSharedPreferences.getString("port",""));
        conbtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                SharedPreferences.Editor editor = mSharedPreferences.edit();
                ip=ip_editText.getText().toString();
                port= port_editText.getText().toString();
                Matcher isNum = NUMBER_PATTERN.matcher(ip);
                if (!isNum.matches()) {
                    Toast.makeText(CamActivity.this,"ip格式错误",Toast.LENGTH_SHORT).show();
                    return;

                }
                isNum = PORT_PATTERN.matcher(port);
                if (!isNum.matches()) {
                    Toast.makeText(CamActivity.this,"端口格式错误",Toast.LENGTH_SHORT).show();
                    return;
                }
                editor.putString("ip",ip);
                editor.putString("port",port);
                editor.commit();
                con_server();
            }
        });
        backbtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(socket!=null){
                    try { socket.close(); } catch (IOException e) { e.printStackTrace(); }
                    try { inputStream.close(); }catch (IOException e) { e.printStackTrace(); }
                }

                Intent backIntent = new Intent(CamActivity.this, MainActivity.class);
                startActivity(backIntent);
                CamActivity.this.finish();
            }
        });

    }
    //连接服务器
    protected void con_server(){
        new Thread(new Runnable() {
            @Override
            public void run() {
                Message msg = myHandler.obtainMessage();
                try {
                    socket = new Socket((ip_editText.getText()).toString(),Integer.parseInt(port_editText.getText().toString()));
                    if(socket.isConnected()){
                        msg.what = 0;//显示连接服务器成功信息
                        inputStream = socket.getInputStream();

                        Recv();//接收数据
                    }else{
                        msg.what = 1;

                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
                myHandler.sendMessage(msg);

            }
        }).start();


    }
    //    接收图像数据方法
    public void Recv(){
        new Thread(new Runnable() {
            @Override
            public void run() {
                int i=0;
                while(socket != null && socket.isConnected()){
                    try {
                        int Len = inputStream.read(RevBuff);
                        if(Len != -1){
//                          图像数据包的头  FrameBegin
                            boolean begin_cam_flag = RevBuff[0] == 70 && RevBuff[1] == 114 && RevBuff[2] == 97 && RevBuff[3] == 109 && RevBuff[4] == 101
                                    && RevBuff[5] == 66 && RevBuff[6] == 101 && RevBuff[7] == 103 && RevBuff[8] == 105 && RevBuff[9] == 110 ;
//                            图像数据包的尾  FrameOverr
                            boolean end_cam_flag = RevBuff[0] == 70 && RevBuff[1] == 114 && RevBuff[2] == 97 && RevBuff[3] == 109 && RevBuff[4] == 101
                                    && RevBuff[5] == 79 && RevBuff[6] == 118 && RevBuff[7] == 101 && RevBuff[8] == 114 && RevBuff[9] == 114;
//                            判断接收的包是不是图片的开头数据 是的话s说明下面的数据属于图片数据 将headFlag置1
                            if(begin_cam_flag){
                                headFlag = 1;
                                //temp = new byte[0];  //一帧图像显示结束  将 temp清零
                            }else if(end_cam_flag){  //判断包是不是图像的结束包 是的话 将数据传给 myHandler  3 同时将headFlag置0
                                Message msg = myHandler.obtainMessage();
                                msg.what = 3;
                                myHandler.sendMessage(msg);
                                i++;
                                Log.i("img",String.valueOf(i));
                                headFlag = 0;
                            }else if(headFlag == 1){ //如果 headFlag == 1 说明包是图像数据  将数据发给byteMerger方法 合并一帧图像
                                temp = byteMerger(temp,RevBuff);
                            }
//
                        }else{
//                            如果Len = -1 说明接受异常  显示连接服务器失败信息  跳出循环
                            Message msg = myHandler.obtainMessage();
                            msg.what = 1;
                            myHandler.sendMessage(msg);
                            break;
                        }
                    } catch (IOException e) {
//                        如果接受数据inputStream.read(RevBuff)语句执行失败 显示连接服务器失败信息  跳出循环
                        e.printStackTrace();
                        Message msg = myHandler.obtainMessage();
                        msg.what = 1;
                        myHandler.sendMessage(msg);
                        break;
                    }
                }
            }
        }).start();
    }

    //    合并一帧图像数据  a 全局变量 temp   b  接受的一个数据包 RevBuff
    public byte[] byteMerger(byte[] a,byte[] b){
        int i = a.length + b.length;
        byte[] t = new byte[i]; //定义一个长度为 全局变量temp  和 数据包RevBuff 一起大小的字节数组 t
        System.arraycopy(a,0,t,0,a.length);  //先将 temp（先传过来的数据包）放进  t
        System.arraycopy(b,0,t,a.length,b.length);//然后将后进来的这各数据包放进t
        return t; //返回t给全局变量 temp
    }

    /**
     * ......................主线程终止处......................
     **/
    protected void onDestroy()
    {
        super.onDestroy();
    }

}


