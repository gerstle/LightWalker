package com.inappropirates.util;	

import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.RectF;
import android.graphics.SweepGradient;
import android.os.Bundle;
import android.preference.DialogPreference;
import android.util.AttributeSet;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

public class ColorPickerPreference extends DialogPreference implements OnClickListener {
	private int mColor = Color.BLACK;
	private TextView mSplashText,mValueText;
    private Context mContext;

    private String mDialogMessage;
	
	public ColorPickerPreference(Context context, AttributeSet attrs) {
		super(context, attrs);

		mContext = context;
	}

	/**
	 * Interface describing a color change listener.
	 */
	public interface OnColorChangedListener {
		/**
		 * Method colorChanged is called when a new color is selected.
		 * @param color new color.
		 */
		void colorChanged(int color);
	}

	/**
	 * Inner class representing the color chooser.
	 */
	private class ColorPickerView extends View {
		private int CENTER_X = 400;
		private int CENTER_Y = 400;
		private int CENTER_RADIUS = 50;

		private Paint paint = null;
		private Paint mCenterPaint = null;
		private final int[] mColors = new int[] {
			0xFFFF0000, 0xFFFF00FF, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000,
			0xFF00FFFF, 0xFF00FF00,	0xFFFFFF00, 0xFFFF0000 };

		/**
		 * @param context
		 * @param listener
		 * @param color
		 */
		ColorPickerView(Context context, int initialColor) {
			super(context);

			mColor = initialColor;
			paint = new Paint(Paint.ANTI_ALIAS_FLAG);
			paint.setShader(new SweepGradient(0, 0, mColors, null));
			paint.setStyle(Paint.Style.STROKE);
			paint.setStrokeWidth(300);

			mCenterPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
			mCenterPaint.setColor(mColor);
			mCenterPaint.setStrokeWidth(5);
		}
		
        private boolean mTrackingCenter;
        private boolean mHighlightCenter;

		/**
		 * {@inheritDoc}
		 */
		@SuppressLint("DrawAllocation")
		protected void onDraw(Canvas canvas) {
			int centerX = getRootView().getWidth()/2 -
				(int)(paint.getStrokeWidth()/4);
			float r = CENTER_X - paint.getStrokeWidth()/2;

			canvas.translate(centerX, CENTER_Y);
			canvas.drawOval(new RectF(-r, -r, r, r), paint);
			canvas.drawCircle(0, 0, CENTER_RADIUS, mCenterPaint);

			if (mTrackingCenter) {
				int c = mCenterPaint.getColor();
				mCenterPaint.setStyle(Paint.Style.STROKE);
				
				 if (mHighlightCenter) {
	                 mCenterPaint.setAlpha(0xFF);
	             } else {
	            	 mCenterPaint.setAlpha(0x80);
	             }

				canvas.drawCircle(0, 0, CENTER_RADIUS +
					mCenterPaint.getStrokeWidth(), mCenterPaint);

				mCenterPaint.setStyle(Paint.Style.FILL);
				mCenterPaint.setColor(c);
			}
		}
		
		/**
		 * {@inheritDoc}
		 */
		protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
			int width = getRootView().getWidth();

			if (width == 0) {
				width = CENTER_X*2 + 50;
			}

			setMeasuredDimension(width, CENTER_Y * 2);
		}
		/**
		 * @param s
		 * @param d
		 * @param p
		 * @return
		 */
		private int ave(int s, int d, float p) {
			return s + Math.round(p * (d - s));
		}
		/**
		 * @param mColors
		 * @param unit
		 * @return
		 */
		private int interpColor(int mColors[], float unit) {
			if (unit <= 0) {
				return mColors[0];
			}

			if (unit >= 1) {
				return mColors[mColors.length - 1];
			}

			float p = unit * (mColors.length - 1);
			int i = (int) p;
			p -= i;

			// now p is just the fractional part [0...1) and i is the index
			int c0 = mColors[i];
			int c1 = mColors[i + 1];
			int a = ave(Color.alpha(c0), Color.alpha(c1), p);
			int r = ave(Color.red(c0), Color.red(c1), p);
			int g = ave(Color.green(c0), Color.green(c1), p);
			int b = ave(Color.blue(c0), Color.blue(c1), p);

			return Color.argb(a, r, g, b);
		}
		
        private static final float PI = 3.1415926f;

		/**
		 * {@inheritDoc}
		 */
		public boolean onTouchEvent(MotionEvent event) {
			float x = event.getX() - getRootView().getWidth()/2;
			float y = event.getY() - CENTER_Y;
            boolean inCenter = java.lang.Math.sqrt(x*x + y*y) <= CENTER_RADIUS;

			switch (event.getAction()) {
			case MotionEvent.ACTION_DOWN:
			case MotionEvent.ACTION_MOVE:
				if (!inCenter)
				{
                    float angle = (float)java.lang.Math.atan2(y, x);
                    // need to turn angle [-PI ... PI] into unit [0....1]
                    float unit = angle/(2*PI);
                    if (unit < 0) {
                        unit += 1;
                    }
                    mCenterPaint.setColor(interpColor(mColors, unit));
                    invalidate();
				}
                break;

			case MotionEvent.ACTION_UP:
				mColor = mCenterPaint.getColor();
                break;
			}

			return true;
		}
	}

    @Override
    protected void onSetInitialValue(boolean restore, Object defaultValue)  
    {
        super.onSetInitialValue(restore, defaultValue);
        if (restore) 
            mColor = shouldPersist() ? getPersistedInt(Color.BLACK) : Color.BLACK;
        else 
            mColor = Color.BLACK;
    }
	
	protected View onCreateDialogView()
	{
        LinearLayout.LayoutParams params;
        LinearLayout layout = new LinearLayout(mContext);
        layout.setOrientation(LinearLayout.VERTICAL);
        layout.setPadding(6,6,6,6);

        mSplashText = new TextView(mContext);
        if (mDialogMessage != null)
            mSplashText.setText(mDialogMessage);
        layout.addView(mSplashText);

        mValueText = new TextView(mContext);
        mValueText.setGravity(Gravity.CENTER_HORIZONTAL);
        mValueText.setTextSize(32);
        params = new LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT, 
                LinearLayout.LayoutParams.WRAP_CONTENT);
        layout.addView(mValueText, params);
        
        if (shouldPersist())
            mColor = getPersistedInt(Color.BLACK);

        ColorPickerView mColorPicker = new ColorPickerView(getContext(), mColor);
		layout.addView(mColorPicker, new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT));



        return layout;
	}
	
    // ------------------------------------------------------------------------------------------
    // Set the positive button listener and onClick action : 
    @Override
    public void showDialog(Bundle state) {

        super.showDialog(state);

        Button positiveButton = ((AlertDialog) getDialog()).getButton(AlertDialog.BUTTON_POSITIVE);
        positiveButton.setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {

        if (shouldPersist())
            persistInt(mColor);
        callChangeListener(Integer.valueOf(mColor));

        ((AlertDialog) getDialog()).dismiss();
    }
    // ------------------------------------------------------------------------------------------
}