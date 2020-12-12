package iotgui.esdk.onenet.com.iotgui;

import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;
import android.text.Editable;
import android.text.TextWatcher;
import android.widget.EditText;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;

/**
 * 节点配置Activity与相关逻辑
 * GIE Studio
 * */

public class ButtonActivity extends Activity {
    private String [] Setting_inf={"","","","","",""};
    String dev_name;
    String dev_id,dev_appid,dev_secret,dev_size,dev_pos;
    private EditText mET_name;
    private EditText mET_devid;
    private EditText mET_secret;
    private EditText mET_size;
    private EditText mET_pos;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_button);
        read_setting();
        mET_name=(EditText)findViewById(R.id.ed1);
        mET_devid=(EditText)findViewById(R.id.ed2);
        mET_secret=(EditText)findViewById(R.id.ed4);
        mET_size=(EditText)findViewById(R.id.ed5);
        mET_pos=(EditText)findViewById(R.id.ed6);
        mET_name.setText(dev_name);
        mET_devid.setText(dev_id);
        mET_secret.setText(dev_secret);
        mET_size.setText(dev_size);
        mET_pos.setText(dev_pos);
        mET_name.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {

            }

            @Override
            public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {
                Setting_inf[0]=("dev_name>"+charSequence.toString()+"\n");
            }

            @Override
            public void afterTextChanged(Editable editable) {

            }
        });
        mET_devid.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {

            }

            @Override
            public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {
                Setting_inf[1]=("device_id>"+charSequence.toString()+"\n");
            }

            @Override
            public void afterTextChanged(Editable editable) {

            }
        });
        mET_secret.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {

            }

            @Override
            public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {
                Setting_inf[3]=("app_secret>"+charSequence.toString()+"\n");
            }

            @Override
            public void afterTextChanged(Editable editable) {

            }
        });
        mET_size.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {

            }

            @Override
            public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {
                Setting_inf[4]=("font_size>"+charSequence.toString()+"\n");
            }

            @Override
            public void afterTextChanged(Editable editable) {

            }
        });
        mET_pos.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {

            }

            @Override
            public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {
                Setting_inf[5]=("font_pos>"+charSequence.toString()+"\n");
            }

            @Override
            public void afterTextChanged(Editable editable) {
            }
        });

    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onDestroy() {

        super.onDestroy();
        write_setting();
        Toast.makeText(ButtonActivity.this,"修改已保存",Toast.LENGTH_LONG).show();
    }

    private  void read_setting() {
        int line_log=0;
        String setting_file = Environment.getExternalStorageDirectory().toString() + File.separator +"Weather_station_settingsV3.txt";
        File fs_inf=new File(setting_file);
        try {
            InputStream instream = new FileInputStream(fs_inf);
            if (instream != null)
            {
                InputStreamReader inputreader = new InputStreamReader(instream);
                BufferedReader buffreader = new BufferedReader(inputreader);
                String line;
                //分行读取
                while ((( line = buffreader.readLine()) != null)&&(line_log<=4)) {
                    Setting_inf[line_log] = line+"\n" ;
                    line_log++;
                }
                instream.close();
                if(Setting_inf[0].length()<=11)
                    dev_name="0";
                else
                    dev_name=Setting_inf[0].substring(Setting_inf[0].indexOf(">")+1,Setting_inf[0].indexOf("\n"));
                if(Setting_inf[1].length()<=10)
                    dev_id="0";
                else
                    dev_id=Setting_inf[1].substring(Setting_inf[1].indexOf(">")+1,Setting_inf[1].indexOf("\n"));
                if(Setting_inf[2].length()<=10)
                    dev_secret="0";
                else
                    dev_secret=Setting_inf[2].substring(Setting_inf[2].indexOf(">")+1,Setting_inf[2].indexOf("\n"));
                if(Setting_inf[3].length()<=11)
                    dev_size="0";
                else
                    dev_size=Setting_inf[3].substring(Setting_inf[3].indexOf(">")+1,Setting_inf[3].indexOf("\n"));
                if(Setting_inf[4].length()<=10)
                    dev_pos="0";
                else
                    dev_pos=Setting_inf[4].substring(Setting_inf[4].indexOf(">")+1,Setting_inf[4].indexOf("\n"));

            }
        }
        catch (java.io.FileNotFoundException e)//未找到配置文件则创建之
        {
            try {
                FileOutputStream outStream_log =new FileOutputStream(fs_inf,true);
                outStream_log.write("dev_name>\n".getBytes("gbk"));
                outStream_log.write("device_id>\n".getBytes("gbk"));
                outStream_log.write("app_secret>\n".getBytes("gbk"));
                outStream_log.write("font_size>\n".getBytes("gbk"));
                outStream_log.write("font_pos>\n".getBytes("gbk"));
                outStream_log.flush();
                outStream_log.close();
            } catch (FileNotFoundException ee) {
                e.printStackTrace();
            }
            catch (IOException ee) {
                e.printStackTrace();
            }
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }
    }
    private  void write_setting() {
        int line_log=0;
        String setting_file = Environment.getExternalStorageDirectory().toString() + File.separator +"Weather_station_settings.txt";
        File fs_inf=new File(setting_file);
        FileOutputStream outStream;
        try {
            fs_inf.delete();
            fs_inf = new File(setting_file);
            outStream =new FileOutputStream(fs_inf,true);
            while (line_log<=5) {
                outStream.write((Setting_inf[line_log]).getBytes("gbk")) ;
                line_log++;
            }
            outStream.flush();
            outStream.close();
        } catch (FileNotFoundException ee) {
            ee.printStackTrace();
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }catch (IOException e) {
        e.printStackTrace();
        }


    }
}
