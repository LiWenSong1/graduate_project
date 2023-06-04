package com.example.test;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.example.test.bean.BeforeDataBean;

import java.util.List;

public class ListAdapter extends BaseAdapter {
    Context context;
    List<BeforeDataBean> hisData;

    public ListAdapter(Context context, List<BeforeDataBean> hisData) {
        this.context = context;
        this.hisData = hisData;
    }

    @Override
    public int getCount() {
        return hisData.size();
    }

    @Override
    public Object getItem(int i) {
        return hisData.get(i);
    }

    @Override
    public long getItemId(int i) {
        return i;
    }

    @Override
    public View getView(int i, View view, ViewGroup viewGroup) {
        ViewHolder holder=null;
        if(view==null){
            view= LayoutInflater.from(context).inflate(R.layout.list_view_item,null);//将布局转成view对象的方法

            holder=new ViewHolder(view);  //加载控件
            view.setTag(holder);         //将控件保存在view中，以便复用
        }else{
            holder= (ViewHolder) view.getTag();
        }
        //加载控件显示的内容
        BeforeDataBean beforeDataBean=hisData.get(i);
        holder.before_time.setText(beforeDataBean.getTime());
        holder.before_max_temp.setText(beforeDataBean.getMax_temp());
        holder.before_max_humid.setText(beforeDataBean.getMax_humid());
        holder.before_max_air.setText(beforeDataBean.getMax_air());
        holder.before_min_temp.setText(beforeDataBean.getMin_temp());
        holder.before_min_humid.setText(beforeDataBean.getMin_humid());
        holder.before_min_air.setText(beforeDataBean.getMin_air());
        return view;
    }
    class ViewHolder{
        TextView before_time,before_max_temp,before_min_temp,before_max_humid,before_min_humid,before_max_air,before_min_air;
        public ViewHolder(View view){
            before_max_humid=view.findViewById(R.id.before_max_humid);
            before_max_temp=view.findViewById(R.id.before_max_temp);
            before_time=view.findViewById(R.id.before_time);
            before_max_air=view.findViewById(R.id.before_max_air);
            before_min_air=view.findViewById(R.id.before_min_air);
            before_min_humid=view.findViewById(R.id.before_min_humid);
            before_min_temp=view.findViewById(R.id.before_min_temp);
        }
    }

}
