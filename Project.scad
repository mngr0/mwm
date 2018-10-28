echo(version=version());
module sensor(){
    linear_extrude(height = 25)
        square([20, 50]);
    
}

module rounded_square( width, radius_corner ) {
	translate( [ radius_corner, radius_corner, 0 ] )
		minkowski() {
			square( width - 2 * radius_corner );
			circle( radius_corner ,$fn=30);
		}
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

module hole(d,y,z,h){ 
    translate([0,y,z])
        rotate([0,90,0])
            linear_extrude(height = h)
                circle(d,$fn=30);
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
    difference(){
        translate([170,0,30])
            guide();
        
    }
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

module maschera(){
        vite=2.5/2;
        buco=20/2;
        centroZ=300;
        centroY=35-buco+1;
        distanzaViti=31;
        scavo=43;
    
        hole(vite,centroY+distanzaViti/2,centroZ+distanzaViti/2,100);
        hole(vite,centroY+distanzaViti/2,centroZ-distanzaViti/2,100);        
        hole(vite,centroY-distanzaViti/2,centroZ+distanzaViti/2,100);
        hole(vite,centroY-distanzaViti/2,centroZ-distanzaViti/2,100);
        hole(buco,centroY,centroZ,200);
        translate([0,centroY-scavo/2,centroZ+scavo/2])
            rotate([0,90,0])
                linear_extrude(height=22)
                    rounded_square(scavo,2);

    }

module total(){
    difference(){
        basePlusGuide();
        maschera();
    }
        //translate([0,0,300])   
    //    topSide();
}
total();

