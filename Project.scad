echo(version=version());

vite=2.5/2;
diametro_rullo = 7;
buco=diametro_rullo+1;

centroZ=300;
centroY=35-buco+2;
distanzaViti=31;
lato_motore=41;
scavo=lato_motore+2;

engine_rot = 21;
rot_diam=5;

module sensor(){
    linear_extrude(height = 25)
        square([20, 50]);
    
}

module rounded_square( width,height, radius_corner ) {
	translate( [ radius_corner, radius_corner, 0 ] )
		minkowski() {
			square( [width - 2 * radius_corner,height - 2 * radius_corner]  );
			circle( radius_corner ,$fn=30);
		}
}

module engine(){
        linear_extrude(height = lato_motore)
            square([30, lato_motore]);
        translate([30,lato_motore/2,lato_motore/2]){
            rotate([90,0,90])
                cylinder(  engine_rot,   rot_diam, rot_diam,false);
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

module maschera(){
        offset=20;
    
        hole(vite,centroY+distanzaViti/2,centroZ+distanzaViti/2,100);
        hole(vite,centroY+distanzaViti/2,centroZ-distanzaViti/2,100);        
        hole(vite,centroY-distanzaViti/2,centroZ+distanzaViti/2,100);
        hole(vite,centroY-distanzaViti/2,centroZ-distanzaViti/2,100);
        hole(buco,centroY,centroZ,200);
        translate([0,centroY-scavo/2,centroZ+scavo/2+offset])
            rotate([0,90,0])
                linear_extrude(height=22)
                    rounded_square(scavo+offset,scavo,2);
    }

module scanalature_elettronica(){
        //node
        translate([195,55,300])
          rotate([0,90,0])
                linear_extrude(height=10)
                    rounded_square(50,25,2);
        //drv
        translate([0,30,250])
            rotate([0,90,0])
                linear_extrude(height=5)
                    rounded_square(20,15,2);
    }
    
module buchi_cavi(){
    //front hole
    translate([105,10,0])
        rotate([-90,0,0])
            cylinder(  150,   10, 10,false,$fn=30);
    //laterals bottom
    translate([-5,15,0])
        rotate([0,90,0])
            cylinder(  210,   10, 10,false,$fn=30);
    
    //side 1
    translate([10,5,-5])
        linear_extrude(height=205)
            rounded_square(15,15,2);
    //side 2
    translate([175,5,-5])
        linear_extrude(height=240)
            rounded_square(15,15,2);
    //exit 1        
    translate([-20,12.5,200])
        rotate([0,90,0])
            cylinder(  45,   7.5, 7.5,false,$fn=30);        
    //exit 2
    translate([175,12.5,235])
        rotate([0,90,0])
            cylinder(  45,   7.5, 7.5,false,$fn=30);             
    }    
    

module rullo(){
    translate([25,centroY,centroZ])
            difference(){
            rotate([0,90,0])
                difference(){
                    cylinder(  175,   diametro_rullo, diametro_rullo,false);
                    cylinder(  20,   rot_diam+1, rot_diam+1,false);
                }
                translate([30,0,-10])
                    cylinder(  20,   2, 2,false,$fn=30);
                translate([80,0,-10])
                    cylinder(  20,   2, 2,false,$fn=30);
                translate([130,0,-10])
                    cylinder(  20,   2, 2,false,$fn=30);
            }
    }

module total(){
    difference(){
       basePlusGuide();
       maschera();
       scanalature_elettronica();
      buchi_cavi();
    }
    translate ([0,-200,0])
        buchi_cavi();
    //rullo();
    //translate([-8,centroY-lato_motore/2,centroZ-lato_motore/2])
    //    engine();
}



total();

