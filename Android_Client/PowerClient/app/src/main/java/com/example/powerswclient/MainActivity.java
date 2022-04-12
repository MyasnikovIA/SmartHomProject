package com.example.powerswclient;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Spinner;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {
    SocketClientSlave sc;
    private String HOST = "128.0.24.172";
    private String PASS = "******";
    private String SendDevice = "";
    private int PORT = 8200;

    private Button mBtnSend = null;
    private Button btn_send2 = null;
    private Button status_1 = null;
    private TextView mTextView = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Spinner dynamicSpinner = (Spinner) findViewById(R.id.dynamic_spinner);
        String[] items = new String[]{"PowerSW_001","" , "PowerSW_005"};
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, items);
        dynamicSpinner.setAdapter(adapter);
        dynamicSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                Log.v("item", (String) parent.getItemAtPosition(position));
                SendDevice = (String) parent.getItemAtPosition(position);
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {
                // TODO Auto-generated method stub
            }
        });

        mBtnSend = (Button) findViewById(R.id.btn_send);
        btn_send2 = (Button) findViewById(R.id.btn_send2);
        mTextView = (TextView) findViewById(R.id.textView);
        mBtnSend.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (SendDevice.length() == 0) {
                    return;
                }
                sc.send("send:" + SendDevice + "\rpass:"+PASS+"\rmethod:push\rstatus:1\r\r\r", (byte[] data) -> {
                    Log.e("MainActivity", " Ответ на команду " + new String(data));
                });
            }
        });

        btn_send2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sc.send("pop\r\r\r");
            }
        });

        new Thread(new Runnable() {
            @Override
            public void run() {
                sc = new SocketClientSlave(HOST, PORT, "Android_Socket_Client");
                sc.loop((byte[] data) -> {
                    Log.e("MainActivity", " Основной поток " + new String(data));
                    runOnUiThread(new Runnable() { // переключаем контекст основного потока интерфейса, для визуализации данных
                        @Override
                        public void run() {
                            mTextView.setText(new String(data));
                        }
                    });
                });
                sc.start();
            }
        }).start();
    }

    public void onClick(View view) {
        String cmd = "";
        switch (view.getId()) {
            case R.id.on_0:
                cmd = "on:0";
                break;
            case R.id.on_1:
                cmd = "on:1";
                break;
            case R.id.on_2:
                cmd = "on:2";
                break;
            case R.id.on_3:
                cmd = "on:3";
                break;
            case R.id.on_4:
                cmd = "on:4";
                break;
            case R.id.on_5:
                cmd = "on:5";
                break;
            case R.id.on_6:
                cmd = "on:6";
                break;
            case R.id.on_7:
                cmd = "on:7";
                break;
            case R.id.off_0:
                cmd = "off:0";
                break;
            case R.id.off_1:
                cmd = "off:1";
                break;
            case R.id.off_2:
                cmd = "off:2";
                break;
            case R.id.off_3:
                cmd = "off:3";
                break;
            case R.id.off_4:
                cmd = "off:4";
                break;
            case R.id.off_5:
                cmd = "off:5";
                break;
            case R.id.off_6:
                cmd = "off:6";
                break;
            case R.id.off_7:
                cmd = "off:7";
                break;
            case R.id.status_1:
                cmd = "status:1";
                break;
            default:
                break;
        }
        if (cmd.length() == 0) {
            return;
        }
        Log.i("MainActivity", cmd);
        sc.send(cmd + "\r\r\r", (byte[] data) -> {
            sc.send("pop\r\r\r");
        });
    }
}