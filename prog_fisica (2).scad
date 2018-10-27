echo(version=version());

module guide_shape(){
        difference(){
        square([30, 100]);
        translate([0,30])
            square([20,10]);
        translate([0,60])
            square([20,10]);
    }
}

module sensor(){
    linear_extrude(height = 15)
        square([10, 30]);
    
}
module engine(){
    color("purple")
        linear_extrude(height = 40)
            square([30, 50]);
        translate([30,25,20]){
        rotate([90,0,90])
        color("purple")
        
            cylinder(  20,   5, 5,false);
        }
    
}
// simple 2D -> 3D extrusion of a rectangle
color("green")
linear_extrude(height = 500)
    guide_shape();

translate([-140,0,0]){
    color("green")
    linear_extrude(height = 500)
        rotate([0,180,0])
            guide_shape();

    }
    
translate([-170, 0, -20])
    color("red")
    difference(){
        linear_extrude(height = 20)
            square([200, 100]);
        translate([95,50,5])
           linear_extrude(height = 5)
        square([10, 30]);
    }
    
translate([-160, 30, 0])
    color("cyan")
        linear_extrude(height = 480)
            square([180, 10]);

 translate([-75,50,-10])
    color("purple")
    sensor();

translate([-170, 0, 500])
    color("red")
    difference(){
        linear_extrude(height = 10)
            square([200, 100]);
        translate([15,60,-2])
            linear_extrude(height = 20)
                square([170, 10]);
    }
translate([-55, 50, 485])
      rotate([0,0,90])
      color("purple")
    sensor();
    
    
translate([-160, 60, 300])
    color("black")
        linear_extrude(height = 480)
            square([180, 10]);

translate([-105, 0, 510])
    engine();