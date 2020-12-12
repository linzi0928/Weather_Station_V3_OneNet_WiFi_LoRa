package clwater.androiddashboard;

import android.animation.ValueAnimator;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PaintFlagsDrawFilter;
import android.graphics.Path;
import android.graphics.RectF;
import android.graphics.SweepGradient;
import android.support.annotation.Nullable;
import android.util.AttributeSet;
import android.view.View;
import android.view.animation.OvershootInterpolator;

import com.why.project.densityutildemo.utildemo.DensityUtil;


/**
 * 折线图相关函数
 * Created by gengzhibo on 17/8/3.
 * Edited by GIE Studio on 20/2/8.
 */
public class DashBoard extends View {



    private Paint paint , tmpPaint , textPaint ,  strokePain;
    private RectF rect;
    private int backGroundColor=Color.parseColor("#0B1557");    //背景色
    private float pointLength;      //指针长度
    private float per ;             //指数百分比
    private float perPoint ;        //缓存(变化中)指针百分比
    private float perOld ;          //变化前指针百分比
    private float length ;          //仪表盘半径
    private float r ;
    private  float scale_start=0;
    private  float scale_step=20;
    private  String num_p="20";
    private  int disp_mode=0;
    public static int size_setting=15;
    public static int pos_setting=20;
    public DashBoard(Context context) {
        super(context);
        init();
    }



    public DashBoard(Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        init();
    }


    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        int width = MeasureSpec.getSize(widthMeasureSpec);
        int heitht = width / 2 / 4 * 5;
        initIndex(width / 2);
        //优化组件高度
        setMeasuredDimension(width, heitht);
    }



    private void initIndex(int specSize) {
        r = specSize;
        length = r  / 4 * 3;
        pointLength =  - (float) (r *  0.6);
        per = 0;
        perOld = 0;
    }


    private void init() {
        paint = new Paint();
        rect = new RectF();
        textPaint = new Paint();
        tmpPaint = new Paint();
        strokePain = new Paint();
    }

    public DashBoard(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }


    public void setR(float r) {
        this.r = r;
        this.length = r  / 4 * 3;
    }

    @Override
    protected void onDraw(Canvas canvas) {

        setLayerType(LAYER_TYPE_SOFTWARE, null);
        //颜色指示的环
        initRing(canvas);
        //刻度文字
        initScale(canvas);
        //指针
        initPointer(canvas);
        //提示内容
        initText(canvas);
    }

    private void initText(Canvas canvas) {
        //抗锯齿
        canvas.setDrawFilter(new PaintFlagsDrawFilter(0, Paint.ANTI_ALIAS_FLAG|Paint.FILTER_BITMAP_FLAG));
        canvas.restore();
        canvas.save();
        canvas.translate(canvas.getWidth()/2, r);

        float rIndex = length ;

        //设置文字展示的圆环
        paint.setColor(Color.WHITE);
        paint.setShader(null);
        paint.setShadowLayer(5, 0, 0, 0x54000000);
        rect = new RectF( - (rIndex/ 3 ), - (rIndex / 3), rIndex / 3, rIndex / 3);
        canvas.drawArc(rect, 0, 360, true, paint);

        paint.clearShadowLayer();

        canvas.restore();
        canvas.save();
        canvas.translate(canvas.getWidth()/2f , r);


        textPaint.setStrokeWidth(1);
        textPaint.setAntiAlias(true);

        textPaint.setTextSize(DensityUtil.dip2px(getContext(),size_setting));
        textPaint.setColor(Color.parseColor("#fc6555"));
        textPaint.setTextAlign(Paint.Align.RIGHT);


        //判断指数变化及颜色设定

        int _per = (int) (per * 120);

        if(disp_mode==0) {
            if (_per < 60) {//低于15度时蓝色字体
                textPaint.setColor(Color.parseColor("#0000AA"));
            } else if (_per < 90) {//高于15度低于30度绿色字体
                textPaint.setColor(Color.parseColor("#2C6D1B"));
            } else {//高于30度红色字体
                textPaint.setColor(Color.parseColor("#D52B2B"));
            }
        }




        canvas.translate( 0 , 0);

        if(disp_mode==0) {
            canvas.drawText(num_p+"℃", DensityUtil.dip2px(getContext(),pos_setting), 0, textPaint);

            textPaint.setTextSize(DensityUtil.dip2px(getContext(),size_setting));
            textPaint.setTextAlign(Paint.Align.CENTER);
//            canvas.drawText("℃", 0, 0, textPaint);
//            textPaint.setTextAlign(Paint.Align.RIGHT);
            textPaint.setColor(Color.parseColor("#777777"));


            canvas.restore();
            canvas.save();
            canvas.translate(canvas.getWidth() / 2, r + length / 3 / 2);
            canvas.drawText("温度", DensityUtil.dip2px(getContext(),1), 0, textPaint);
        }
        else
        {
            canvas.drawText(num_p+"%", DensityUtil.dip2px(getContext(),pos_setting), 0, textPaint);

            textPaint.setTextSize(DensityUtil.dip2px(getContext(),size_setting));
            textPaint.setTextAlign(Paint.Align.CENTER);
//            canvas.drawText("%", 0, 0, textPaint);
//            textPaint.setTextAlign(Paint.Align.CENTER);
            textPaint.setColor(Color.parseColor("#777777"));


            canvas.restore();
            canvas.save();
            canvas.translate(canvas.getWidth() / 2, r + length / 3 / 2);
            canvas.drawText("湿度", DensityUtil.dip2px(getContext(),1), 0, textPaint);
        }


    }
    public  void setMode(int d_mode){this.disp_mode=d_mode;}

    public void setScale(float Scale_Start,float Scale_Step){this.scale_start=Scale_Start;this.scale_step=Scale_Step;}

  //  public void setTitleText(String title,String title_unit){this.title_dis=title;this.title_unit_dis=title_unit;}

    public void setNumber(String Num_disp){this.num_p=Num_disp;}

    public void setBackGroundColor(int color){
        this.backGroundColor = color;
    }

    public void setPointLength1(float pointLength1){
        this.pointLength = -length * pointLength1 ;
    }

    private void initScale(Canvas canvas) {
        canvas.restore();
        canvas.save();
        canvas.translate(canvas.getWidth()/2, r);
        paint.setColor(Color.WHITE);

        tmpPaint = new Paint(paint); //小刻度画笔对象
        tmpPaint.setStrokeWidth(1);
        tmpPaint.setTextSize(DensityUtil.dip2px(getContext(),size_setting-4));
        tmpPaint.setTextAlign(Paint.Align.CENTER);



        canvas.rotate(-90,0f,0f);

        float  y = length;
        y = - y;
        int count = 12; //总刻度数
        paint.setColor(Color.WHITE);

        float tempRou = 180 / 12f;

        paint.setColor(Color.WHITE);
        paint.setStrokeWidth(5);

        //绘制刻度和百分比
        for (int i = 0 ; i <= count ; i++){

            if (i % 2 == 0 ) {
                canvas.drawText(String.valueOf(scale_start+(i*scale_step)), 0, y - 20f, tmpPaint);
            }

            canvas.drawLine(0f, y , 0, y + length / 15, paint);


            canvas.rotate(tempRou,0f,0f);
        }

    }


    private void initPointer(Canvas canvas) {
        paint.setColor(Color.LTGRAY);


        canvas.restore();
        canvas.save();
        canvas.translate(canvas.getWidth()/2, r);
        float change;

        if (perPoint < 1 ){
            change = perPoint * 180;
        }else {
            change = 180;
        }

        //根据参数得到旋转角度
        canvas.rotate(-90 + change,0f,0f);

        //绘制三角形形成指针
        Path path = new Path();
        path.moveTo(0 , pointLength);
        path.lineTo(-10 , 0);
        path.lineTo(10,0);
        path.lineTo(0 , pointLength);
        path.close();

        canvas.drawPath(path, paint);

    }

    private void initRing(Canvas canvas) {
        paint.setAntiAlias(true);
        paint.setStrokeWidth(2);
        canvas.save();
        canvas.translate(canvas.getWidth()/2, r);

    if(disp_mode==0) {
            //前15渐变蓝环
            paint.setStyle(Paint.Style.FILL);
            int[] colors = {Color.parseColor("#0000FF"), Color.parseColor("#4BF9F9")};
            float[] positions = {0.5f - 10f / 180f * 0.5f, 0.5f + 0.5f * 5f / 6f};
            SweepGradient sweepGradient = new SweepGradient(0, 0, colors, positions);
            paint.setShader(sweepGradient);
            rect = new RectF(-length, -length, length, length);
            canvas.drawArc(rect, 110, 10f + 180f / 6f * 5f, true, paint);

            //15-30绿环
            paint.setStyle(Paint.Style.FILL);
            int[] colors3 = {Color.parseColor("#07C607"), Color.parseColor("#ACD608")};
            float[] positions3 = {0.5f - 10f / 180f * 0.5f, 0.5f + 0.5f * 5f / 6f};
            sweepGradient = new SweepGradient(0, 0, colors3, positions3);
            paint.setShader(sweepGradient);
            rect = new RectF(-length, -length, length, length);
            canvas.drawArc(rect, 270f, 50f, true, paint);

            //30之后绿色渐变红环
            paint.setStyle(Paint.Style.FILL);
            canvas.rotate(10, 0f, 0f);
            int[] colors2 = {Color.parseColor("#F87C2A"), Color.parseColor("#FF2222")};
            float[] positions2 = {0.5f + 0.5f * (144f / 180f), 1.0f};
            sweepGradient = new SweepGradient(0, 0, colors2, positions2);
            paint.setShader(sweepGradient);
            rect = new RectF(-length, -length, length, length);
            canvas.drawArc(rect, 180f + 180f * (125f / 180f), 180f / 6 + 55, true, paint);
        }
    else
    {
        //前15渐变蓝环
        paint.setStyle(Paint.Style.FILL);
        int[] colors = {Color.parseColor("#DE1010"), Color.parseColor("#00DDDD")};
        float[] positions = {0.5f - 10f / 180f * 0.5f, 0.8f + 0.5f * 5f / 6f};
        SweepGradient sweepGradient = new SweepGradient(0, 0, colors, positions);
        paint.setShader(sweepGradient);
        rect = new RectF(-length, -length, length, length);
        canvas.drawArc(rect, 170, 200f, true, paint);
    }


        canvas.restore();
        canvas.save();
        canvas.translate(canvas.getWidth()/2, r);

        strokePain = new Paint(paint);

        strokePain.setColor(0x3f979797);
        strokePain.setStrokeWidth(10);
        strokePain.setShader(null);
        strokePain.setStyle(Paint.Style.STROKE);
        canvas.drawArc(rect, 170, 200, true, strokePain);



        canvas.restore();
        canvas.save();
        canvas.translate(canvas.getWidth()/2, r);

        //底边水平
        paint.setShader(null);
        paint.setColor(backGroundColor);
        paint.setStyle(Paint.Style.FILL);
        canvas.drawRect(-length  , (float) (Math.sin(Math.toRadians(10) ) * length /3f * 2f), length  ,  (float) (Math.sin(Math.toRadians(10)) * length  + 100) , paint);
        canvas.drawRect(-length  , (float) (Math.sin(Math.toRadians(10) ) * length /3f * 2f), length  ,  (float) (Math.sin(Math.toRadians(10) ) * length /3f * 2f) , strokePain);


        //内部背景色填充
        paint.setColor(backGroundColor);
        paint.setShader(null);
        rect = new RectF( - (length - length / 3f  - 2), -(length / 3f * 2f - 2), length - length / 3f -2 , length / 3f * 2f - 2);
        canvas.drawArc(rect, 170, 200, true, strokePain);
        canvas.drawArc(rect, 0, 360, true, paint);



    }



    public void cgangePer(float per ){
        this.perOld = this.per;
        this.per = per;
        ValueAnimator va =  ValueAnimator.ofFloat(perOld,per);
        va.setDuration(1000);
        va.setInterpolator(new OvershootInterpolator());
        va.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
            @Override
            public void onAnimationUpdate(ValueAnimator animation) {
                perPoint = (float) animation.getAnimatedValue();
                invalidate();
            }
        });
        va.start();

    }
}