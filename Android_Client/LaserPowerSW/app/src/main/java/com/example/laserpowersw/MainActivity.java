package com.example.laserpowersw;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {
    SocketClientSlave sc;
    private String HOST = "128.0.24.172";
    private String PASS = "********";
    private String SendDevice = "LaserPowerSW";
    private int PORT = 8200;

    private Button connect_pw = null;
    private Button status_1 = null;
    private TextView mTextView = null;
    private Thread loop;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mTextView = (TextView) findViewById(R.id.textView);
        ((Button) findViewById(R.id.connect_pw)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sc.send("send:" + SendDevice + "\rpass:"+PASS+"\rmethod:push\rstatus:1\r\r\r", (byte[] data) -> {
                    Log.e("MainActivity", " Ответ на команду " + new String(data));
                });
            }
        });

        ((Button) findViewById(R.id.pop_sw)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sc.send("pop\r\r\r");
            }
        });

        loop = new Thread(new Runnable() {
            @Override
            public void run() {
                sc = new SocketClientSlave(HOST, PORT, "Samsyng_Client");
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
        });
        loop.start();
    }

    public void onClick(View view) {
        String cmd = "";
        switch (view.getId()) {
            case R.id.Lazer_on:
                cmd = "on:0";
                break;
            case R.id.Lazer_off:
                cmd = "off:0";
                break;
            case R.id.Vint_on:
                cmd = "on:1";
                break;
            case R.id.Vint_off:
                cmd = "off:1";
                break;
            case R.id.ozon_on:
                cmd = "on:5";
                break;
            case R.id.ozon_off:
                cmd = "off:5";
                break;
            case R.id.rezerv_on:
                cmd = "on:6";
                break;
            case R.id.rezerv_off:
                cmd = "off:6";
                break;
            case R.id.reset_lazer_click:
                cmd = "reset:1\rdelay:1000000";
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