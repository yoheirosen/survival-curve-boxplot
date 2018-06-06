var viz_width = 500;
var viz_height = 200;
var margin = 10;
var left_margin = 20;
var bottom_margin = 25;

var eggplant = "#820082";

var selected_ids = [];

d3.select(window).on("load", getProcessedData());

function processData() {
  child_process.execSync()
}

function getProcessedData() {
  d3.select(".svg-container").remove();

  d3.json("data.json", function(data) {
    var patients = data.patients;
    var t_min = data.ranges.t0;
    var t_max = data.ranges.t1;
    var v_min = data.ranges.v0;
    var v_max = data.ranges.v1;
    console.log(t_min);
    console.log(v_min);
    console.log(t_max);
    console.log(v_max);

    var viz = d3.select("div#viz")
      .append("div")
      .classed("svg-container", true)
      .style("width", "96%")
      .style("padding-bottom", (viz_height / viz_width) * 96 + "%")
      .append("svg:svg")
      .attr("preserveAspectRatio", "xMinYMin meet")
      .attr("viewBox", "0 0 "+viz_width+" "+viz_height)
      .classed("svg-content-responsive", true)
      .append("svg:g");

    var x = d3.scale.linear().domain([t_min, t_max]).range([0 + left_margin + margin, viz_width - margin]);
    var y = d3.scale.linear().domain([v_min, v_max]).range([viz_height - margin - bottom_margin, 0 + margin]);

    var xAxis = d3.svg.axis()
      .scale(x)
      .orient("bottom")
      .ticks(5);

    viz.append("g")
      .attr("transform", "translate(0," + (viz_height - margin - bottom_margin) + ")")
      .attr("class", "axis")
      .call(xAxis);

    var yAxis = d3.svg.axis()
      .scale(y)
      .orient("left")
      .ticks(4);

    viz.append("g")
      .attr("transform", "translate(" + (margin + left_margin) + ",0)")
      .attr("class", "axis")
      .call(yAxis);

    var contours_points = [];
    var contours_densities = [];
    var contours_ids = [];

    var midpoint_line_data_array = [];

    for(i = 0; i < patients.length; i++) {
      var patient = patients[i];
      var mid = patient.midpoint;
      var far1 = patient.upper_far;
      var close1 = patient.upper_close;
      var close0 = patient.lower_close;
      var far0 = patient.lower_far;
      var times = [];
      var values = [];
      var midpoint_line_data = [];
      for(j = 0; j < mid.t.length; j++) {
        times.push(mid.t[j]);
        values.push(mid.v[j]);
        midpoint_line_data.push({x: mid.t[j], y : mid.v[j]});
      }
      console.log(times);
      console.log(values);
      console.log(midpoint_line_data);

      midpoint_line_data_array.push(midpoint_line_data);

      var line = d3.svg.line()
        .x(function(d) { return x(d.x); })
        .y(function(d) { return y(d.y); })
        .interpolate("linear");

      if(patient.censored == true) {
        viz.append("circle")
          .attr("cx", x(mid.t[mid.t.length - 1]))
          .attr("cy", y(mid.v[mid.t.length - 1]))
          .attr("r", 1.5)
          .attr("fill", "none")
          .attr("stroke", eggplant)
          .attr("stroke-width", 0.5)
      }

      var contours = patient.contours;

      for(j = 0; j < contours.length; j++) {
        contours_densities.push(contours[j].density);
        var contour_points = [];
        for(k = 0; k < contours[j].t0.length; k++) {
          contour_points.push({x: contours[j].t0[k], y: contours[j].v0[k]});
        }
        for(k = contours[j].t1.length - 1; k > -1; k--) {
          contour_points.push({x: contours[j].t1[k], y: contours[j].v1[k]});
        }
        contours_points.push(contour_points);
        contours_ids.push(patient.name);
      }
    }

    for(i = 0; i < contours_ids.length; i++) {
      viz.append("path")
        .attr("fill", eggplant)
        .attr("fill-opacity", +contours_densities[i])
        .attr("class", "difference")
        .attr("d", line(contours_points[i]))
        .classed("patient_id_"+contours_ids[i], true)
        .classed("patient_contour", true);
    }

    for(i = 0; i < patients.length; i++) {
      for(j = 0; j < patients[i].events.length; j++) {
        viz.append("circle")
          .attr("cx", x(patients[i].midpoint.t[patients[i].events[j].index]))
          .attr("cy", y(patients[i].midpoint.v[patients[i].events[j].index]))
          .attr("r", 2)
          .classed("patient_id_"+patients[i].name, true)
          .classed("hidden_event", true)
          .attr("fill-opacity", 0);

        viz.append("text")
          .style("text-anchor", "middle")
          .attr("x", x(patients[i].midpoint.t[patients[i].events[j].index]))
          .attr("y", y(patients[i].midpoint.v[patients[i].events[j].index]) - 5)
          .classed("patient_id_"+patients[i].name, true)
          .classed("hidden_event", true)
          .text(patients[i].events[j].type)
          .attr("fill-opacity", 0)
          .attr("font-size", "8px");
      }
    }

    for(i = 0; i < patients.length; i++) {
      var patient = patients[i];

      viz.append("path")
        .attr("d", line(midpoint_line_data_array[i]))
        .attr("stroke", eggplant)
        .attr("stroke-opacity", 0.3)
        .attr("stroke-width", 1)
        .attr("fill", "none")
        .classed("patient_id_"+patient.name, true)
        .classed("patient_midline", true)
        .classed("sex_"+patient.sex, true)
        .on("mouseover", lineMouseOver)
        .on("mouseout", lineMouseOut);
    }
  });
}

function lineMouseOver(d, i) {
  var classes = d3.select(this).attr("class").split(" ");
  var patient_class = classes[0];
  d3.selectAll("."+patient_class)
    .classed("mousedOver", true);
  var selected = d3.selectAll(".mousedOver");
  selected.each(function(d,i) {
    var this_object = d3.select(this);
    if(this_object.classed("hidden_event")) {
      this_object.attr("fill-opacity", 1);
    } else {
      this_object.attr("stroke-opacity", this_object.attr("stroke-opacity") * 2);
      this_object.attr("fill-opacity", this_object.attr("fill-opacity") * 2);
    }
  });
}

function lineMouseOut(d, i) {
  var selected = d3.selectAll(".mousedOver");
  selected.each(function(d,i) {
    var this_object = d3.select(this);
    if(this_object.classed("hidden_event")) {
      this_object.attr("fill-opacity", 0);
    } else {
      this_object.attr("stroke-opacity", this_object.attr("stroke-opacity") / 2);
      this_object.attr("fill-opacity", this_object.attr("fill-opacity") / 2);
    }
  });

  d3.selectAll(".mousedOver")
    .classed("mousedOver", false);
}
