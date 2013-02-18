package com.inappropirates.remotecontrol;	

import android.annotation.SuppressLint;
import android.app.Dialog;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.RectF;
import android.graphics.SweepGradient;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;

public class ColorPickerDialog extends Dialog {
	private int mColor = Color.BLACK;
	
	public ColorPickerDialog(Context context,
              OnColorChangedListener listener,
              int initialColor) {
		super(context);
		
		if (listener != null)
			externalListener = listener;
		
		mColor = initialColor;
	}
	
	// Color change listener.
	private OnColorChangedListener mListener;
	private OnColorChangedListener externalListener;

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
		private static final int CENTER_X = 150;
		private static final int CENTER_Y = 150;
		private static final int CENTER_RADIUS = 50;

		private Paint paint = null;
		private Paint mCenterPaint = null;
		private final int[] mColors = new int[] {
			0xFFFF0000, 0xFFFF00FF, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000,
			0xFF00FFFF, 0xFF00FF00,	0xFFFFFF00, 0xFFFF0000 };

//			0xFFFF0000, 0xFFFF00FF, 0xFF0000FF, 0xFF00FFFF, 0xFF00FF00,
//			0xFFFFFF00, 0xFFFF0000 };

		/**
		 * @param context
		 * @param listener
		 * @param color
		 */
		ColorPickerView(Context context, OnColorChangedListener listener,
				int color) {
			super(context);


			mListener = listener;

			paint = new Paint(Paint.ANTI_ALIAS_FLAG);
			paint.setShader(new SweepGradient(0, 0, mColors, null));
			paint.setStyle(Paint.Style.STROKE);
			paint.setStrokeWidth(85);

			mCenterPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
			mCenterPaint.setColor(color);
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
				(int)(paint.getStrokeWidth()/2);
			float r = CENTER_X - paint.getStrokeWidth()*0.5f;

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

			setMeasuredDimension(width, CENTER_Y*2);
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
					mTrackingCenter = inCenter;
	                if (inCenter) {
	                    mHighlightCenter = true;
	                    invalidate();
	                }
                    break;
                    
			case MotionEvent.ACTION_MOVE:
                if (mTrackingCenter) {
                    if (mHighlightCenter != inCenter) {
                        mHighlightCenter = inCenter;
                        invalidate();
                    }
                } else {
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
                if (mTrackingCenter) {
                	// <cgerstle> uncomment these to require the click of the center to pick the color
                    //if (inCenter) {
                        mListener.colorChanged(mCenterPaint.getColor());
                    //}
                    mTrackingCenter = false;    // so we draw w/o halo
                    invalidate();
                }
                break;
			}

			return true;
		}
	}

	
	/**
	 * {@inheritDoc}
	 */
	/*
	protected void onDialogClosed(boolean positiveResult) {
		// Persist the color after the ok button is clicked.
		//if (positiveResult && color != 0) {
			//persistInt(color);
		//}

		super.onDialogClosed(positiveResult);
	}
	*/
	/**
	 * {@inheritDoc}
	 */
	protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
		mListener = new OnColorChangedListener() {
			/**
			 * {@inheritDoc}
			 */
			public void colorChanged(int c) {
				ColorPickerDialog.this.mColor = c;
				externalListener.colorChanged(ColorPickerDialog.this.mColor);
				ColorPickerDialog.this.dismiss();
			}
		};

		setContentView(new ColorPickerView(getContext(), mListener, mColor));
		setTitle(R.string.pick_color);
	}
}