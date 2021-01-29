package iotgui.esdk.onenet.com.iotgui;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.text.format.DateFormat;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.components.XAxis;
import com.github.mikephil.charting.data.Entry;
import com.github.mikephil.charting.data.LineData;
import com.github.mikephil.charting.data.LineDataSet;
import com.github.mikephil.charting.formatter.ValueFormatter;
import com.github.mikephil.charting.utils.ViewPortHandler;

import org.apache.http.HttpResponse;
import org.apache.http.NameValuePair;
import org.apache.http.client.HttpClient;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.client.methods.HttpUriRequest;
import org.apache.http.conn.scheme.Scheme;
import org.apache.http.conn.ssl.AllowAllHostnameVerifier;
import org.apache.http.conn.ssl.SSLSocketFactory;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;
import org.apache.http.util.EntityUtils;
import org.json.JSONArray;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.security.KeyStore;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TimeZone;

import javax.net.ssl.KeyManagerFactory;
import javax.net.ssl.TrustManagerFactory;

import static android.content.ContentValues.TAG;
import static clwater.androiddashboard.DashBoard.pos_setting;
import static clwater.androiddashboard.DashBoard.size_setting;
import static iotgui.esdk.onenet.com.iotgui.Constant.APPID;
import static iotgui.esdk.onenet.com.iotgui.Constant.SECRET_OneNet;
/**
户外长时气象站User Terminal
本APP所获数据从中国移动物联网平台OneNet抓取并显示，相关节点信息用户可在节点设置中调整与保存
初次写安卓应用，代码如有不规范还请多担待。
本文件主要是信息获取与显示相关代码。
DashBoard.java主要包含仪表盘相关代码。
BottonActivity.java主要包含节点配置相关代码。
DensityUtil.java主要包含分辨率适配相关代码
折线图的库来自：com.github.mikephil.charting.charts.LineChart
仪表盘的库来自：clwater.androiddashboard
Http连接onenet相关代码来自华为SDK改
 Android Studio版本：3.5.3
天津大学 GIE工作室2020年2月8日V1.0
                  2020年12月13日V2.0
关注b站up主：GIE工作室，获得更多干货与信息
*/

// import org.apache.http.client.utils.URIBuilder; // The package is not usable!

public class MainActivity extends Activity {

    public final static String HTTPGET = "GET";
    public final static String HTTPPUT = "PUT";
    public final static String HTTPPOST = "POST";
    public final static String HTTPDELETE = "DELETE";
    public final static String HTTPACCEPT = "Accept";
    public final static String CONTENT_LENGTH = "Content-Length";
    public final static String CHARSET_UTF8 = "UTF-8";
    public static String fff_time = "20190124 010101";
    public String save_data_final;
    public String save_data_old;
    public String [] Weather_log={"1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1",};//存储24小时内的数据
    public String [] Setting_inf={"","","","","",""};//第0位节点名称，第1位设备ID，第2位应用ID，第3位应用秘钥，第4位仪表盘字体大小，第5位仪表盘字体位置
    public  String dev_id;
    public static String history_result;
    public static IoTData data,data_his;
    public static int flag_tt=1;
    private static HttpClient httpClient;
    public static int fir_flag=1;
    private static TextView clock_txt;
    private static TextView warning_txt_r;
    private static TextView warning_txt_y;
    private static TextView tx01;
    private Handler handler = new Handler();
    LineData mLineData1;
    LineChart chart1;
    clwater.androiddashboard.DashBoard d1;
    clwater.androiddashboard.DashBoard d2;

    private Button mBtn_Menu;
    private Button mBtn_Return;
    private ImageView cover_img;

    int line_count=0;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        tx01 = (TextView) findViewById(R.id.tx01);
        clock_txt = (TextView) findViewById(R.id.clock_txt);
        warning_txt_r = (TextView) findViewById(R.id.warning_txt_r);
        warning_txt_y = (TextView) findViewById(R.id.warning_txt_y);
        data = new IoTData();
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_NAVIGATION);
        chart1 = (LineChart) findViewById(R.id.chart1);
        setChartStyle(chart1, mLineData1, Color.parseColor("#000044")," ");
        d1 = (clwater.androiddashboard.DashBoard) findViewById(R.id.dash1);
        d2=(clwater.androiddashboard.DashBoard) findViewById(R.id.dash2);
        mBtn_Menu=(Button)findViewById(R.id.btn_menu);
        cover_img = (ImageView) findViewById(R.id.cover);
        mBtn_Return=(Button)findViewById(R.id.btn_return);
        mBtn_Menu.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent=new Intent(MainActivity.this,ButtonActivity.class);
                startActivity(intent);
            }
        });
        mBtn_Return.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent=new Intent(MainActivity.this,MainActivity.class);
                fir_flag=0;
                flag_tt=1;
               // mBtn_Return.setVisibility(View.INVISIBLE);
                intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK|Intent.FLAG_ACTIVITY_NEW_TASK);
                startActivity(intent);
            }
        });
        new Thread(new Runnable() {
            @Override
            public void run() {//主函数入口
                if(fir_flag==1) {
                    try {
                        fir_flag=0;
                        Thread.sleep(2000);//显示2s启动界面
                        cover_img.setVisibility(View.INVISIBLE);
                        //mBtn_Return.setVisibility(View.INVISIBLE);

                    }//隐藏启动界面
                    catch (Exception e) {

                    }
                }
                else {
                    cover_img.setVisibility(View.INVISIBLE);
                }
                while (true) {//主循环
                    read_setting();
                    try {
                        cert();
                    } catch (Exception e) {
                        Looper.prepare();
                        Toast.makeText(MainActivity.this, "配置错误，请检查网络或节点设置", Toast.LENGTH_LONG).show();
                        Looper.loop();

                    } finally {
                    }
                    try {
                        String result ;
                        if(flag_tt==1) {
                            flag_tt=0;
                            result = queryNode_OneNet(dev_id, "24");//使用提前设置的设备ID和key查询设备信息，返回查询结果
                            parseIoTString24(result);
                            result = queryNode_OneNet(dev_id, "1");
                            if(parseIoTString(result)) {
                                String rff_time;
                                rff_time = data.getTime();
                                fff_time = rff_time.substring(0, 16);
                            }
                        }
                        else
                        {
                            result = queryNode_OneNet(dev_id, "1");
                            if(parseIoTString(result)) {
                            String rff_time;
                            rff_time = data.getTime();
                            fff_time = rff_time.substring(0, 16);
                            }
                        }
                    } catch (Exception e) {
                        Log.d("MainActivity", "++++++++++++++++");
                    }
                    handler.post(new Runnable() {
                        @Override
                        public void run() {
                                reportEnv();
                                check_value();
                        }
                    });
                    try {
                        Thread.sleep(50000);
                    } catch (Exception e) {

                    }

                }

            }
        }).start();
        new TimeThread().start();
    }
    /**
     * 获取系统时间并显示
     */
    class TimeThread extends Thread {
        @Override
        public void run() {
            do {
                try {
                    Thread.sleep(1000);
                    Message msg = new Message();
                    msg.what = 1;  //消息(一个整型值)
                    mHandler.sendMessage(msg);// 每隔1秒发送一个msg给mHandler
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            } while (true);
        }
    }
    private Handler mHandler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case 1:
                    clock_refresh();
                break;
                default:
                    break;

            }
        }
    };
    private  void clock_refresh(){
        long sysTime = System.currentTimeMillis();
        CharSequence sysTimeStr = DateFormat.format("yyyy-MM-dd\nHH:mm:ss", sysTime);
        clock_txt.setText(sysTimeStr);
    }
    /**
     * 根据所得值给出报警和临界提示
     */
    private void check_value(){//
        String tx_disp_y=" ",tx_disp_r=" ";
        if(data.getTemperature()>=30.0f&&data.getTemperature()<=34.0f)
        {
            tx_disp_y+="高温临界\n";
        }
        else if(data.getTemperature()>34.0f)
        {
            tx_disp_r+="高温报警\n";
        }
        else if(data.getTemperature()<10.0f)
        {
            tx_disp_r+="低温报警\n";
        }
        if(data.getHumidity()>=80.0f&&data.getHumidity()<=90.0f)
        {
            tx_disp_y+="高湿临界\n";
        }
        else if(data.getHumidity()>90.0f)
        {
            tx_disp_r+="高湿报警\n";
        }
        else if(data.getHumidity()<40.0f)
        {
            tx_disp_r+="低湿报警\n";
        }
        if(data.getPm2_5()>=800f&&data.getPm2_5()<=1000f)
        {
            tx_disp_y+="PM2.5临界\n";
        }
        else if(data.getPm2_5()>1000f)
        {
            tx_disp_r+="PM2.5报警\n";
        }
        if(data.getTVOC()>=30f&&data.getTVOC()<=45f)
        {
            tx_disp_y+="TVOC临界\n";
        }
        else if(data.getTVOC()>45f)
        {
            tx_disp_r+="TVOC报警\n";
        }
        if(data.getvoltage()>=3.0f&&data.getvoltage()<=3.2f)
        {
            tx_disp_y+="电压临界\n";
        }
        else if(data.getvoltage()<3.0f)
        {
            tx_disp_r+="电压报警\n";
        }
        if(data.getUvv()>=100f&&data.getUvv()<=200f)
        {
            tx_disp_y+="紫外线临界\n";
        }
        else if(data.getUvv()>200f)
        {
            tx_disp_r+="紫外线报警\n";
        }
            warning_txt_y.setText(tx_disp_y);
            warning_txt_r.setText(tx_disp_r);
    }

    /**
     * 从文件中获取相关配置，如果未读取到配置则创建配置文件模板并告知用户进行节点相关配置。
     */
    private  void read_setting() {
        int line_log=0;
    String setting_file = Environment.getExternalStorageDirectory().toString() + File.separator +"Weather_station_settingsV3.txt";
    File fs_inf=new File(setting_file);
        try {
            InputStream instream = new FileInputStream(fs_inf);//尝试打开配置文件
            if (instream != null)
            {
                InputStreamReader inputreader = new InputStreamReader(instream);
                BufferedReader buffreader = new BufferedReader(inputreader);
                String line;
                //分行读取配置信息
                while ((( line = buffreader.readLine()) != null)&&(line_log<=5)) {
                    Setting_inf[line_log] = line+"\n" ;
                    line_log++;
                }
                instream.close();
                if(Setting_inf[1].length()<=17)//检查读取到的设置是否为空。
                    dev_id="0";
                else
                    dev_id=Setting_inf[1].substring(Setting_inf[1].indexOf(">")+1,Setting_inf[1].indexOf("\n"));//截取并更新配置
                if(Setting_inf[2].length()<=17)
                    SECRET_OneNet="0";
                else
                    SECRET_OneNet=Setting_inf[2].substring(Setting_inf[2].indexOf(">")+1,Setting_inf[2].indexOf("\n"));
                if(Setting_inf[3].length()<=11)
                    size_setting=Integer.valueOf("15").intValue();
                else
                    size_setting=Integer.valueOf(Setting_inf[3].substring(Setting_inf[3].indexOf(">")+1,Setting_inf[3].indexOf("\n"))).intValue();
                if(Setting_inf[4].length()<=10)
                    pos_setting=Integer.valueOf("20").intValue();
                else
                    pos_setting=Integer.valueOf(Setting_inf[4].substring(Setting_inf[4].indexOf(">")+1,Setting_inf[4].indexOf("\n"))).intValue();

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
    /**
     * 将获取的数据保存到本地文件中，V3.0已去除该功能，折线图也不再依赖该函数
     * */
//    private void saveData(String finalContent) {
//        if(!finalContent.equals(save_data_old))
//        {
//            line_count=0;
//            String data_Dir = Environment.getExternalStorageDirectory().toString() + File.separator +"Weather_data.txt";//自运行以来所有接收到的数据
//            String temp_data_Dir = Environment.getExternalStorageDirectory().toString() + File.separator +"24h_Weather_data.log";//过去24小时的数据，仅用于绘制折线图
//            File fs = new File(data_Dir);
//            File fs_log = new File(temp_data_Dir);
//
//            try {
//                InputStream instream = new FileInputStream(fs_log);
//                if (instream != null)
//                {
//                    InputStreamReader inputreader = new InputStreamReader(instream);
//                    BufferedReader buffreader = new BufferedReader(inputreader);
//                    String line;
//                    //分行读取
//                    while ((( line = buffreader.readLine()) != null)&&(line_count<=23)) {
//                        Weather_log[line_count] = line+"\n" ;
//                        line_count++;
//                    }
//                    instream.close();
//                }
//            }
//            catch (java.io.FileNotFoundException e)//未找到最近24h记录文件则创建之
//            {
//                try {
//                    FileOutputStream outStream_log =new FileOutputStream(fs_log,true);
//                    if(save_data_final!=null) {
//                        outStream_log.write(finalContent.getBytes("gbk"));
//                        outStream_log.write(finalContent.getBytes("gbk"));
//                    }
//                    outStream_log.flush();
//                    outStream_log.close();
//                } catch (FileNotFoundException ee) {
//                    e.printStackTrace();
//                }
//                catch (IOException ee) {
//                    e.printStackTrace();
//                }
//            }
//            catch (IOException e)
//            {
//                e.printStackTrace();
//            }
//            if(line_count==0)//避免数组越界，如果为空文件则不再继续
//                return;
//            if(!(save_data_final).equals(Weather_log[line_count-1]))//比较当前获得的数据与最新一行数据是否相同，不同才写入，避免重复行
//            {
//                try {
//                    FileOutputStream outStream;
//                    if(line_count>=23)//如果积累到24小时的数据则删除最早的数据，加入最新的数据（类似FIFO队列）
//                    {
//                        fs_log.delete();
//                        fs_log = new File(temp_data_Dir);
//                        outStream =new FileOutputStream(fs_log,true);
//                        for(int co=1;co<23;co++) {
//                            outStream.write(Weather_log[co].getBytes("gbk"));
//                        }
//                        outStream.write(save_data_final.getBytes("gbk"));
//                        outStream.flush();
//                        outStream.close();
//                        save_data_old = save_data_final;
//                    }
//                    else {
//                        outStream = new FileOutputStream(fs_log, true);
//                        outStream.write(finalContent.getBytes("gbk"));
//                        outStream.flush();
//                        outStream.close();
//                        save_data_old = save_data_final;
//                    }
//                } catch (FileNotFoundException e) {
//                    e.printStackTrace();
//                } catch (UnsupportedEncodingException e) {
//                    e.printStackTrace();
//                } catch (IOException e) {
//                    e.printStackTrace();
//                }
//                try {
//                    FileOutputStream outStream =new FileOutputStream(fs,true);
//                    outStream.write(finalContent.getBytes("gbk"));
//                    outStream.flush();
//                    outStream.close();
//                    save_data_old=save_data_final;
//                } catch (FileNotFoundException e) {
//                    e.printStackTrace();
//                } catch (UnsupportedEncodingException e) {
//                    e.printStackTrace();
//                } catch (IOException e) {
//                    e.printStackTrace();
//                }
//
//            }
//
//        }
//    }
    /**
     * Classify the environment status.
     */
    private void reportEnv() {
        // Thresholds

        String text_output = "环境参数\n";
        float value;
        String rf_time,save_temp_data;
        value = data.getTemperature();
       // text_output += "室温 " + Float.toString(value) + "℃  " + "\n";
        save_temp_data= Float.toString(value) + "C  ";

        value = data.getHumidity();
       // text_output += "湿度 " + Float.toString(value) + "%  " + "\n";
        save_temp_data+= Float.toString(value) + "%  ";

        value = data.getPressure();
        text_output += "气压：" + Float.toString(value) + "Pa  " + "\n";
        save_temp_data+=Float.toString(value) + "Pa  ";

        value = data.getPm2_5();
        text_output += "PM2.5：" + Float.toString(value) + "ug  " + "\n";
        save_temp_data+=Float.toString(value) + "ug ";

        value = data.getUvv();
        text_output += "紫外线：" + Float.toString(value) + "uW  " + "\n";
        save_temp_data+=Float.toString(value) + "uW ";

        value = data.getGas_r();
        text_output += "烟阻：" + Float.toString(value) + "hOhm  " + "\n";
        save_temp_data+=Float.toString(value) + "hOhm ";

        value = data.getCO2();
        text_output += "eCO2：" + Float.toString(value) + "ppm  " + "\n";
        save_temp_data+=Float.toString(value) + "ppm ";

        value = data.getTVOC();
        text_output += "TVOC：" + Float.toString(value) + "ppb  "+"\n";
        save_temp_data+=Float.toString(value) + "T  ";

        value = data.getvoltage();
        text_output += "电量：" + Float.toString(value) + "V  " + "\n";
        save_temp_data+=Float.toString(value) + "V ";

        rf_time = fff_time;
        text_output += "更新时间" + (rf_time) ;
        save_temp_data+=rf_time+"\n" ;

        save_data_final=save_temp_data;
        tx01.setText(text_output);
        //saveData(save_data_final);
        show_data();

    }
    /**
     * 调用相关库显示数据
     * */
    private void show_data()
    {
        float temp_tt,temp_ht;
         mLineData1 = makeLineData(line_count,Color.CYAN);//根据读取的历史数据制作折线图库能够识别的数据格式
        chart1.setData(mLineData1);//将数据装入折线图
        chart1.setAutoScaleMinMaxEnabled(true);
        chart1.invalidate();//刷新折线图显示使数据生效
        temp_tt=Float.parseFloat(save_data_final.substring(0,save_data_final.indexOf("C")));//从最新数据中截取出浮点格式温度值
        temp_ht=Float.parseFloat(save_data_final.substring(save_data_final.indexOf("C")+2,save_data_final.indexOf("%")));//从最新数据中截取出浮点格式湿度值
        d1.setBackGroundColor(Color.parseColor("#0B1557"));
        d1.setMode(0);
       // d1.setTitleText("温度","℃");
        d1.setScale(-15,5);
        d1.setNumber(save_data_final.substring(0,save_data_final.indexOf("C")));
        if(temp_tt<-15)
            temp_tt=-15;
        else if(temp_tt>45)
            temp_tt=45;
        d1.cgangePer((temp_tt+15)/(60f));
        d2.setBackGroundColor(Color.parseColor("#0B1557"));
        d2.setMode(1);
        //d2.setTitleText("湿度","%");
        d2.setScale(0,8);
        d2.setNumber(save_data_final.substring(save_data_final.indexOf("C")+2,save_data_final.indexOf("%")));
        d2.cgangePer(temp_ht/(100f));
    }
    /**
     * 以下相关调用折线图代码均来自互联网
     * */
    private void setChartStyle(LineChart mLineChart, LineData lineData,int color,String description) {
        // 是否在折线图上添加边框
        mLineChart.setDrawBorders(false);
        mLineChart.setDescription(description);
        // 如果没有数据的时候，会显示这个，类似listview的emtpyview
        mLineChart
                .setNoDataTextDescription("加载中...");

        mLineChart.setDrawGridBackground(false);
        mLineChart.setGridBackgroundColor(Color.CYAN);

        // 触摸
        mLineChart.setTouchEnabled(true);

        // 拖拽
        mLineChart.setDragEnabled(true);

        // 缩放
        mLineChart.setScaleEnabled(true);

        mLineChart.setPinchZoom(false);
        // 隐藏右边 的坐标轴
        mLineChart.getAxisRight().setEnabled(false);
        // 让x轴在下面
        mLineChart.getXAxis().setPosition(XAxis.XAxisPosition.BOTTOM);

        mLineChart.getAxisRight().setEnabled(true); // 隐藏右边 的坐标轴(true不隐藏)
        mLineChart.getXAxis().setPosition(XAxis.XAxisPosition.BOTTOM); // 让x轴在下面
        mLineChart.getXAxis().setTextColor(Color.WHITE);
        mLineChart.getAxisRight().setTextColor(Color.WHITE);
        mLineChart.getAxisLeft().setTextColor(Color.WHITE);
        // 设置背景
        mLineChart.setBackgroundColor(color);

        // 设置x,y轴的数据
        mLineChart.setData(lineData);
        mLineChart.setAutoScaleMinMaxEnabled(true);
        mLineChart.invalidate();

    }
    private LineData makeLineData(int count,int color_fill) {
        ArrayList<String> x = new ArrayList<String>();
        for (int i = 0; i < count; i++) {
            // x轴显示的数据
            x.add(i+"h");
        }
        ArrayList<Entry> y = new ArrayList<Entry>();
        // y轴的数据
            for (int i = 0; i < count; i++) {
                float val = Float.parseFloat(Weather_log[i]);
                Entry entry = new Entry(val, i);
                y.add(entry);
            }

        // y轴数据集
        count++;
        LineDataSet mLineDataSet = new LineDataSet(y, " ");

        // 用y轴的集合来设置参数
        // 线宽
        mLineDataSet.setLineWidth(3.0f);

        // 显示的圆形大小
        mLineDataSet.setCircleSize(5.0f);

        // 折线的颜色
        mLineDataSet.setColor(Color.WHITE);

        // 圆球的颜色
        mLineDataSet.setCircleColor(Color.WHITE);

        // 设置mLineDataSet.setDrawHighlightIndicators(false)后，
        // Highlight的十字交叉的纵横线将不会显示，
        // 同时，mLineDataSet.setHighLightColor(Color.CYAN)失效。
        mLineDataSet.setDrawHighlightIndicators(true);

        // 按击后，十字交叉线的颜色
        mLineDataSet.setHighLightColor(Color.WHITE);

        // 设置这项上显示的数据点的字体大小。
        mLineDataSet.setValueTextSize(10.0f);

        // mLineDataSet.setDrawCircleHole(true);

        // 改变折线样式，用曲线。
         mLineDataSet.setDrawCubic(true);
        // 默认是直线
        // 曲线的平滑度，值越大越平滑。
         mLineDataSet.setCubicIntensity(0.2f);
        mLineDataSet.setDrawValues(false);

        // 填充曲线下方的区域，红色，半透明。
         mLineDataSet.setDrawFilled(true);
         mLineDataSet.setFillAlpha(128);
         mLineDataSet.setFillColor(color_fill);

        // 填充折线上数据点、圆球里面包裹的中心空白处的颜色。
        mLineDataSet.setCircleColorHole(Color.LTGRAY);

        // 设置折线上显示数据的格式。如果不设置，将默认显示float数据格式。
        mLineDataSet.setValueFormatter(new ValueFormatter() {


            @Override
            public String getFormattedValue(float value, Entry entry,
                                            int dataSetIndex, ViewPortHandler viewPortHandler) {
                // TODO Auto-generated method stub
                int n = (int) value;
                String s = "y:" + n;
                return s;
            }
        });

        ArrayList<LineDataSet> mLineDataSets = new ArrayList<LineDataSet>();
        mLineDataSets.add(mLineDataSet);

        LineData mLineData = new LineData(x, mLineDataSets);

        return mLineData;
    }

    private boolean parseIoTString(String str) throws Exception {
        try {
            JSONObject top = new JSONObject(str);
            JSONObject obj = top.getJSONObject("data");
            JSONArray jarray = obj.getJSONArray("datastreams");
            JSONObject obj0,obj2;JSONArray arr1;
            String num_type,num;
            for(int ixx=0;ixx<9;ixx++) {
               obj0 = jarray.getJSONObject(ixx);
               num_type = obj0.getString("id");
               arr1 = obj0.getJSONArray("datapoints");
               obj2 = arr1.getJSONObject(0);
               data.setTime(obj2.getString("at"));
               num = obj2.getString("value");
               if(num_type.equals("temp"))
               {
                   data.setTemperature(Float.valueOf(num));
               }
               else if(num_type.equals("humi"))
               {
                   data.setHumidity(Float.valueOf(num));
               }
               else if(num_type.equals("press"))
               {
                   data.setPressure(Float.valueOf(num));
               }
               else if(num_type.equals("GR"))
               {
                   data.setGas_r(Float.valueOf(num));
               }
               else if(num_type.equals("UV"))
               {
                   data.setUvv(Float.valueOf(num));
               }
               else if(num_type.equals("PM2.5"))
               {
                   data.setPm2_5(Float.valueOf(num));
               }else if(num_type.equals("eCO2"))
               {
                   data.setCO2(Float.valueOf(num));
               }
               else if(num_type.equals("TVOC"))
               {
                   data.setTVOC(Float.valueOf(num));
               }
               else if(num_type.equals("bat"))
               {
                   data.setvoltage(Float.valueOf(num));
               }
            }
        } catch (Exception e) {
            System.out.println("Format error in acquired data!");
            return false;
        }

        return true;
    }
    private boolean parseIoTString24(String str) throws Exception {
        try {
            JSONObject top = new JSONObject(str);
            JSONObject obj = top.getJSONObject("data");
            JSONArray jarray = obj.getJSONArray("datastreams");
            JSONObject obj0,obj2;JSONArray arr1;
            String num_type;
            for(int ixx=0;ixx<9;ixx++) {
                obj0 = jarray.getJSONObject(ixx);
                num_type = obj0.getString("id");
                if(num_type.equals("temp"))
                {
                    int ixxx=0;
                    arr1 = obj0.getJSONArray("datapoints");
                    try{

                        for(ixxx=0;ixxx<24;ixxx++) {
                            obj2 = arr1.getJSONObject(ixxx);
                            Weather_log[ixxx] = obj2.getString("value");
                            }
                         }catch (Exception e) {
                            line_count=ixxx;
                            return true;
                        }
                    line_count=24;
                    return true;
                }
            }
        } catch (Exception e) {
            System.out.println("Format error in acquired data!");
            return false;
        }

        return true;
    }
/**
 * 获取节点信息
 * */
public String queryNode_OneNet(String devId, String num_limit) throws Exception {
    String result = null;
    Map<String, String> header = new HashMap<>();
    header.put(Constant.OneNet_API_KEY, SECRET_OneNet);
    //header.put(Constant.HEADER_APP_AUTH, "Bearer" + " " + accessToken);
    header.put("Content-Type", "application/json");

    String myUrl = Constant.OneNet_API_URL + "/" + devId + "/datapoints" +  "?limit=" +  num_limit;

    HttpGet request = new HttpGet(myUrl);
    addRequestHeader(request, header);

    HttpResponse response = executeHttpRequest(request);

    if (response.getStatusLine().getStatusCode() == 200) {
        //System.out.print(bodyQueryDevices.getStatusLine());
        result = convertStreamToString(response.getEntity().getContent());
        System.out.println("QueryDevices, response content:" + result);
    }
    history_result=result;
    return result;
}

    private static void addRequestHeader(HttpUriRequest request,
                                         Map<String, String> headerMap) {
        if (headerMap == null) {
            return;
        }

        for (String headerName : headerMap.keySet()) {
            if (CONTENT_LENGTH.equalsIgnoreCase(headerName)) {
                continue;
            }
            String headerValue = headerMap.get(headerName);
            request.addHeader(headerName, headerValue);
        }
    }

    /**
     * Do certification and login.
     *
     * @return Return token if connection was accepted, else return null.
     * @throws Exception
     */
    public String cert() throws Exception {
        String appId = APPID;
        String secret = Constant.SECRET;
        String urlLogin = Constant.APP_AUTH;
        String token = null;

        // 服务器端需要验证的客户端证书
        KeyStore keyStore = KeyStore.getInstance("PKCS12");
        // 客户端信任的服务器端证书
        KeyStore trustStore = KeyStore.getInstance("BKS");

        InputStream ksIn = getApplicationContext().getAssets().open(Constant.SELFCERTPATH);
        InputStream tsIn = getApplicationContext().getAssets().open(Constant.TRUSTCAPATH);
        try {
            keyStore.load(ksIn, Constant.SELFCERTPWD.toCharArray());
            trustStore.load(tsIn, Constant.TRUSTCAPWD.toCharArray());
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                ksIn.close();
            } catch (Exception ignore) {
            }
            try {
                tsIn.close();
            } catch (Exception ignore) {
            }
        }

//        SSLContext sc = SSLContext.getInstance("TLS");
        TrustManagerFactory trustManagerFactory = TrustManagerFactory.getInstance(TrustManagerFactory.getDefaultAlgorithm());
        trustManagerFactory.init(trustStore);
        KeyManagerFactory keyManagerFactory = KeyManagerFactory.getInstance("X509");
        keyManagerFactory.init(keyStore, Constant.SELFCERTPWD.toCharArray());
//        sc.init(keyManagerFactory.getKeyManagers(), trustManagerFactory.getTrustManagers(), null);

        SSLSocketFactory ssl = new SSLSocketFactory(keyStore, Constant.SELFCERTPWD, trustStore);
        ssl.setHostnameVerifier(new AllowAllHostnameVerifier());
//        ssl.getSocketFactory().setHostnameVerifier(new AllowAllHostnameVerifier());
        httpClient = new DefaultHttpClient();
        if (ssl != null) {
            Scheme sch = new Scheme("https", ssl, 443);
            httpClient.getConnectionManager().getSchemeRegistry().register(sch);
        }
        Map<String, String> param = new HashMap<>();
        param.put("appId", appId);
        param.put("secret", secret);

        /*List<NameValuePair> nvps = new LinkedList<NameValuePair>();
        Set<Map.Entry<String, String>> paramsSet = param.entrySet();
        for (Map.Entry<String, String> paramEntry : paramsSet) {
            nvps.add(new BasicNameValuePair(paramEntry.getKey(), paramEntry.getValue()));
        }*/
        List<NameValuePair> nvps = paramsConverter(param);
        HttpPost request = new HttpPost(urlLogin);
        request.setEntity(new UrlEncodedFormEntity(nvps));
        HttpResponse response = executeHttpRequest(request);

//        KeyStore selfCert = KeyStore.getInstance("pkcs12");
//        selfCert.load(getApplicationContext().getAssets().open(Constant.SELFCERTPATH),
//                Constant.SELFCERTPWD.toCharArray());
////		KeyManagerFactory kmf = KeyManagerFactory.getInstance(KeyManagerFactory.getDefaultAlgorithm());
//        KeyManagerFactory kmf = KeyManagerFactory.getInstance("x509");
//        kmf.init(selfCert, Constant.SELFCERTPWD.toCharArray());
//
//
////		keyManagerFactory.init(clientKeyStore, "123456".toCharArray());
//
//        // 2 Import the CA certificate of the server,
//        KeyStore caCert = KeyStore.getInstance("bks");
//        caCert.load(getApplicationContext().getAssets().open(Constant.TRUSTCAPATH), Constant.TRUSTCAPWD.toCharArray());
//        TrustManagerFactory tmf = TrustManagerFactory.getInstance("x509");
//        tmf.init(caCert);
//
//        SSLContext sc = SSLContext.getInstance("TLS");
//        sc.init(kmf.getKeyManagers(), tmf.getTrustManagers(), null);
//        OkHttpClient client = new OkHttpClient();
//
//        if (sc != null) {
//            client.newBuilder().sslSocketFactory(sc.getSocketFactory());
//        }
//        client.newBuilder().hostnameVerifier(SSLSocketFactory.ALLOW_ALL_HOSTNAME_VERIFIER);
//        RequestBody requestBodyPost = new FormBody.Builder()
//                .add("appId", appId)
//                .add("secret", secret)
//                .build();
//        Request requestPost = new Request.Builder()
//                .url(urlLogin)
//                .post(requestBodyPost)
//                .build();
//
////        Response response = client.newCall(requestPost).execute();
//        client.newCall(requestPost).enqueue(new Callback() {
//            @Override
//            public void onFailure(Call call, IOException e) {
//                Log.d("MainActivity","The response is failure");
//            }
//
//            @Override
//            public void onResponse(Call call, Response response) throws IOException {
//                final String string = response.body().string();
//                Log.d("MainActivity","The response is "+response.body().string());
//                runOnUiThread(new Runnable() {
//                    @Override
//                    public void run() {
//
//                    }
//                });
//            }
//        });

//        if (response.isSuccessful()) {
////            return response.body().string();
//            Log.d("MainActivity","The response is "+response.body().string());
//        } else {
//            throw new IOException("Unexpected code " + response);
//        }
        if (response.getStatusLine().getStatusCode() == 200) {
            try {
                String result = convertStreamToString(response.getEntity().getContent());
                JSONObject json = new JSONObject(result);
                token = json.getString("accessToken");
            } catch (Exception e) {
                //e.printStackTrace();
            }
        }

        return token;
    }

    private HttpResponse executeHttpRequest(HttpUriRequest request) {
        HttpResponse response = null;
        try {
            response = httpClient.execute(request);
        } catch (Exception e) {
            System.out.println("executeHttpRequest failed.");
        } finally {
            if (response.getStatusLine().getStatusCode() == 200) {
                System.out.println("executeHttpRequest succeeded.");
               /* try {
                    result = convertStreamToString(response.getEntity().getContent());
                    System.out.println("result code" + result);
                 } catch (Exception e) {
                    e.printStackTrace();
                }*/
            } else {
                try {
                    Log.e(TAG, "CommonPostWithJson | response error | "
                            + response.getStatusLine().getStatusCode()
                            + " error :"
                            + EntityUtils.toString(response.getEntity()));
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
//            try {
//                System.out.println("aaa  ==  response -> " + response);
//                HttpEntity entity = response.getEntity();
//
//            } catch (Exception e) {
//                System.out.println("IOException: " + e.getMessage());
//            }
        }

        return response;
    }

    public static String convertStreamToString(InputStream is) {
        /*
         * To convert the InputStream to String we use the BufferedReader.readLine()
         * method. We iterate until the BufferedReader return null which means
         * there's no more data to read. Each line will appended to a StringBuilder
         * and returned as String.
         */
        BufferedReader reader = new BufferedReader(new InputStreamReader(is));
        StringBuilder sb = new StringBuilder();

        String line = null;
        try {
            while ((line = reader.readLine()) != null) {
                sb.append(line + "\n");
            }
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                is.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        return sb.toString();
    }

    private List<NameValuePair> paramsConverter(Map<String, String> params) {
        List<NameValuePair> nvps = new LinkedList<NameValuePair>();
        Set<Map.Entry<String, String>> paramsSet = params.entrySet();
        for (Map.Entry<String, String> paramEntry : paramsSet) {
            nvps.add(new BasicNameValuePair(paramEntry.getKey(),
                    paramEntry.getValue()));
        }

        return nvps;
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    public static String timeZoneTransfer(String time, String pattern, String nowTimeZone, String targetTimeZone) {
        SimpleDateFormat simpleDateFormat = new SimpleDateFormat(pattern);
        simpleDateFormat.setTimeZone(TimeZone.getTimeZone("GMT" + nowTimeZone));
        Date date;
        try {
            date = simpleDateFormat.parse(time);
        } catch (ParseException e) {
           // logger.error("时间转换出错。", e);
            return "";
        }
        simpleDateFormat.setTimeZone(TimeZone.getTimeZone("GMT" + targetTimeZone));
        return simpleDateFormat.format(date);
    }
}
