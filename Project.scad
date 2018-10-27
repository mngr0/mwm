echo(version=version());
module sensor(){
    linear_extrude(height = 25)
        square([20, 50]);
    
}
module engine(){
        linear_extrude(height = 40)
            square([30, 50]);
        translate([30,25,20]){
        rotate([90,0,90])
        color("purple")
        
            cylinder(  20,   5, 5,false);
        }
    
}
module guide(){
    linear_extrude(height = 300)
            difference(){
        square([30, 100]);
        translate([0,30])
            square([20,10]);
        translate([0,60])
            square([20,10]);
    }
}

module baseSide(){
 difference(){
        linear_extrude(height = 30)
            square([200, 100]);
        translate([110,40,31])
             rotate([0,180,0])
           linear_extrude(height = 21)
        square([20, 50]);
    }
}   
module basePlusGuide(){
baseSide();
    translate([170,0,30])
guide();
    translate([30,100,30])
    rotate([0,0,180])
guide();
    
}
module topSide(){
    difference(){
        linear_extrude(height = 30)
            square([200, 100]);
        translate([15,60,-1])
            linear_extrude(height = 32)
                square([170, 10]);
    }
}
module total(){
    basePlusGuide();
    translate([0,0,300])   
        topSide();
}
total();