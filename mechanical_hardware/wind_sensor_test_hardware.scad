


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


transducer_z_spacing = 250;

//transducer_radius_from_center = sqrt(2* pow(transducer_xy_spacing/2,2));
transducer_radius_from_center = 75;

transducer_xy_spacing = 2* transducer_radius_from_center * sin(45);

flight_distance = 2 * sqrt(
   pow(transducer_radius_from_center,2) + pow(transducer_z_spacing,2));

echo("flight distance = " , flight_distance, "mm");

module transducer_arrange(){
  for ( i = [-1:2:1]){
     for( j = [-1:2:1]){
        translate([i * transducer_xy_spacing / 2,j * transducer_xy_spacing / 2, 0]){
           rotate([0,0, -atan2(i,j)]){
              children();
           };
        }
     }
  }
}

module transducer_mount()
{
   difference(){
      //cube([transducer_xy_spacing + 50,transducer_xy_spacing + 50, 3], center =  true);
      cylinder(d = transducer_radius_from_center*2 + 50, h = 3, $fn = 50);

      translate([0,0,-10]){
         transducer_arrange(){
            cylinder(d = 25, h = 25, $fn = 20);
         }
      }
   }

   // echo plate
   translate([0,0,transducer_z_spacing]){
     cylinder ( d = 62, h = 6,$fn = 50);
   }
   // stem
   translate([0,0,-20]){
   cylinder( d= 10, h = transducer_z_spacing + 20);
   }

   // stem mount
   translate([0,0,-18]){
     cylinder ( d = 50, h = 18,$fn = 50);
   }
}

transducer_mount();

translate([0,0,3]){
  transducer_arrange()
    rotate([0,0,0]){
    transducer();
    }
}
