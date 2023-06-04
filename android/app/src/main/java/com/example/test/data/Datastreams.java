package com.example.test.data;

import com.example.test.data.Datapoints;

import java.util.List;

public class Datastreams {
    private List<Datapoints> datapoints;
    private String id;
    public void setDatapoints(List<Datapoints>datapoints){this.datapoints=datapoints;}
    public List<Datapoints> getDatapoints(){return datapoints;}
}