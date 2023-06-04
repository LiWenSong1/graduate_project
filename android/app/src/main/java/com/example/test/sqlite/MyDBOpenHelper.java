package com.example.test.sqlite;

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.util.Log;

import androidx.annotation.Nullable;

import java.io.File;

//使用SQLiteOpenHelper类创建数据库与版本管理
public class MyDBOpenHelper extends SQLiteOpenHelper {
    public MyDBOpenHelper(@Nullable Context context, @Nullable String name, @Nullable SQLiteDatabase.CursorFactory factory, int version) {
        super(context, name, factory, version);
    }

    String str="create table datatable(Date varchar(20) primary key," +
            "max_temp varchar(20),min_temp varchar(20)," +
            "max_humid varchar(20),min_humid varchar(20)," +
            "max_air varchar(20),min_air varchar(20))";
    //数据库第一次创建时调用
    @Override
    public void onCreate(SQLiteDatabase sqLiteDatabase) {
        sqLiteDatabase.execSQL(str);

    }
    public void deletetable(String table,SQLiteDatabase sqLiteDatabase){
        sqLiteDatabase.execSQL("drop table"+table);
    }
//软件版本号发生改变时调用
    @Override
    public void onUpgrade(SQLiteDatabase sqLiteDatabase, int oldversion, int newversion) {
        //sqLiteDatabase.execSQL("ALTER TABLE usertable ADD phone VARCHAR(20)");

    }
    private static boolean mainTmpDirSet = false;
    @Override
    public SQLiteDatabase getWritableDatabase() {
        if (!mainTmpDirSet) {
            boolean rs = new File("/data/data/com.example.test/databases/main").mkdir();
            Log.d("ahang", rs + "");
            super.getWritableDatabase().execSQL("PRAGMA temp_store_directory = '/data/data/com.example.test/databases/main'");
            mainTmpDirSet = true;
            return super.getWritableDatabase();
        }
        return super.getWritableDatabase();
    }
}
