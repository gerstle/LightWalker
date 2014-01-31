package com.inappropirates.remotecontrol;

import java.util.ArrayList;

import android.content.Context;
import android.widget.ArrayAdapter;
import android.widget.ListView;

public class ListLogger {
    private ArrayList<String> mListItems = new ArrayList<String>();
    private ArrayAdapter<String> mAdapter;
    private ListView mListView;
    
    public ListLogger(Context context, ListView listView)
    {    	
    	mListView = listView;
    	mAdapter = new ArrayAdapter<String>(context, android.R.layout.simple_list_item_1, mListItems);
        mListView.setAdapter(mAdapter);	
    }
    
    public void Log(String msg)
    {
    	if (mListItems.size() > 75)
    		mListItems.clear();
    	
    	mListItems.add(msg);
    	mAdapter.notifyDataSetChanged();
    	mListView.setSelection(mListView.getCount() - 1);
    }
}
