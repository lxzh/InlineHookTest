package com.lxzh123.inlinehooktest;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import com.geetest.libcore.LIB;
//import com.lxzh123.hooker.XHooker;

public class MainActivity extends AppCompatActivity {
    TextView textView,textView1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Log.d("DumpDex", "MainActivity onCreate");
//        Hooker.hook();
        textView = findViewById(R.id.text);
        textView1 = findViewById(R.id.text1);
        textView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

//                Hooker.hook();
//                XHooker.getInstance().load();
                int x = 5;
                int square = LIB.get().square(5);

                textView.setText("square(" + x + ")=" + square);
            }
        });

        textView1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Hooker.printMap();
//                App.init();
            }
        });
    }
}