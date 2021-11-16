using System;
/*public bool collision_detection(float hLat, float hLong, float hSpeed, float oLat, float oLong) {
  //Assuming speed is in kilometers per hour
  bool warning = false;
  char letter; //8 degree vertical segments of the UTM grid
  int projection_zone; //6 degree horizontal segments of UTM
  float rad_equator = 6378137, rad_poles = 6356752.314245,rad_lat;
  float lat_circumference; //the circumference of a circle sliced out of the earth with a plane perpendicular to the axis of rotation
  float react_distance, brake_distance, warning_distance, car_gap;
  float[] lats = new float[] {hLat, oLat};
  float[] longs = new float {hLong, oLong};
  react_distance = hSpeed/3.6; //convert from kM/h to meters/second
  brake_distance = (hSpeed*hSpeed)/(250*0.8); //250 is fixed value and 0.8 is coefficient of friction on dry asphalt
  warning_distance = (1.5*hSpeed)+react_distance+brake_distance; //Adding 1.5 seconds of extra reaction time for a total of 2.5 seconds to react

  if (hLat < -72) {
    letter = "C";
  }
  else if (hLat < -64) {
    letter = "D";
  }
  else if (hLat < -56) {
    letter = "E";
  }
  else if (hLat < -48) {
    letter = "F";
  }
  else if (hLat < -40) {
    letter = "G";
  }
  else if (hLat < -32) {
    letter = "H";
  }
  else if (hLat < -24) {
    letter = "J";
  }
  else if (hLat < -16) {
    letter = "K";
  }
  else if (hLat < -8) {
    letter = "L";
  }
  else if (hLat < 0) {
    letter = "M";
  }
  else if (hLat < 8) {
    letter = "N";
  }
  else if (hLat < 16) {
    letter = "P";
  }
  else if (hLat < 24) {
    letter = "Q";
  }
  else if (hLat < 32) {
    letter = "R";
  }
  else if (hLat < 40) {
    letter = "S";
  }
  else if (hLat < 48) {
    letter = "T";
  }
  else if (hLat < 56) {
    letter = "U";
  }
  else if (hLat < 64) {
    letter = "V";
  }
  else if (hLat < 8) {
    letter = "W";
  }
  else {
    letter = "X";
  }
  projection_zone = (Math.Floor(hLong)/6)+31;
  float hEasting, hNorthing, oEasting, oNorthing; //UTM coordinates for each vehicle
  rad_lat = Math.Sqrt(Math.Pow(rad_equator*Math.Cos(hLat*Math.PI/180),2)+Math.Pow(rad_poles*Math.Sin(hLat*Math.PI/180),2));
  lat_circumference = 2*Math.PI*(Math.Cos(hLat*Math.PI/180)*rad_lat);
  if (((hLong-6*projection_zone)-3) < 0) {
    hEasting = 500000-Math.Abs(hLong-6*projection_zone)*lat_circumference;
  }
  else if (((hLong-6*projection_zone)-3) > 0) {
    hEasting = 500000+Math.Abs(hLong-6*projection_zone)*lat_circumference;
  }
  else {
    hEasting = 500000;
  }
}*/

class Program {
  public static void Main (string[] args) {
    

  }
}