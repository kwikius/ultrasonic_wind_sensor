module transducer()
{
 
   color([0.3,0.3,0.1]){

      difference(){
         union(){
            translate([0,0,-17]){
               cylinder ( d = 21, h = 18, $fn = 20);
            }

            translate([0,0,0]){
               cylinder ( d = 24, h = 1.5, $fn = 20);
            }

            translate([0,6,-18.5]){
               cylinder(d = 6, h = 2, $fn = 20);
            }

            translate([0,6,-(17 + 10)]){
               cylinder(d = 3, h = 10, $fn = 20);
            }
         }
         translate([0,0,0,]){
            cylinder ( d = 12,h = 3,$fn = 20);
         }
      }
   }

}


show_transducer = true;

module housing(angle)
{

 transducer_depth = 30;
 tube_length = 30;
   if (show_transducer){
         rotate([angle,0,0]){
           translate([0,0,30]){
           transducer();
         }
      }
   }
  difference(){
   union(){
      sphere( d = 25);
      rotate([angle,0,0]){
        cylinder(d = 25, h = transducer_depth);
      }
      rotate([90,0,0]){
         cylinder( d = 15, h = tube_length);
      }
   }
   union(){
      sphere( d = 21);
      rotate([angle,0,0]){
         translate([0,0,1]){
           cylinder(d = 22, h = transducer_depth + 2);
         }
      }
      rotate([90,0,0]){
        translate([0,0,-1]){
          cylinder( d = 12, h = tube_length + 2);
       }
      }
   }
  }

}

module bun(radius, thickness)
{
   rotate_extrude(){
      union(){
      translate([0,-thickness/2,0]){
         square([radius, thickness]);
      }
      translate([radius,0,0]){
        circle( d = thickness, $fn = 50);
      }
      }
   }
}

module sensor_hub(){

  hull(){
  scale([1,1,0.3]) {
   sphere(d = 165, $fn = 100);
  }
  translate([0,0,20]){
    bun(60,15);
  }
  }
}

module spine()
{
  translate([0,0,-280]){
   cylinder ( d = 15, h = 300);
  }
  translate([0,0,-(280 + 210)]){
   cylinder ( d = 25, h = 300);
  }
}

module solar_panel()
{
  r_inner = 20;
  r_outer = 60;
  l_inner = 7;
  l_outer = 25;
  polygon( points = [
   [r_inner,l_inner/2],
   [r_inner,-l_inner/2],
   [r_outer,-l_outer/2],
   [r_outer,l_outer/2]
  ]);
}

module solar_panels()
{
  num_elements = 14;
  angle = 360/num_elements;
color("blue"){
  for ( i = [0:1:num_elements]){
    rotate([0,0,angle * i ]){
      solar_panel();
    }
  }
}
}

module echo_plate()
{
   radius = 30;
   thickness = 20;

   bun(radius, thickness);
}

sensor_hub();
rotate([180,0,0]){
for ( i = [0:1:3]){
rotate( [0,0,90 * i]){
translate([0, 110,0]){
housing(35);
}
}
}
}

spine();

translate([0,0, -180]){
   echo_plate();
}

translate([0,0,28]){
 solar_panels();
}



